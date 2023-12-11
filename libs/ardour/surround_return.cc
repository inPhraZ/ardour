/*
 * Copyright (C) 2023 Robin Gareus <robin@gareus.org>
 * Copyright (C) 2023 Paul Davis <paul@linuxaudiosystems.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "ardour/surround_return.h"
#include "ardour/audio_buffer.h"
#include "ardour/lv2_plugin.h"
#include "ardour/route.h"
#include "ardour/session.h"
#include "ardour/surround_pannable.h"
#include "ardour/surround_send.h"
#include "ardour/uri_map.h"
#include "pbd/i18n.h"

using namespace ARDOUR;

SurroundReturn::SurroundReturn (Session& s)
	: Processor (s, _("SurrReturn"), Temporal::TimeDomainProvider (Temporal::AudioTime))
	, _current_n_objects (max_object_id)
	, _in_map (ChanCount (DataType::AUDIO, 128))
	, _out_map (ChanCount (DataType::AUDIO, 14))
{
#if !(defined(LV2_EXTENDED) && defined(HAVE_LV2_1_10_0))
	throw failed_constructor ();
#endif

	_surround_processor = std::dynamic_pointer_cast<LV2Plugin> (find_plugin (_session, "urn:ardour:a-vapor", ARDOUR::LV2));

	if (!_surround_processor) {
		throw failed_constructor ();
	}

	_flush.store (0);
	_surround_processor->activate ();
	_surround_bufs.ensure_buffers (DataType::AUDIO, 128, s.get_block_size ());
	_surround_bufs.set_count (ChanCount (DataType::AUDIO, 128));

	lv2_atom_forge_init (&_forge, URIMap::instance ().urid_map ());

	for (size_t i = 0; i < max_object_id; ++i) {
		_current_render_mode[i] = -1;
		for (size_t p = 0; p < num_pan_parameters; ++p) {
			_current_value[i][p] = -1111; /* some invalid data that forces an update */
		}
	}
}

SurroundReturn::~SurroundReturn ()
{
}

int
SurroundReturn::set_block_size (pframes_t nframes)
{
	_surround_bufs.ensure_buffers (DataType::AUDIO, 128, nframes);
	_surround_processor->set_block_size (nframes);
	return 0;
}

samplecnt_t
SurroundReturn::signal_latency () const
{
	return _surround_processor->signal_latency ();
}

void
SurroundReturn::flush ()
{
	_flush.store (1);
}

void
SurroundReturn::run (BufferSet& bufs, samplepos_t start_sample, samplepos_t end_sample, double speed, pframes_t nframes, bool)
{
	if (!check_active ()) {
		return;
	}

	int canderef (1);
	if (_flush.compare_exchange_strong (canderef, 0)) {
		_surround_processor->flush ();
	}

	bufs.set_count (_configured_output);
	_surround_bufs.silence (nframes, 0);

	RouteList rl = *_session.get_routes (); // XXX this allocates memory
	rl.sort (Stripable::Sorter (true));

	size_t id = 10; // First 10 IDs are reseved for bed mixes

	for (auto const& r : rl) {
		std::shared_ptr<SurroundSend> ss;
		if (!r->active ()) {
			continue;
		}
		if (!(ss = r->surround_send ()) || !ss->active ()) {
			continue;
		}

		timepos_t start, end;

		for (uint32_t s = 0; s < ss->bufs ().count ().n_audio () && id < max_object_id; ++s, ++id) {

			std::shared_ptr<SurroundPannable> const& p (ss->pan_param (s, start, end));
			AutoState const as = p->automation_state ();
			bool const automated = (as & Play) || ((as & (Touch | Latch)) && !p->touching ());

			AudioBuffer&       dst_ab (_surround_bufs.get_audio (id));
			AudioBuffer const& src_ab (ss->bufs ().get_audio (s));
			if (id > 9) {
				/* object */
				dst_ab.read_from (src_ab, nframes);
				if (!automated || start_sample >= end_sample) {
					pan_t const v[num_pan_parameters] =
					{
						(pan_t)p->pan_pos_x->get_value (),
						(pan_t)p->pan_pos_y->get_value (),
						(pan_t)p->pan_pos_z->get_value (),
						(pan_t)p->pan_size->get_value (),
						(pan_t)p->pan_snap->get_value ()
					};
					maybe_send_metadata (id, 0, v);
				} else {
					/* Evaluate Automation
					 *
					 * Note, exclusive end: range = [start_sample, end_sample[
					 * nframes == end_sample - start_sample
					 * IOW: end_sample == next cycle's start_sample;
					 */
					if (nframes < 2) {
						evaluate (id, p, timepos_t (start_sample), 0);
					} else {
						timepos_t start (start_sample);
						timepos_t end (end_sample - 1);
						while (true) {
							Evoral::ControlEvent next_event (timepos_t (Temporal::AudioTime), 0.0f);
							if (!p->find_next_event (start, end, next_event)) {
								break;
							}
							samplecnt_t pos = std::min (timepos_t (start_sample).distance (next_event.when).samples(), (samplecnt_t) nframes - 1);
							evaluate (id, p, next_event.when, pos);
							start = next_event.when;
						}
						/* end */
						evaluate (id, p, end, nframes - 1);
					}
				}
				/* configure near/mid/far - not sample-accurate */
				int const brm = p->binaural_render_mode->get_value ();
				if (brm!= _current_render_mode[id]) {
					_current_render_mode[id] = brm;
#if defined(LV2_EXTENDED) && defined(HAVE_LV2_1_10_0)
					URIMap::URIDs const& urids = URIMap::instance ().urids;
					forge_int_msg (urids.surr_Settings, urids.surr_Channel, id, urids.surr_BinauralRenderMode, brm);
#endif
				}

			} else {
				/* bed mix */
				dst_ab.merge_from (src_ab, nframes);
			}

		}

		if (id >= max_object_id) {
			break;
		}
	}

	if (_current_n_objects != id) {
		_current_n_objects = id;
#if defined(LV2_EXTENDED) && defined(HAVE_LV2_1_10_0)
		URIMap::URIDs const& urids = URIMap::instance ().urids;
		forge_int_msg (urids.surr_Settings, urids.surr_ChannelCount, _current_n_objects);
#endif
	}

	_surround_processor->connect_and_run (_surround_bufs, start_sample, end_sample, speed, _in_map, _out_map, nframes, 0);

	BufferSet::iterator i = _surround_bufs.begin (DataType::AUDIO);
	for (BufferSet::iterator o = bufs.begin (DataType::AUDIO); o != bufs.end (DataType::AUDIO); ++i, ++o) {
		o->read_from (*i, nframes);
	}
}

void
SurroundReturn::forge_int_msg (uint32_t obj_id, uint32_t key, int val, uint32_t key2, int val2)
{
	URIMap::URIDs const& urids = URIMap::instance ().urids;
	LV2_Atom_Forge_Frame frame;
	lv2_atom_forge_set_buffer (&_forge, _atom_buf, sizeof(_atom_buf));
	lv2_atom_forge_frame_time (&_forge, 0);
	LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object (&_forge, &frame, 1, obj_id);
	lv2_atom_forge_key (&_forge, key);
	lv2_atom_forge_int (&_forge, val);
	if (key2 > 0) {
		lv2_atom_forge_key (&_forge, key2);
		lv2_atom_forge_int (&_forge, val2);
	}
	lv2_atom_forge_pop (&_forge, &frame);
	_surround_processor->write_from_ui (0, urids.atom_eventTransfer, lv2_atom_total_size (msg), (const uint8_t*)msg);
}

void
SurroundReturn::maybe_send_metadata (size_t id, pframes_t sample, pan_t const v[num_pan_parameters])
{
	bool changed = false;
	for (size_t i = 0; i < num_pan_parameters; ++i) {
		if (_current_value[id][i] != v[i]) {
			changed = true;
		}
		_current_value[id][i] = v[i];
	}
	if (!changed) {
		return;
	}
	URIMap::URIDs const& urids = URIMap::instance ().urids;

#if defined(LV2_EXTENDED) && defined(HAVE_LV2_1_10_0)
			LV2_Atom_Forge_Frame frame;
			lv2_atom_forge_set_buffer (&_forge, _atom_buf, sizeof(_atom_buf));
			lv2_atom_forge_frame_time (&_forge, 0);
			LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object (&_forge, &frame, 1, urids.surr_MetaData);
			lv2_atom_forge_key (&_forge, urids.time_frame);
			lv2_atom_forge_int (&_forge, sample);
			lv2_atom_forge_key (&_forge, urids.surr_Channel);
			lv2_atom_forge_int (&_forge, id);
			lv2_atom_forge_key (&_forge, urids.surr_PosX);
			lv2_atom_forge_float (&_forge, v[0]);
			lv2_atom_forge_key (&_forge, urids.surr_PosY);
			lv2_atom_forge_float (&_forge, v[1]);
			lv2_atom_forge_key (&_forge, urids.surr_PosZ);
			lv2_atom_forge_float (&_forge, v[2]);
			lv2_atom_forge_key (&_forge, urids.surr_Size);
			lv2_atom_forge_float (&_forge, v[3]);
			lv2_atom_forge_key (&_forge, urids.surr_Snap);
			lv2_atom_forge_bool (&_forge, v[4]> 0 ? true : false);
			lv2_atom_forge_pop (&_forge, &frame);

			_surround_processor->write_from_ui (0, urids.atom_eventTransfer, lv2_atom_total_size (msg), (const uint8_t*)msg);
#endif
}

void
SurroundReturn::evaluate (size_t id, std::shared_ptr<SurroundPannable> const& p, timepos_t const& when, pframes_t sample)
{
	bool ok[num_pan_parameters];
	pan_t const v[num_pan_parameters] =
	{
		(pan_t)p->pan_pos_x->list()->rt_safe_eval (when, ok[0]),
		(pan_t)p->pan_pos_y->list()->rt_safe_eval (when, ok[1]),
		(pan_t)p->pan_pos_z->list()->rt_safe_eval (when, ok[2]),
		(pan_t)p->pan_size->list()->rt_safe_eval (when, ok[3]),
		(pan_t)p->pan_snap->list()->rt_safe_eval (when, ok[4])
	};
	if (ok[0] && ok[1] && ok[2] && ok[3] && ok[4]) {
		maybe_send_metadata (id, sample, v);
	}
}

bool
SurroundReturn::can_support_io_configuration (const ChanCount& in, ChanCount& out)
{
	out = ChanCount (DataType::AUDIO, 14); // 7.1.4 + binaural
	return in.n_total () == 0;
}

void
SurroundReturn::set_playback_offset (samplecnt_t cnt)
{
	Processor::set_playback_offset (cnt);
	std::shared_ptr<RouteList const> rl (_session.get_routes ());
	for (auto const& r : *rl) {
		std::shared_ptr<SurroundSend> ss = r->surround_send ();
		if (ss) {
			ss->set_delay_out (cnt);
		}
	}
}

XMLNode&
SurroundReturn::state () const
{
	XMLNode* node = new XMLNode (X_("SurroundReturn"));
	return *node;
}

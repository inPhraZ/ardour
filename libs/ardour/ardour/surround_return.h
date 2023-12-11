/*
 * Copyright (C) 2023 Paul Davis <paul@linuxaudiosystems.com>
 * Copyright (C) 2023 Robin Gareus <robin@gareus.org>
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

#ifndef __ardour_surround_return_h__
#define __ardour_surround_return_h__

#ifdef HAVE_LV2_1_18_6
#include <lv2/atom/atom.h>
#include <lv2/atom/forge.h>
#else
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#endif

#include "ardour/chan_mapping.h"
#include "ardour/processor.h"

namespace ARDOUR
{
class Session;
class SurroundSend;
class SurroundPannable;
class LV2Plugin;

class LIBARDOUR_API SurroundReturn : public Processor
{
public:
	SurroundReturn (Session&);
	virtual ~SurroundReturn ();

	bool can_support_io_configuration (const ChanCount& in, ChanCount& out);
	void run (BufferSet& bufs, samplepos_t start_sample, samplepos_t end_sample, double speed, pframes_t nframes, bool);
	int  set_block_size (pframes_t);
	void flush ();
	void set_playback_offset (samplecnt_t cnt);
	bool display_to_user () const { return false; }

	samplecnt_t signal_latency () const;

protected:
	XMLNode& state () const;

private:
	static const size_t max_object_id = 128; // happens to be the same as a constant in a well known surround system
	static const size_t num_pan_parameters = 5; // X, Y, Z, Size, Snap

	void forge_int_msg (uint32_t obj_id, uint32_t key, int val, uint32_t key2 = 0, int val2 = 0);
	void maybe_send_metadata (size_t id, pframes_t frame, pan_t const v[num_pan_parameters]);
	void evaluate (size_t id, std::shared_ptr<SurroundPannable> const&, timepos_t const& , pframes_t);

	std::shared_ptr<LV2Plugin> _surround_processor;

	LV2_Atom_Forge   _forge;
	uint8_t          _atom_buf[8192];
	pan_t            _current_value[max_object_id][num_pan_parameters];
	int              _current_render_mode[max_object_id];
	size_t           _current_n_objects;
	BufferSet        _surround_bufs;
	ChanMapping      _in_map;
	ChanMapping      _out_map;
	std::atomic<int> _flush;
};

} // namespace ARDOUR

#endif /* __ardour_surround_return_h__ */

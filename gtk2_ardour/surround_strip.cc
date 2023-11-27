/*
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

#include "pbd/fastlog.h"

#include "ardour/logmeter.h"
#include "ardour/meter.h"
#include "ardour/profile.h"
#include "ardour/route.h"
#include "ardour/session.h"
#include "ardour/value_as_string.h"

#include "gtkmm2ext/utils.h"

#include "widgets/tooltips.h"

#include "ardour_window.h"
#include "surround_strip.h"

#include "gui_thread.h"
#include "io_selector.h"
#include "keyboard.h"
#include "mixer_ui.h"
#include "ui_config.h"
#include "utils.h"

#include "pbd/i18n.h"

using namespace ARDOUR;
using namespace ArdourWidgets;
using namespace PBD;
using namespace Gtk;

#define PX_SCALE(px) std::max ((float)px, rintf ((float)px* UIConfiguration::instance ().get_ui_scale ()))

PBD::Signal1<void, SurroundStrip*> SurroundStrip::CatchDeletion;

SurroundStrip::SurroundStrip (Mixer_UI& mx, Session* s, std::shared_ptr<Route> r)
	: SessionHandlePtr (s)
	, RouteUI (s)
	, _width (80)
	, _output_button (false)
	, _comment_button (_("Comments"))
	, _level_control (ArdourKnob::default_elements, ArdourKnob::Detent)
{
	init ();
	set_route (r);
}

void
SurroundStrip::init ()
{
	_name_button.set_name ("mixer strip button");
	_name_button.set_text_ellipsize (Pango::ELLIPSIZE_END);
	_name_button.set_layout_ellipsize_width (PX_SCALE (_width) * PANGO_SCALE);

	for (int i = 0; i < 14; ++i) {
		_meter[i] = new FastMeter ((uint32_t)floor (UIConfiguration::instance ().get_meter_hold ()),
		                           8, FastMeter::Horizontal, PX_SCALE (100),
		                           UIConfiguration::instance ().color ("meter color0"),
		                           UIConfiguration::instance ().color ("meter color1"),
		                           UIConfiguration::instance ().color ("meter color2"),
		                           UIConfiguration::instance ().color ("meter color3"),
		                           UIConfiguration::instance ().color ("meter color4"),
		                           UIConfiguration::instance ().color ("meter color5"),
		                           UIConfiguration::instance ().color ("meter color6"),
		                           UIConfiguration::instance ().color ("meter color7"),
		                           UIConfiguration::instance ().color ("meter color8"),
		                           UIConfiguration::instance ().color ("meter color9"),
		                           UIConfiguration::instance ().color ("meter background bottom"),
		                           UIConfiguration::instance ().color ("meter background top"),
		                           0x991122ff, 0x551111ff,
		                           (115.0 * log_meter0dB (-15)),
		                           89.125,
		                           106.375,
		                           115.0,
		                           (UIConfiguration::instance ().get_meter_style_led () ? 3 : 1));

		_meter_box.pack_start (*_meter[i], false, false, i == 12 ? PX_SCALE (3) : 0);
	}

	_level_control.set_size_request (PX_SCALE (50), PX_SCALE (50));
	_level_control.set_tooltip_prefix (_("Level: "));
	_level_control.set_name ("monitor section knob");

	VBox* lcenter_box = manage (new VBox);
	lcenter_box->pack_start (_level_control, true, false);
	_level_box.pack_start (*lcenter_box, true, false);
	_level_box.set_size_request (-1, PX_SCALE (80));
	_level_box.set_name ("AudioBusStripBase");
	lcenter_box->show ();

	_output_button.set_text (_("Output"));
	_output_button.set_name ("mixer strip button");
	_output_button.set_text_ellipsize (Pango::ELLIPSIZE_MIDDLE);
	_output_button.set_layout_ellipsize_width (PX_SCALE (_width) * PANGO_SCALE);

	_comment_button.set_name (X_("mixer strip button"));
	_comment_button.set_text_ellipsize (Pango::ELLIPSIZE_END);
	_comment_button.set_layout_ellipsize_width (PX_SCALE (_width) * PANGO_SCALE);

	_global_vpacker.set_border_width (1);
	_global_vpacker.set_spacing (2);

	Gtk::Label* top_spacer = manage (new Gtk::Label);
	top_spacer->show ();

	_global_vpacker.pack_start (*top_spacer, false, false, PX_SCALE (3));
	_global_vpacker.pack_start (_name_button, Gtk::PACK_SHRINK);
#ifndef MIXBUS
	/* Add a spacer underneath the foldback bus;
	 * this fills the area that is taken up by the scrollbar on the tracks;
	 * and therefore keeps the strip boxes "even" across the bottom
	 */
	int scrollbar_height = 0;
	{
		Gtk::Window window (WINDOW_TOPLEVEL);
		HScrollbar  scrollbar;
		window.add (scrollbar);
		scrollbar.set_name ("MixerWindow");
		scrollbar.ensure_style ();
		Gtk::Requisition requisition (scrollbar.size_request ());
		scrollbar_height = requisition.height;
		scrollbar_height += 3; // track_display_frame border/shadow
	}
	_spacer.set_size_request (-1, scrollbar_height);
	_global_vpacker.pack_end (_spacer, false, false);
	_spacer.show ();
#endif
	_global_vpacker.pack_end (_comment_button, Gtk::PACK_SHRINK);
	_global_vpacker.pack_end (_output_button, Gtk::PACK_SHRINK);
	_global_vpacker.pack_end (_level_box, Gtk::PACK_SHRINK);
	_global_vpacker.pack_end (_meter_box, false, false, PX_SCALE (3));
	_global_vpacker.pack_end (*mute_button, false, false);

	_global_frame.add (_global_vpacker);
	_global_frame.set_shadow_type (Gtk::SHADOW_IN);
	_global_frame.set_name ("MixerStripFrame");
	add (_global_frame);

	_name_button.signal_button_press_event ().connect (sigc::mem_fun (*this, &SurroundStrip::name_button_button_press), false);
	_comment_button.signal_clicked.connect (sigc::mem_fun (*this, &RouteUI::toggle_comment_editor));

	add_events (Gdk::BUTTON_RELEASE_MASK |
	            Gdk::ENTER_NOTIFY_MASK |
	            Gdk::KEY_PRESS_MASK |
	            Gdk::KEY_RELEASE_MASK);

	set_can_focus ();

	//PresentationInfo::Change.connect (*this, invalidator (*this), boost::bind (&SurroundStrip::presentation_info_changed, this, _1), gui_context ());
}

SurroundStrip::~SurroundStrip ()
{
	CatchDeletion (this);
	for (int i = 0; i < 14; ++i) {
		delete _meter[i];
	}
}

void
SurroundStrip::set_route (std::shared_ptr<Route> r)
{
	assert (r);
	RouteUI::set_route (r);

	_output_button.set_route (_route, this);

	_level_control.set_controllable (_route->gain_control ());
	_level_control.show ();

	/* set up metering */
	_route->set_meter_point (MeterPostFader);
	_route->set_meter_type (MeterPeak0dB);

	_route->comment_changed.connect (route_connections, invalidator (*this), boost::bind (&SurroundStrip::setup_comment_button, this), gui_context ());

	/* now force an update of all the various elements */
	name_changed ();
	comment_changed ();
	setup_comment_button ();

	add_events (Gdk::BUTTON_RELEASE_MASK);
	show_all ();
}

void
SurroundStrip::setup_comment_button ()
{
	std::string comment = _route->comment ();

	set_tooltip (_comment_button, comment.empty () ? _("Click to add/edit comments") : _route->comment ());

	if (comment.empty ()) {
		_comment_button.set_name ("generic button");
		_comment_button.set_text (_("Comments"));
		return;
	}

	_comment_button.set_name ("comment button");

	std::string::size_type pos = comment.find_first_of (" \t\n");
	if (pos != std::string::npos) {
		comment = comment.substr (0, pos);
	}
	if (comment.empty ()) {
		_comment_button.set_text (_("Comments"));
	} else {
		_comment_button.set_text (comment);
	}
}

Gtk::Menu*
SurroundStrip::build_route_ops_menu ()
{
	using namespace Menu_Helpers;

	Menu*     menu  = manage (new Menu);
	MenuList& items = menu->items ();
	menu->set_name ("ArdourContextMenu");

	assert (_route->active ());

	items.push_back (MenuElem (_("Color..."), sigc::mem_fun (*this, &RouteUI::choose_color)));
	items.push_back (MenuElem (_("Comments..."), sigc::mem_fun (*this, &RouteUI::open_comment_editor)));

	items.push_back (MenuElem (_("Outputs..."), sigc::mem_fun (*this, &RouteUI::edit_output_configuration)));

	items.push_back (SeparatorElem ());

	items.push_back (MenuElem (_("Rename..."), sigc::mem_fun (*this, &RouteUI::route_rename)));

	items.push_back (SeparatorElem ());

	if (!Profile->get_mixbus ()) {
		items.push_back (CheckMenuElem (_("Protect Against Denormals"), sigc::mem_fun (*this, &RouteUI::toggle_denormal_protection)));
		denormal_menu_item = dynamic_cast<Gtk::CheckMenuItem*> (&items.back ());
		denormal_menu_item->set_active (_route->denormal_protection ());
	}

	return menu;
}

bool
SurroundStrip::name_button_button_press (GdkEventButton* ev)
{
	if (Gtkmm2ext::Keyboard::is_context_menu_event (ev)) {
		Menu* r_menu = build_route_ops_menu ();
		r_menu->popup (ev->button, ev->time);
		return true;
	}
	return false;
}

void
SurroundStrip::fast_update ()
{
	std::shared_ptr<PeakMeter> peak_meter = _route->shared_peak_meter ();
	for (uint32_t i = 0; i < 14; ++i) {
		const float meter_level = peak_meter->meter_level (i, MeterPeak0dB);
		_meter[i]->set (log_meter0dB (meter_level));
	}
}

void
SurroundStrip::route_property_changed (const PropertyChange& what_changed)
{
	if (what_changed.contains (ARDOUR::Properties::name)) {
		name_changed ();
	}
}

void
SurroundStrip::name_changed ()
{
	_name_button.set_text (_route->name ());
	set_tooltip (_name_button, Gtkmm2ext::markup_escape_text (_route->name ()));
}

void
SurroundStrip::set_button_names ()
{
	mute_button->set_text (_("Mute"));
}

void
SurroundStrip::hide_spacer (bool yn)
{
	if (!yn) {
		_spacer.show ();
	} else {
		_spacer.hide ();
	}
}

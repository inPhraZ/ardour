// -*- c++ -*-
// Generated by gmmproc 2.45.3 -- DO NOT MODIFY!
#ifndef _GDKMM_EVENT_H
#define _GDKMM_EVENT_H


#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>

/* Copyright (C) 1998-2002 The gtkmm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

 
#include <gdkmm/window.h>
#include <gdkmm/screen.h>

/* Shadow DELETE macro (from winnt.h).
 */
#if defined(DELETE) && !defined(GTKMM_MACRO_SHADOW_DELETE)
enum { GTKMM_MACRO_DEFINITION_DELETE = DELETE };
#undef DELETE
enum { DELETE = GTKMM_MACRO_DEFINITION_DELETE };
#define DELETE DELETE
#define GTKMM_MACRO_SHADOW_DELETE 1
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
extern "C" { typedef union _GdkEvent GdkEvent; }
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gdk
{

/** @addtogroup gdkmmEnums gdkmm Enums and Flags */

/** 
 *
 * @ingroup gdkmmEnums
 */
enum EventType
{
  NOTHING = -1,
  DELETE,
  DESTROY,
  EXPOSE,
  MOTION_NOTIFY,
  BUTTON_PRESS,
  DOUBLE_BUTTON_PRESS,
  TRIPLE_BUTTON_PRESS,
  BUTTON_RELEASE,
  KEY_PRESS,
  KEY_RELEASE,
  ENTER_NOTIFY,
  LEAVE_NOTIFY,
  FOCUS_CHANGE,
  CONFIGURE,
  MAP,
  UNMAP,
  PROPERTY_NOTIFY,
  SELECTION_CLEAR,
  SELECTION_REQUEST,
  SELECTION_NOTIFY,
  PROXIMITY_IN,
  PROXIMITY_OUT,
  DRAG_ENTER,
  DRAG_LEAVE,
  DRAG_MOTION,
  DRAG_STATUS,
  DROP_START,
  DROP_FINISHED,
  CLIENT_EVENT,
  VISIBILITY_NOTIFY,
  NO_EXPOSE,
  SCROLL,
  WINDOW_STATE,
  SETTING,
  OWNER_CHANGE,
  GRAB_BROKEN,
  DAMAGE,
  EVENT_LAST
};

} // namespace Gdk


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Glib
{

template <>
class Value<Gdk::EventType> : public Glib::Value_Enum<Gdk::EventType>
{
public:
  static GType value_type() G_GNUC_CONST;
};

} // namespace Glib
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gdk
{


/** 
 *
 * @ingroup gdkmmEnums
 */
enum ExtensionMode
{
  EXTENSION_EVENTS_NONE,
  EXTENSION_EVENTS_ALL,
  EXTENSION_EVENTS_CURSOR
};

} // namespace Gdk


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Glib
{

template <>
class Value<Gdk::ExtensionMode> : public Glib::Value_Enum<Gdk::ExtensionMode>
{
public:
  static GType value_type() G_GNUC_CONST;
};

} // namespace Glib
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gdk
{

/** 
 *
 * @ingroup gdkmmEnums
 */
enum AxisUse
{
  AXIS_IGNORE,
  AXIS_X,
  AXIS_Y,
  AXIS_PRESSURE,
  AXIS_XTILT,
  AXIS_YTILT,
  AXIS_WHEEL,
  AXIS_LAST
};

} // namespace Gdk


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Glib
{

template <>
class Value<Gdk::AxisUse> : public Glib::Value_Enum<Gdk::AxisUse>
{
public:
  static GType value_type() G_GNUC_CONST;
};

} // namespace Glib
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gdk
{


//TODO: Actually use this class instead of GdkEvent?
class Event
{
  public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef Event CppObjectType;
  typedef GdkEvent BaseObjectType;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  /** Get the GType for this class, for use with the underlying GObject type system.
   */
  static GType get_type() G_GNUC_CONST;

  Event();

  explicit Event(GdkEvent* gobject, bool make_a_copy = true);

  Event(const Event& other);
  Event& operator=(const Event& other);

  ~Event();

  void swap(Event& other);

  ///Provides access to the underlying C instance.
  GdkEvent*       gobj()       { return gobject_; }

  ///Provides access to the underlying C instance.
  const GdkEvent* gobj() const { return gobject_; }

  ///Provides access to the underlying C instance. The caller is responsible for freeing it. Use when directly setting fields in structs.
  GdkEvent* gobj_copy() const;

protected:
  GdkEvent* gobject_;

private:

  
public:

  
  /** Checks all open displays for a Gdk::Event to process,to be processed
   * on, fetching events from the windowing system if necessary.
   * See Gdk::Display::get_event().
   * 
   * @return The next Gdk::Event to be processed, or <tt>0</tt> if no events
   * are pending. The returned Gdk::Event should be freed with free().
   */
  static Event get();
  
  /** If there is an event waiting in the event queue of some open
   * display, returns a copy of it. See Gdk::Display::peek_event().
   * 
   * @return A copy of the first Gdk::Event on some event queue, or <tt>0</tt> if no
   * events are in any queues. The returned Gdk::Event should be freed with
   * free().
   */
  static Event peek();
  
#ifndef GDKMM_DISABLE_DEPRECATED

  /** Waits for a GraphicsExpose or NoExpose event from the X server.
   * This is used in the Gtk::Text and Gtk::CList widgets in GTK+ to make sure any
   * GraphicsExpose events are handled before the widget is scrolled.
   * 
   * Deprecated: 2.18:
   * 
   * @param window The Gdk::Window to wait for the events for.
   * @return A Gdk::EventExpose if a GraphicsExpose was received, or <tt>0</tt> if a
   * NoExpose event was received.
   */
  static Event get_graphics_expose(const Glib::RefPtr<Window>& window);
#endif // GDKMM_DISABLE_DEPRECATED


  /** Appends a copy of the given event onto the front of the event
   * queue for event->any.window's display, or the default event
   * queue if event->any.window is <tt>0</tt>. See Gdk::Display::put_event().
   */
  void put();

  
  /** Checks if any events are ready to be processed for any display.
   * 
   * @return <tt>true</tt> if any events are pending.
   */
  static bool events_pending();

  
  /** Returns the time stamp from @a event, if there is one; otherwise
   * returns Gdk::CURRENT_TIME. If @a event is <tt>0</tt>, returns Gdk::CURRENT_TIME.
   * 
   * @return Time stamp field from @a event.
   */
  guint32 get_time() const;
  
  /** If the event contains a "state" field, puts that field in @a state. Otherwise
   * stores an empty state (0). Returns <tt>true</tt> if there was a state field
   * in the event. @a event may be <tt>0</tt>, in which case it's treated
   * as if the event had no state field.
   * 
   * @param state Return location for state.
   * @return <tt>true</tt> if there was a state field in the event.
   */
  bool get_state(ModifierType& state) const;
  
  /** Extract the event window relative x/y coordinates from an event.
   * 
   * @param x_win Location to put event window x coordinate.
   * @param y_win Location to put event window y coordinate.
   * @return <tt>true</tt> if the event delivered event window coordinates.
   */
  bool get_coords(double& x_win, double& y_win) const;
  
  /** Extract the root window relative x/y coordinates from an event.
   * 
   * @param x_root Location to put root window x coordinate.
   * @param y_root Location to put root window y coordinate.
   * @return <tt>true</tt> if the event delivered root window coordinates.
   */
  bool get_root_coords(double& x_root, double& y_root) const;
  
  /** Extract the axis value for a particular axis use from
   * an event structure.
   * 
   * @param axis_use The axis use to look for.
   * @param value Location to store the value found.
   * @return <tt>true</tt> if the specified axis was found, otherwise <tt>false</tt>.
   */
  bool get_axis(AxisUse axis_use, double& value) const;
  //_WRAP_METHOD(void	gdk_event_handler_set(GdkEventFunc func, gpointer data, GDestroyNotify  notify), gdk_event_handler_set)

  
  /** Sets whether a trace of received events is output.
   * Note that GTK+ must be compiled with debugging (that is,
   * configured using the <tt>--enable-debug</tt> option)
   * to use this option.
   * 
   * @param show_events <tt>true</tt> to output event debugging information.
   */
  static void set_show_events(bool show_events);
  
  /** Gets whether event debugging output is enabled.
   * 
   * @return <tt>true</tt> if event debugging output is enabled.
   */
  static bool get_show_events();

  
  /** Sets the screen for @a event to @a screen. The event must
   * have been allocated by GTK+, for instance, by
   * copy().
   * 
   * @newin{2,2}
   * 
   * @param screen A Gdk::Screen.
   */
  void set_screen(const Glib::RefPtr<Screen>& screen);
  
  /** Returns the screen for the event. The screen is
   * typically the screen for <tt>event->any.window</tt>, but
   * for events such as mouse events, it is the screen
   * where the pointer was when the event occurs -
   * that is, the screen which has the root window 
   * to which <tt>event->motion.x_root</tt> and
   * <tt>event->motion.y_root</tt> are relative.
   * 
   * @newin{2,2}
   * 
   * @return The screen for the event.
   */
  Glib::RefPtr<Screen> get_screen();
  
  /** Returns the screen for the event. The screen is
   * typically the screen for <tt>event->any.window</tt>, but
   * for events such as mouse events, it is the screen
   * where the pointer was when the event occurs -
   * that is, the screen which has the root window 
   * to which <tt>event->motion.x_root</tt> and
   * <tt>event->motion.y_root</tt> are relative.
   * 
   * @newin{2,2}
   * 
   * @return The screen for the event.
   */
  Glib::RefPtr<const Screen> get_screen() const;

  
  /** Sends an X ClientMessage event to a given window (which must be
   * on the default Gdk::Display.)
   * This could be used for communicating between different applications,
   * though the amount of data is limited to 20 bytes.
   * 
   * @param winid The window to send the X ClientMessage event to.
   * @return Non-zero on success.
   */
  bool send_client_message(NativeWindow winid);
  bool send_client_message(const Glib::RefPtr<Display>& display, NativeWindow winid);
  

};

} // namespace Gdk


namespace Gdk
{

/** @relates Gdk::Event
 * @param lhs The left-hand side
 * @param rhs The right-hand side
 */
inline void swap(Event& lhs, Event& rhs)
  { lhs.swap(rhs); }

} // namespace Gdk

namespace Glib
{

/** A Glib::wrap() method for this object.
 * 
 * @param object The C instance.
 * @param take_copy False if the result should take ownership of the C instance. True if it should take a new copy or ref.
 * @result A C++ instance that wraps this C instance.
 *
 * @relates Gdk::Event
 */
Gdk::Event wrap(GdkEvent* object, bool take_copy = false);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <>
class Value<Gdk::Event> : public Glib::Value_Boxed<Gdk::Event>
{};
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

} // namespace Glib


#endif /* _GDKMM_EVENT_H */


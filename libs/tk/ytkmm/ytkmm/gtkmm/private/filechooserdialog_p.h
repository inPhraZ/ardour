// -*- c++ -*-
// Generated by gmmproc 2.45.3 -- DO NOT MODIFY!
#ifndef _GTKMM_FILECHOOSERDIALOG_P_H
#define _GTKMM_FILECHOOSERDIALOG_P_H


#include <gtkmm/private/dialog_p.h>

#include <glibmm/class.h>

namespace Gtk
{

class FileChooserDialog_Class : public Glib::Class
{
public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef FileChooserDialog CppObjectType;
  typedef GtkFileChooserDialog BaseObjectType;
  typedef GtkFileChooserDialogClass BaseClassType;
  typedef Gtk::Dialog_Class CppClassParent;
  typedef GtkDialogClass BaseClassParent;

  friend class FileChooserDialog;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  const Glib::Class& init();


  static void class_init_function(void* g_class, void* class_data);

  static Glib::ObjectBase* wrap_new(GObject*);

protected:

  //Callbacks (default signal handlers):
  //These will call the *_impl member methods, which will then call the existing default signal callbacks, if any.
  //You could prevent the original default signal handlers being called by overriding the *_impl method.

  //Callbacks (virtual functions):
};


} // namespace Gtk


#endif /* _GTKMM_FILECHOOSERDIALOG_P_H */


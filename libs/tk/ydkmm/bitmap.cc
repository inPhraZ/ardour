// Generated by gmmproc 2.45.3 -- DO NOT MODIFY!


#include <glibmm.h>

#include <gdkmm/bitmap.h>
#include <gdkmm/private/bitmap_p.h>


// -*- c++ -*-
/* $Id: bitmap.ccg,v 1.2 2003/01/22 23:04:18 murrayc Exp $ */

/* Copyright 2002 The gtkmm Development Team
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
#include <gdk/gdk.h>

namespace Gdk
{

Bitmap::Bitmap(GdkBitmap* castitem)
:
  Pixmap((GdkPixmap*) castitem)
{}

Bitmap::Bitmap(const char* data, int width, int height)
:
  Pixmap((GdkPixmap*) gdk_bitmap_create_from_data(0, data, width, height))
{}

Bitmap::Bitmap(const Glib::RefPtr<Drawable>& drawable, const char* data, int width, int height)
:
  Pixmap((GdkPixmap*) gdk_bitmap_create_from_data(Glib::unwrap(drawable), data, width, height))
{}

} // namespace Gdk


namespace
{
} // anonymous namespace


namespace Gdk
{


Glib::RefPtr<Bitmap> Bitmap::create(const char* data, int width, int height)
{
  return Glib::RefPtr<Bitmap>( new Bitmap(data, width, height) );
}

Glib::RefPtr<Bitmap> Bitmap::create(const Glib::RefPtr<Gdk::Drawable>& drawable, const char* data, int width, int height)
{
  return Glib::RefPtr<Bitmap>( new Bitmap(drawable, data, width, height) );
}


} // namespace Gdk



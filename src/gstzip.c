/* GStreamer zip plugin
 * Copyright (C) 2017 Nikolay Beloborodov <sonntex@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstzipgzdec.h"
#include "gstzipbz2dec.h"

static gboolean
plugin_init (GstPlugin *plugin)
{
  if (!gst_element_register (plugin, "zipgzdec", GST_RANK_NONE, GST_TYPE_ZIP_GZ_DEC))
    return FALSE;
  if (!gst_element_register (plugin, "zipbz2dec", GST_RANK_NONE, GST_TYPE_ZIP_BZ2_DEC))
    return FALSE;

  return TRUE;
}

#ifdef HAVE_GST1
#  define GST_PLUGIN_NAME  zip
#else
#  define GST_PLUGIN_NAME "zip"
#endif

GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    GST_PLUGIN_NAME,
    "zip plugins",
    plugin_init,
    VERSION,
    "LGPL",
    "GStreamer",
    "https://github.com/sonntex/gst-zip"
)

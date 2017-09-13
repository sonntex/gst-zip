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

#ifndef __GST_ZIP_BASE_DEC_H__
#define __GST_ZIP_BASE_DEC_H__

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_ZIP_BASE_DEC \
  (gst_zip_base_dec_get_type())
#define GST_ZIP_BASE_DEC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_ZIP_BASE_DEC,GstZipBaseDec))
#define GST_ZIP_BASE_DEC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_ZIP_BASE_DEC,GstZipBaseDecClass))
#define GST_ZIP_BASE_DEC_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_ZIP_BASE_DEC, GstZipBaseDecClass))
#define GST_IS_ZIP_BASE_DEC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_ZIP_BASE_DEC))
#define GST_IS_ZIP_BASE_DEC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_ZIP_BASE_DEC))

typedef struct _GstZipBaseDec      GstZipBaseDec;
typedef struct _GstZipBaseDecClass GstZipBaseDecClass;

struct _GstZipBaseDec
{
  GstElement element;
  GstPad *sinkpad, *srcpad;
  guint buffer_capacity;
  GstBuffer *buf;
#ifdef HAVE_GST1
  GstMapInfo buf_map;
#endif
  gsize buf_size;
  gpointer buf_data;
  goffset buf_offset;
};

struct _GstZipBaseDecClass 
{
  GstElementClass parent_class;

  gboolean (*zip_init) (GstZipBaseDec *basedec);
  void (*zip_finalize) (GstZipBaseDec *basedec);
  void (*zip_next_in) (GstZipBaseDec *basedec, gpointer data, gsize size);
  gsize (*zip_avail_in) (GstZipBaseDec *basedec);
  void (*zip_next_out) (GstZipBaseDec *basedec, gpointer data, gsize size);
  gsize (*zip_avail_out) (GstZipBaseDec *basedec);
  gboolean (*zip_decompress) (GstZipBaseDec *basedec);
};

GType gst_zip_base_dec_get_type (void);

G_END_DECLS

#endif /* __GST_ZIP_BASE_DEC_H__ */

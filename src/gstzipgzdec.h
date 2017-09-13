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

#ifndef __GST_ZIP_GZ_DEC_H__
#define __GST_ZIP_GZ_DEC_H__

#include <gstzipbasedec.h>
#include <zlib.h>

G_BEGIN_DECLS

#define GST_TYPE_ZIP_GZ_DEC \
  (gst_zip_gz_dec_get_type())
#define GST_ZIP_GZ_DEC_CAST(obj) \
  ((GstZipGzDec *)(obj))
#define GST_ZIP_GZ_DEC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_ZIP_GZ_DEC,GstZipGzDec))
#define GST_ZIP_GZ_DEC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_ZIP_GZ_DEC,GstZipGzDecClass))
#define GST_IS_ZIP_GZ_DEC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_ZIP_GZ_DEC))
#define GST_IS_ZIP_GZ_DEC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_ZIP_GZ_DEC))

typedef struct _GstZipGzDec      GstZipGzDec;
typedef struct _GstZipGzDecClass GstZipGzDecClass;

struct _GstZipGzDec
{
  GstZipBaseDec parent_instance;
  z_stream strm;
};

struct _GstZipGzDecClass
{
  GstZipBaseDecClass parent_class;
};

GType gst_zip_gz_dec_get_type (void);

G_END_DECLS

#endif /* __GST_ZIP_GZ_DEC_H__ */

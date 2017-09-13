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

/**
 * SECTION:element-zipgzdec
 * @title: zipgzdec
 * @see_also: zipgzdec
 *
 * zipgzdec decodes gzip compressed data.
 *
 * ## Example launch line
 * |[
 * gst-launch-0.10 filesrc location=data.gz ! zipgzdec ! filesink location=data
 * ]| Decode gzip compressed file data.gz to data.
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string.h>
#include <gstzipgzdec.h>

#ifdef HAVE_GST1
#define gst_zip_gz_dec_parent_class parent_class
G_DEFINE_TYPE (GstZipGzDec, gst_zip_gz_dec,
    GST_TYPE_ZIP_BASE_DEC);
#else
GST_BOILERPLATE (GstZipGzDec, gst_zip_gz_dec, GstZipBaseDec,
    GST_TYPE_ZIP_BASE_DEC);
#endif

static gboolean z_init (GstZipBaseDec *basedec);
static void z_finalize (GstZipBaseDec *basedec);
static void z_next_in (GstZipBaseDec *basedec, gpointer data, gsize size);
static gsize z_avail_in (GstZipBaseDec *basedec);
static void z_next_out (GstZipBaseDec *basedec, gpointer data, gsize size);
static gsize z_avail_out (GstZipBaseDec *basedec);
static gboolean z_decompress (GstZipBaseDec *basedec);

static void
gst_zip_gz_dec_class_init (GstZipGzDecClass *klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);
  GstZipBaseDecClass *basedec_class = GST_ZIP_BASE_DEC_CLASS (klass);

#ifndef HAVE_GST1
  parent_class = g_type_class_peek_parent (klass);
#endif

  basedec_class->zip_init = z_init;
  basedec_class->zip_finalize = z_finalize;
  basedec_class->zip_next_in = z_next_in;
  basedec_class->zip_avail_in = z_avail_in;
  basedec_class->zip_next_out = z_next_out;
  basedec_class->zip_avail_out = z_avail_out;
  basedec_class->zip_decompress = z_decompress;

  gst_element_class_set_details_simple(element_class,
      "gzip decoder",
      "decoder",
      "decode gzip compressed data",
      "Nikolay Beloborodov <sonntex@gmail.com>");
}

#ifndef HAVE_GST1
static void
gst_zip_gz_dec_init (GstZipGzDec *dec, GstZipGzDecClass *klass)
#else
static void
gst_zip_gz_dec_init (GstZipGzDec *dec)
#endif
{
}

#ifndef HAVE_GST1
static void
gst_zip_gz_dec_base_init (gpointer klass)
{
}
#endif

static gboolean
z_init (GstZipBaseDec *basedec)
{
  GstZipGzDec *dec = GST_ZIP_GZ_DEC_CAST (basedec);
  int err;

  memset (&dec->strm, 0, sizeof (dec->strm));

  err = inflateInit2 (&dec->strm, 16);
  if (err != Z_OK) {
    GST_ERROR_OBJECT (dec, "zlib error %d", err);
    return FALSE;
  }

  return TRUE;
}

static void
z_finalize (GstZipBaseDec *basedec)
{
  GstZipGzDec *dec = GST_ZIP_GZ_DEC_CAST (basedec);

  inflateEnd (&dec->strm);
}

static void
z_next_in (GstZipBaseDec *basedec, gpointer data, gsize size)
{
  GstZipGzDec *dec = GST_ZIP_GZ_DEC_CAST (basedec);

  dec->strm.next_in = data;
  dec->strm.avail_in = size;
}

static gsize
z_avail_in (GstZipBaseDec *basedec)
{
  GstZipGzDec *dec = GST_ZIP_GZ_DEC_CAST (basedec);

  return (gsize) dec->strm.avail_in;
}

static void
z_next_out (GstZipBaseDec *basedec, gpointer data, gsize size)
{
  GstZipGzDec *dec = GST_ZIP_GZ_DEC_CAST (basedec);

  dec->strm.next_out = data;
  dec->strm.avail_out = size;
}

static gsize
z_avail_out (GstZipBaseDec *basedec)
{
  GstZipGzDec *dec = GST_ZIP_GZ_DEC_CAST (basedec);

  return (gsize) dec->strm.avail_out;
}

static gboolean
z_decompress (GstZipBaseDec *basedec)
{
  GstZipGzDec *dec = GST_ZIP_GZ_DEC_CAST (basedec);
  int err;

  err = inflate (&dec->strm, Z_NO_FLUSH);
  if (err != Z_OK &&
      err != Z_STREAM_END) {
    GST_ERROR_OBJECT (dec, "zlib error %d", err);
    return FALSE;
  }

  return TRUE;
}

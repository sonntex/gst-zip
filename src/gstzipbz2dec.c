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
 * SECTION:element-zipbz2dec
 * @title: zipbz2dec
 * @see_also: zipbz2dec
 *
 * zipbz2dec decodes bzip2 compressed data.
 *
 * ## Example launch line
 * |[
 * gst-launch-0.10 filesrc location=data.bz2 ! zipbz2dec ! filesink location=data
 * ]| Decode bzip2 compressed file data.bz2 to data.
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string.h>
#include <gstzipbz2dec.h>

#ifdef HAVE_GST1
#define gst_zip_bz2_dec_parent_class parent_class
G_DEFINE_TYPE (GstZipBz2Dec, gst_zip_bz2_dec,
    GST_TYPE_ZIP_BASE_DEC);
#else
GST_BOILERPLATE (GstZipBz2Dec, gst_zip_bz2_dec, GstZipBaseDec,
    GST_TYPE_ZIP_BASE_DEC);
#endif

static gboolean bz2_init (GstZipBaseDec *basedec);
static void bz2_finalize (GstZipBaseDec *basedec);
static void bz2_next_in (GstZipBaseDec *basedec, gpointer data, gsize size);
static gsize bz2_avail_in (GstZipBaseDec *basedec);
static void bz2_next_out (GstZipBaseDec *basedec, gpointer data, gsize size);
static gsize bz2_avail_out (GstZipBaseDec *basedec);
static gboolean bz2_decompress (GstZipBaseDec *basedec);

static void
gst_zip_bz2_dec_class_init (GstZipBz2DecClass *klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);
  GstZipBaseDecClass *basedec_class = GST_ZIP_BASE_DEC_CLASS (klass);

#ifndef HAVE_GST1
  parent_class = g_type_class_peek_parent (klass);
#endif

  basedec_class->zip_init = bz2_init;
  basedec_class->zip_finalize = bz2_finalize;
  basedec_class->zip_next_in = bz2_next_in;
  basedec_class->zip_avail_in = bz2_avail_in;
  basedec_class->zip_next_out = bz2_next_out;
  basedec_class->zip_avail_out = bz2_avail_out;
  basedec_class->zip_decompress = bz2_decompress;

  gst_element_class_set_details_simple(element_class,
      "bzip2 decoder",
      "decoder",
      "decode bzip2 compressed data",
      "Nikolay Beloborodov <sonntex@gmail.com>");
}

#ifndef HAVE_GST1
static void
gst_zip_bz2_dec_init (GstZipBz2Dec *dec, GstZipBz2DecClass *klass)
#else
static void
gst_zip_bz2_dec_init (GstZipBz2Dec *dec)
#endif
{
}

#ifndef HAVE_GST1
static void
gst_zip_bz2_dec_base_init (gpointer klass)
{
}
#endif

static gboolean
bz2_init (GstZipBaseDec *basedec)
{
  GstZipBz2Dec *dec = GST_ZIP_BZ2_DEC_CAST (basedec);
  int err;

  memset (&dec->strm, 0, sizeof (dec->strm));

  err = BZ2_bzDecompressInit (&dec->strm, 0, 0);
  if (err != BZ_OK) {
    GST_ERROR_OBJECT (dec, "bzlib error %d", err);
    return FALSE;
  }

  return TRUE;
}

static void
bz2_finalize (GstZipBaseDec *basedec)
{
  GstZipBz2Dec *dec = GST_ZIP_BZ2_DEC_CAST (basedec);

  BZ2_bzDecompressEnd (&dec->strm);
}

static void
bz2_next_in (GstZipBaseDec *basedec, gpointer data, gsize size)
{
  GstZipBz2Dec *dec = GST_ZIP_BZ2_DEC_CAST (basedec);

  dec->strm.next_in = data;
  dec->strm.avail_in = size;
}

static gsize
bz2_avail_in (GstZipBaseDec *basedec)
{
  GstZipBz2Dec *dec = GST_ZIP_BZ2_DEC_CAST (basedec);

  return (gsize) dec->strm.avail_in;
}

static void
bz2_next_out (GstZipBaseDec *basedec, gpointer data, gsize size)
{
  GstZipBz2Dec *dec = GST_ZIP_BZ2_DEC_CAST (basedec);

  dec->strm.next_out = data;
  dec->strm.avail_out = size;
}

static gsize
bz2_avail_out (GstZipBaseDec *basedec)
{
  GstZipBz2Dec *dec = GST_ZIP_BZ2_DEC_CAST (basedec);

  return (gsize) dec->strm.avail_out;
}

static gboolean
bz2_decompress (GstZipBaseDec *basedec)
{
  GstZipBz2Dec *dec = GST_ZIP_BZ2_DEC_CAST (basedec);
  int err;

  err = BZ2_bzDecompress (&dec->strm);
  if (err != BZ_OK &&
      err != BZ_STREAM_END) {
    GST_ERROR_OBJECT (dec, "bzlib2 error %d", err);
    return FALSE;
  }

  return TRUE;
}

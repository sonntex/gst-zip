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
#  include <config.h>
#endif

#include <string.h>
#include <gstzipbasedec.h>

GST_DEBUG_CATEGORY_STATIC (gst_zip_base_dec_debug);
#define GST_CAT_DEFAULT gst_zip_base_dec_debug

enum
{
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_BUFFER_CAPACITY
};

#define GST_ZIP_BASE_DEC_BUFFER_CAPACITY_DEFAULT 4096

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY"));

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY"));

#ifdef HAVE_GST1
#define gst_zip_base_dec_parent_class parent_class
G_DEFINE_TYPE (GstZipBaseDec, gst_zip_base_dec, GST_TYPE_ELEMENT);
#else
GST_BOILERPLATE (GstZipBaseDec, gst_zip_base_dec, GstElement, GST_TYPE_ELEMENT);
#endif

static void gst_zip_base_dec_set_property (GObject *object, guint prop_id,
    const GValue *value, GParamSpec *pspec);
static void gst_zip_base_dec_get_property (GObject *object, guint prop_id,
    GValue *value, GParamSpec *pspec);

#ifdef HAVE_GST1
static GstFlowReturn gst_zip_base_dec_chain (GstPad *pad, GstObject *parent, GstBuffer *buf);
static gboolean gst_zip_base_dec_event (GstPad *pad, GstObject *parent, GstEvent *event);
#else
static GstFlowReturn gst_zip_base_dec_chain (GstPad *pad, GstBuffer *buf);
static gboolean gst_zip_base_dec_event (GstPad *pad, GstEvent *event);
#endif

static GstStateChangeReturn gst_zip_base_dec_change_state (GstElement *element, GstStateChange transition);

static gboolean gst_zip_base_dec_start (GstZipBaseDec *dec);
static void gst_zip_base_dec_stop (GstZipBaseDec *dec);
static void gst_zip_base_dec_push (GstZipBaseDec *dec);
static void gst_zip_base_dec_reset (GstZipBaseDec *dec);
static void gst_zip_base_dec_drop (GstZipBaseDec *dec);
static void gst_zip_base_dec_shrink (GstZipBaseDec *dec);

void
gst_zip_base_dec_class_init (GstZipBaseDecClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  GST_DEBUG_CATEGORY_INIT (gst_zip_base_dec_debug, "zipbasedec", 0, "zip decoding");

#ifndef HAVE_GST1
  parent_class = g_type_class_peek_parent (klass);
#endif

  object_class->set_property = gst_zip_base_dec_set_property;
  object_class->get_property = gst_zip_base_dec_get_property;

  g_object_class_install_property (object_class, PROP_BUFFER_CAPACITY,
      g_param_spec_uint ("buffer-capacity", "buffer-capacity", "buffer capacity",
          0, G_MAXUINT, GST_ZIP_BASE_DEC_BUFFER_CAPACITY_DEFAULT, G_PARAM_READWRITE));

  element_class->change_state = GST_DEBUG_FUNCPTR (gst_zip_base_dec_change_state);

  klass->zip_init = NULL;
  klass->zip_finalize = NULL;
  klass->zip_next_in = NULL;
  klass->zip_avail_in = NULL;
  klass->zip_next_out = NULL;
  klass->zip_avail_out = NULL;
  klass->zip_decompress = NULL;
}

#ifndef HAVE_GST1
static void
gst_zip_base_dec_init (GstZipBaseDec *dec, GstZipBaseDecClass *klass)
#else
static void
gst_zip_base_dec_init (GstZipBaseDec *dec)
#endif
{
  dec->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_element_add_pad (GST_ELEMENT (dec), dec->sinkpad);
  dec->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  gst_element_add_pad (GST_ELEMENT (dec), dec->srcpad);

  gst_pad_set_chain_function (dec->sinkpad, GST_DEBUG_FUNCPTR (gst_zip_base_dec_chain));
  gst_pad_set_event_function (dec->sinkpad, GST_DEBUG_FUNCPTR (gst_zip_base_dec_event));

  dec->buffer_capacity = GST_ZIP_BASE_DEC_BUFFER_CAPACITY_DEFAULT;
}

#ifndef HAVE_GST1
static void
gst_zip_base_dec_base_init (gpointer klass)
{
}
#endif

static void
gst_zip_base_dec_set_property (GObject *object, guint prop_id,
    const GValue *value, GParamSpec *pspec)
{
  GstZipBaseDec *dec = GST_ZIP_BASE_DEC (object);

  switch (prop_id) {
  case PROP_BUFFER_CAPACITY:
    dec->buffer_capacity = g_value_get_uint (value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
gst_zip_base_dec_get_property (GObject *object, guint prop_id,
    GValue *value, GParamSpec *pspec)
{
  GstZipBaseDec *dec = GST_ZIP_BASE_DEC (object);

  switch (prop_id) {
  case PROP_BUFFER_CAPACITY:
    g_value_set_uint (value, dec->buffer_capacity);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

#ifdef HAVE_GST1
static GstFlowReturn
gst_zip_base_dec_chain (GstPad *pad, GstObject *parent, GstBuffer *buf)
#else
static GstFlowReturn
gst_zip_base_dec_chain (GstPad *pad, GstBuffer *buf)
#endif
{
  GstZipBaseDec *dec = GST_ZIP_BASE_DEC (GST_OBJECT_PARENT (pad));
  GstZipBaseDecClass *dec_class = GST_ZIP_BASE_DEC_GET_CLASS (dec);
  
#ifdef HAVE_GST1
  GstMapInfo buf_map;
#endif
  gsize buf_size;
  gpointer buf_data;

  GST_TRACE_OBJECT (dec, "chain");

#ifdef HAVE_GST1
  gst_buffer_map (buf, &buf_map, GST_MAP_READ);
  buf_data = buf_map.data;
  buf_size = buf_map.size;
#else
  buf_size = GST_BUFFER_SIZE (buf);
  buf_data = GST_BUFFER_DATA (buf);
#endif

  dec_class->zip_next_in (dec, buf_data, buf_size);

  while (dec_class->zip_avail_in (dec))
  {
    dec_class->zip_next_out (dec, dec->buf_data + dec->buf_offset, dec->buf_size - dec->buf_offset);

    GST_TRACE_OBJECT (dec, "avail in %" G_GSIZE_FORMAT, dec_class->zip_avail_in (dec));
    GST_TRACE_OBJECT (dec, "avail out %" G_GSIZE_FORMAT, dec_class->zip_avail_out (dec));

    GST_TRACE_OBJECT (dec, "decompress");

    if (!dec_class->zip_decompress (dec)) {
      gst_zip_base_dec_drop (dec);
      goto done;
    }

    GST_TRACE_OBJECT (dec, "avail in %" G_GSIZE_FORMAT, dec_class->zip_avail_in (dec));
    GST_TRACE_OBJECT (dec, "avail out %" G_GSIZE_FORMAT, dec_class->zip_avail_out (dec));

    dec->buf_offset = dec->buf_size - dec_class->zip_avail_out (dec);

    if (dec->buf_offset == dec->buf_size)
      gst_zip_base_dec_push (dec);
  }

done:

#ifdef HAVE_GST1
  gst_buffer_unmap (buf, &buf_map);
#endif

  gst_buffer_unref (buf);

  return GST_FLOW_OK;
}

#ifdef HAVE_GST1
static gboolean
gst_zip_base_dec_event (GstPad *pad, GstObject *parent, GstEvent *event)
#else
static gboolean
gst_zip_base_dec_event (GstPad *pad, GstEvent *event)
#endif
{
  GstZipBaseDec *dec = GST_ZIP_BASE_DEC (GST_OBJECT_PARENT (pad));

  switch (GST_EVENT_TYPE (event)) {
  case GST_EVENT_EOS:
    gst_zip_base_dec_push (dec);
    break;
  default:
    break;
  }

#ifdef HAVE_GST1
  return gst_pad_event_default (pad, parent, event);
#else
  return gst_pad_event_default (pad, event);
#endif
}

static GstStateChangeReturn
gst_zip_base_dec_change_state (GstElement *element, GstStateChange transition)
{
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
  GstZipBaseDec *dec = GST_ZIP_BASE_DEC (element);

  switch (transition) {
  case GST_STATE_CHANGE_NULL_TO_READY:
    if (!gst_zip_base_dec_start (dec))
      return GST_STATE_CHANGE_FAILURE;
    break;
  default:
    break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
  if (ret == GST_STATE_CHANGE_FAILURE)
    return ret;

  switch (transition) {
  case GST_STATE_CHANGE_READY_TO_NULL:
    gst_zip_base_dec_stop (dec);
    break;
  default:
    break;
  }

  return ret;
}

static gboolean
gst_zip_base_dec_start (GstZipBaseDec *dec)
{
  GstZipBaseDecClass *dec_class = GST_ZIP_BASE_DEC_GET_CLASS (dec);

  GST_TRACE_OBJECT (dec, "start");

  dec_class->zip_init (dec);

  gst_zip_base_dec_reset (dec);

  return TRUE;
}

static void
gst_zip_base_dec_stop (GstZipBaseDec *dec)
{
  GstZipBaseDecClass *dec_class = GST_ZIP_BASE_DEC_GET_CLASS (dec);

  GST_TRACE_OBJECT (dec, "stop");

  dec_class->zip_finalize (dec);

  gst_zip_base_dec_drop (dec);
}

static void
gst_zip_base_dec_push (GstZipBaseDec *dec)
{
  GST_TRACE_OBJECT (dec, "push");

  if (dec->buf_offset > 0 &&
      dec->buf_offset < dec->buf_size)
    gst_zip_base_dec_shrink (dec);

#ifdef HAVE_GST1
  gst_buffer_unmap (dec->buf, &dec->buf_map);
#endif

  gst_pad_push (dec->srcpad, dec->buf);

  gst_zip_base_dec_reset (dec);
}

static void
gst_zip_base_dec_reset (GstZipBaseDec *dec)
{
  GST_TRACE_OBJECT (dec, "reset");

  dec->buf = gst_buffer_new_and_alloc (dec->buffer_capacity);
#ifdef HAVE_GST1
  gst_buffer_map (dec->buf, &dec->buf_map, GST_MAP_WRITE);
  dec->buf_size = dec->buf_map.size;
  dec->buf_data = dec->buf_map.data;
#else
  dec->buf_size = GST_BUFFER_SIZE (dec->buf);
  dec->buf_data = GST_BUFFER_DATA (dec->buf);
#endif
  dec->buf_offset = 0;  
}

static void
gst_zip_base_dec_drop (GstZipBaseDec *dec)
{
  if (!dec->buf)
    return;

  GST_TRACE_OBJECT (dec, "drop");

#ifdef HAVE_GST1
  gst_buffer_unmap (dec->buf, &dec->buf_map);
#endif

  gst_buffer_unref (dec->buf);

  dec->buf = NULL;
  dec->buf_size = 0;
  dec->buf_data = NULL;
  dec->buf_offset = 0;
}

static void
gst_zip_base_dec_shrink (GstZipBaseDec *dec)
{
  GstBuffer *buf;
#ifdef HAVE_GST1
  GstMapInfo buf_map;
#endif
  gsize buf_size;
  gpointer buf_data;

  GST_TRACE_OBJECT (dec, "shrink");

  buf = gst_buffer_new_and_alloc (dec->buf_offset);

#ifdef HAVE_GST1
  gst_buffer_map (buf, &buf_map, GST_MAP_WRITE);
  buf_size = buf_map.size;
  buf_data = buf_map.data;
#else
  buf_size = GST_BUFFER_SIZE (buf);
  buf_data = GST_BUFFER_DATA (buf);
#endif

  memmove (buf_data, dec->buf_data, buf_size);

#ifdef HAVE_GST1
  gst_buffer_unmap (dec->buf, &dec->buf_map);
#endif

  gst_buffer_unref (dec->buf);

  dec->buf = buf;
#ifdef HAVE_GST1
  dec->buf_map = buf_map;
#endif
  dec->buf_size = buf_size;
  dec->buf_data = buf_data;
  dec->buf_offset = buf_size;
}

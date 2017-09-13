plugin_LTLIBRARIES = libgstzip.la

libgstzip_la_SOURCES = \
  gstzip.c \
  gstzipbasedec.c \
  gstzipbasedec.h \
  gstzipgzdec.c \
  gstzipgzdec.h \
  gstzipbz2dec.c \
  gstzipbz2dec.h

libgstzip_la_CFLAGS = $(GST_CFLAGS)
libgstzip_la_LIBADD = $(GST_LIBS) -lz -lbz2
libgstzip_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstzip_la_LIBTOOLFLAGS = --tag=disable-static

noinst_HEADERS = \
  gstzipbasedec.h \
  gstzipgzdec.h \
  gstzipbz2dec.h

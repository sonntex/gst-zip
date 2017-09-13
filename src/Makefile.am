plugin_LTLIBRARIES = libgstzip.la

libgstzip_la_SOURCES = \
  gstzip.c \
  gstzipbasedec.c \
  gstzipbasedec.h \
  gstzipgzdec.c \
  gstzipgzdec.h \
  gstzipbz2dec.c \
  gstzipbz2dec.h

libgstzip_la_CFLAGS = $(GST_CFLAGS) $(ZLIB_CFLAGS) $(BZLIB2_CFLAGS)
libgstzip_la_LIBADD = $(GST_LIBS) $(ZLIB_LIBS) $(BZLIB2_LIBS)
libgstzip_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstzip_la_LIBTOOLFLAGS = --tag=disable-static

noinst_HEADERS = \
  gstzipbasedec.h \
  gstzipgzdec.h \
  gstzipbz2dec.h

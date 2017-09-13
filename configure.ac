AC_PREREQ([2.53])

AC_INIT([gst-zip],[0.10.36])

AC_CONFIG_SRCDIR([src/gstzip.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.10])

AM_MAINTAINER_MODE([enable])

AC_PROG_CC

LT_PREREQ([2.2.6])
LT_INIT

AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, [ ], [
  AC_MSG_ERROR([You need to have pkg-config installed!])
])

AC_ARG_ENABLE(gst1,
  AS_HELP_STRING([--enable-gst1=@<:@no/yes@:>@], [Build with gstreamer 1.0.]),,
  enable_gst1=yes)

if test "x$enable_gst1" != "xno"; then
  GST_MAJORMINOR=1.0
  AC_DEFINE(HAVE_GST1, 1, [Build with gstreamer 1.0.])
else
  GST_MAJORMINOR=0.10
fi

PKG_CHECK_MODULES(GST, [gstreamer-$GST_MAJORMINOR])

AC_MSG_CHECKING([to see if compiler understands -Wall])
save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([ ], [ ])], [
  GST_CFLAGS="$GST_CFLAGS -Wall"
  AC_MSG_RESULT([yes])
], [
  AC_MSG_RESULT([no])
])

if test "x${prefix}" = "x$HOME"; then
  plugindir="$HOME/.gstreamer-$GST_MAJORMINOR/plugins"
else
  plugindir="\$(libdir)/gstreamer-$GST_MAJORMINOR"
fi
AC_SUBST(plugindir)

GST_PLUGIN_LDFLAGS='-module -avoid-version -export-symbols-regex [_]*\(gst_\|Gst\|GST_\).*'
AC_SUBST(GST_PLUGIN_LDFLAGS)

AC_SUBST(GST_MAJORMINOR)
AC_SUBST(GST_CFLAGS)
AC_SUBST(GST_LIBS)

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

#!/bin/sh
# you can either set the environment variables AUTOCONF, AUTOHEADER, AUTOMAKE,
# ACLOCAL, AUTOPOINT and/or LIBTOOLIZE to the right versions, or leave them
# unset and get the defaults

autoreconf --verbose --force --install || {
  echo 'autogen.sh failed';
  exit 1;
}

echo
echo "Now type './configure' to configure and 'make' to compile this module."
echo

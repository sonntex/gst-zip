gst-zip
=======

## Dependencies
To install the plugin, you'll need to satisfy the following dependencies:

* [pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/)
* [zlib](http://zlib.net/)
* [bzip2](http://www.bzip.org/)
* [gstreamer](https://gstreamer.freedesktop.org/)

All of those libraries are usually available on the most common
distributions. Installing these libraries on a recent Arch Linux is
very simple:

    pacman -S core/pkg-config core/zlib core/bzip2 extra/gstreamer

## Compile
Once you have installed all the dependencies, get the code, configure,
compile and install it:

    git clone https://github.com/sonntex/gst-zip.git
    cd gst-zip
    ./autogen.sh
    ./configure
    make
    make install

If you're interested in gstreamer 0.10 support, you should disable
gstreamer 1.0:

    ./configure --disable-gst1

## Test
Use scripts from tests directory to test the plugin and find memory
leaks:

    cd tests
    . ./vars-memory-leaks.sh
    ./test.sh -o
    ./test.sh -n

## Run

Use zipgzdec to decompress gzip compressed data:

    gst-launch-1.0 filesrc location=data.gz \
        ! zipgzdec \
        ! filesink location=data

Use zipbz2dec to decompress bzip2 compressed data:

    gst-launch-1.0 filesrc location=data.bz2 \
        ! zipbz2dec \
        ! filesink location=data

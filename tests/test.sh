#!/bin/sh

gst_ver="1.0"

usage() {
    echo "Usage: $(basename $0) [OPTIONS]"
    echo "  -h help"
    echo "  -o use gstreamer-0.10"
    echo "  -n use gstreamer-1.0"
    exit 0
}

OPTIND=1

while getopts "hon" opt
do
    case $opt in
        o)  gst_ver="0.10"
            ;;
        n)  gst_ver="1.0"
            ;;
        *)  usage
            ;;
    esac
done

shift $((OPTIND-1))

[ $# -ne 0 ] && usage

echo "gstremer version is $gst_ver"

echo "downloading test file..."
wget -O sample.webm "http://video.webmfiles.org/big-buck-bunny_trailer.webm"

gzip -c sample.webm >sample.webm.gz

for buffer_capacity in 2048 4096 6144; do
    echo -n "zipgzdec ($buffer_capacity): "
    gst-launch-$gst_ver filesrc location=sample.webm.gz ! zipgzdec buffer-capacity=$buffer_capacity ! filesink location=sample-copy.webm &>/dev/null
    diff sample.webm sample-copy.webm &>/dev/null
    if [ $? -eq 0 ]; then
        echo "yes"
    else
        echo "no"
    fi
done

bzip2 -c sample.webm >sample.webm.bz2

for buffer_capacity in 2048 4096 6144; do
    echo -n "zipbz2dec ($buffer_capacity): "
    gst-launch-$gst_ver filesrc location=sample.webm.bz2 ! zipbz2dec buffer-capacity=$buffer_capacity ! filesink location=sample-copy.webm &>/dev/null
    diff sample.webm sample-copy.webm &>/dev/null
    if [ $? -eq 0 ]; then
        echo "yes"
    else
        echo "no"
    fi
done

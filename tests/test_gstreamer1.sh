#!/bin/sh

echo 'downloading test file...'
wget -O sample.webm 'http://video.webmfiles.org/big-buck-bunny_trailer.webm'

echo -n 'zipgzdec: '
gzip -c sample.webm >sample.webm.gz
gst-launch-1.0 filesrc location=sample.webm.gz ! zipgzdec ! filesink location=sample-copy.webm &>/dev/null
diff sample.webm sample-copy.webm &>/dev/null
if [ $? -eq 0 ]; then echo 'yes'; else echo 'no'; fi

echo -n 'zipbz2dec: '
bzip2 -c sample.webm >sample.webm.bz2
gst-launch-1.0 filesrc location=sample.webm.bz2 ! zipbz2dec ! filesink location=sample-copy.webm &>/dev/null
diff sample.webm sample-copy.webm &>/dev/null
if [ $? -eq 0 ]; then echo 'yes'; else echo 'no'; fi

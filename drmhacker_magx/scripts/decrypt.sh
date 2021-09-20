#!/bin/sh
find /usr/data_resource/picture -type f -name "*.dcf" | xargs -n1 sh -c '../drmhacker_magx $0 `basename $0 .dcf`'
find /usr/data_resource/sound/ringtones -type f -name "*.dcf" | xargs -n1 sh -c '../drmhacker_magx $0 `basename $0 .dcf`'

#!/bin/sh

rm -Rf uncrypted.tgz

rm -Rf uncrypted
mkdir -p uncrypted/picture
mkdir -p uncrypted/ringtones
mkdir -p uncrypted/videos

cd uncrypted

cd picture
find /usr/data_resource/picture -type f -name "*.dcf*" | while read FILE; do F="${FILE%.*}"; /usr/bin/drmhacker_magx drm_sp "$FILE" "${F##*/}"; done
find /usr/data_resource/picture -type f -name "*.drm*" | while read FILE; do F1="${FILE%.drm*}"; F2="${FILE##*.drm}"; /usr/bin/drmhacker_magx drm_sp "$FILE" "${F1##*/}$F2"; done
cd ..

cd ringtones
find /usr/data_resource/sound/ringtones -type f -name "*.dcf*" | while read FILE; do F="${FILE%.*}"; /usr/bin/drmhacker_magx drm_sp "$FILE" "${F##*/}"; done
find /usr/data_resource/sound/ringtones -type f -name "*.drm*" | while read FILE; do F1="${FILE%.drm*}"; F2="${FILE##*.drm}"; /usr/bin/drmhacker_magx drm_sp "$FILE" "${F1##*/}$F2"; done
cd ..

cd videos
find /usr/data_resource/videos -type f -name "*.dcf*" | while read FILE; do F="${FILE%.*}"; /usr/bin/drmhacker_magx drm_sp "$FILE" "${F##*/}"; done
find /usr/data_resource/videos -type f -name "*.drm*" | while read FILE; do F1="${FILE%.drm*}"; F2="${FILE##*.drm}"; /usr/bin/drmhacker_magx drm_sp "$FILE" "${F1##*/}$F2"; done
cd ..

cd ..
busybox tar -cvzf uncrypted.tgz uncrypted
echo "Done!"

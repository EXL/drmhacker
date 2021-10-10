#!/bin/sh

# Load EZX Environment (not necessary)
. /etc/initservices/services/ezxenv.sh

# One-liners for DCF file extensions
find /usr/data_resource/picture -type f -name "*.dcf*" | xargs -n1 sh -c '../drmhacker_magx "$0" `basename "$0" .dcf`'
find /usr/data_resource/sound/ringtones -type f -name "*.dcf*" | xargs -n1 sh -c '../drmhacker_magx "$0" `basename "$0" .dcf`'

# One-liners for DRM file extensions
find /usr/data_resource/picture -type f -name "*.drm*" | xargs -n1 sh -c '../drmhacker_magx "$0" `basename "$0" | sed -e 's/\.drm//g'`'
find /usr/data_resource/sound/ringtones -type f -name "*.drm*" | xargs -n1 sh -c '../drmhacker_magx "$0" `basename "$0" | sed -e 's/\.drm//g'`'

# Alternative one-liners by VINRARUS for DCF file extensions
find /usr/data_resource/picture -type f -name "*.dcf*" | while read FILE; do F="${FILE%.*}"; ../drmhacker_magx "$FILE" "${F##*/}"; done
find /usr/data_resource/sound/ringtones -type f -name "*.dcf*" | while read FILE; do F="${FILE%.*}"; ../drmhacker_magx "$FILE" "${F##*/}"; done

# Alternative one-liners by VINRARUS for DRM file extensions
find /usr/data_resource/picture -type f -name "*.drm*" | while read FILE; do F1="${FILE%.drm*}"; F2="${FILE##*.drm}"; ../drmhacker_magx "$FILE" "${F1##*/}$F2"; done
find /usr/data_resource/sound/ringtones -type f -name "*.drm*" | while read FILE; do F1="${FILE%.drm*}"; F2="${FILE##*.drm}"; ../drmhacker_magx "$FILE" "${F1##*/}$F2"; done

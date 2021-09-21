#!/bin/sh

# Load EZX Environment (not necessary)
. /etc/initservices/services/ezxenv.sh

# One-liners for DCF file extensions
find /usr/data_resource/picture -type f -name "*.dcf*" | xargs -n1 sh -c '../drmhacker_magx $0 `basename $0 .dcf`'
find /usr/data_resource/sound/ringtones -type f -name "*.dcf*" | xargs -n1 sh -c '../drmhacker_magx $0 `basename $0 .dcf`'

# One-liners for DRM file extensions
find /usr/data_resource/picture -type f -name "*.drm*" | xargs -n1 sh -c '../drmhacker_magx $0 `basename $0 | sed -E 's/\.drm//g'`'
find /usr/data_resource/sound/ringtones -type f -name "*.drm*" | xargs -n1 sh -c '../drmhacker_magx $0 `basename $0 | sed -E 's/\.drm//g'`'

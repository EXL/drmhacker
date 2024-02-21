#!/bin/bash

# Check if the required files exist
if [ ! -f CG43.smg ] || [ ! -f CG44.smg ] || [ ! -f CG52.smg ]; then
    echo "Error: Please check CG43.smg, CG44.smg, and CG52.smg files exist"
    exit 1
fi

rm -Rf squashfs-root

# Call unsquashfs tool on each file
rm -Rf magx-root/usr/setup
unsquashfs CG43.smg 
mv squashfs-root magx-root/usr/setup

rm -Rf magx-root/usr/securesetup
unsquashfs CG44.smg 
mv squashfs-root magx-root/usr/securesetup

rm -Rf magx-root/usr/data_resource
unsquashfs CG52.smg 
mv squashfs-root magx-root/usr/data_resource

echo "unsquashfs operations completed successfully."

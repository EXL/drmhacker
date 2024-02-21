#!/bin/bash

modprobe fuse
mount -t proc /proc magx-root/proc
mount --rbind /sys magx-root/sys
mount --rbind /dev magx-root/dev
touch /dev/efuse

mkdir /dev/fb
cd /dev/fb
ln -s ../fb0 0
cd -

chroot magx-root/ ash

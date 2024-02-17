drm_sp_hacker
=============

The "drm_sp_hacker" utility for decrypting OMA DRM files on Motorola EZX platform through DRM SP API.

## Build (Motorola E680, Motorola E680i)

```sh
. /opt/toolchains/motoe680/setenv-e680.sh
cd drmhacker_ezx/drm_sp
tmake drm_sp_hacker.pro -o Makefile
make
arm-linux-strip -s drm_sp_hacker
mv drm_sp_hacker drm_sp_hacker_e680
```

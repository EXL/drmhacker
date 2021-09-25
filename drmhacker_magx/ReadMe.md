drmhacker_magx
==============

// TODO: Add correct link to the forum thread!

MotoMAGX DRM hacker utility by [EXL](https://github.com/EXL/), additional information in this thread: [TODO](https://forum.motofan.ru/index.php?showtopic=143336) on the MotoFan.Ru forum.

This program can be used to decrypt DRM files on Motorola MotoMAGX phones: Z6, V8, Z6w, U9, ZN5, EM30, E8, VE66, EM35, VA76r etc.

## Build for MotoMAGX

// TODO: how to install toolchain/SDK, link

```
cd drmhacker_magx/
make clean
make
```

## Example

```
cd /mmc/mmca1/utils/
./drmhacker_magx /usr/data_resource/pictures/Picture.drm.jpg Picture.jpg
./drmhacker_magx /usr/data_resource/pictures/Picture.gif.dcf Picture.gif
```

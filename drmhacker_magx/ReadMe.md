drmhacker_magx
==============

MotoMAGX DRM hacker utility by [EXL](https://github.com/EXL/), additional information in this thread: [Decrypted DRM content of Motorola Linux phones on the EZX and MotoMAGX platform, Preserving the legacy of Motorola phones](https://forum.motofan.ru/index.php?showtopic=1742432) on the MotoFan.Ru forum.

This program can be used to decrypt DRM files on Motorola MotoMAGX phones: Z6, V8, Z6w, U9, ZN5, EM30, E8, VE66, EM35, VA76r etc.

## Build for MotoMAGX

// TODO: how to install toolchain/SDK, link

```bash
cd drmhacker_magx/
make clean
make

make -f Makefile.emu clean
make -f Makefile.emu
```

## Examples

```
Information:
	https://forum.motofan.ru/index.php?showtopic=1742432
Source code:
	https://github.com/EXL/drmhacker
Usage:
	drmhacker_magx <method> <in-file-path> <out-file-path>
Examples:
	./drmhacker_magx image.gif.dcf image.gif # Using Standard DRM API decryption mode (default).
	./drmhacker_magx image.drm.gif image.gif # Using Standard DRM API decryption mode (default).

	./drmhacker_magx drm sound.mp3.dcf sound.mp3 # Using Standard DRM API decryption mode.
	./drmhacker_magx drm_sp sound.mp3.dcf sound.mp3 # Using DRM SP API decryption mode.
	./drmhacker_magx drm_sp_reg sound.mp3.dcf sound.mp3 # Using DRM SP API decryption mode and register type.
	./drmhacker_magx player sound.mp3.dcf sound.mp3 # Using Media Player API decryption mode (not tested).
```

```bash
cd /mmc/mmca1/utils/
./drmhacker_magx drm_sp /usr/data_resource/pictures/Picture.drm.jpg Picture.jpg
./drmhacker_magx drm_sp /usr/data_resource/pictures/Animation.dcf Animation.gif
```

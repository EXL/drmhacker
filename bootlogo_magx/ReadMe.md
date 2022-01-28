bootlogo_magx
=============

Boot logotype extractor for Motorola MotoMAGX platform.

Usage:

```bash
./bootlogo_magx <screen size> <CG42 SMG file> <BMP image file>
./bootlogo_magx <screen size> <CG41 SMG file> <BMP image file>
./bootlogo_magx <screen size> <CG41 SMG file> <BMP image file> <depth>
```

Example:

```bash
./bootlogo_magx 240x320 CG42.smg bootlogo.bmp
./bootlogo_magx 128x160 CG41.smg bootlogo_cli.bmp
./bootlogo_magx 96x80 CG41.smg bootlogo_cli.bmp 16
```

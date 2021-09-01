drmhacker
=========

For Motorola EZX phones: A780, E680, E2, E6, A1200, A910, etc.

## Build for E2/E6/A1200

TODO: how to install toolchain/SDK, link

```
progen CONFIG+=thread LIBS="-lezxaid -lezxtapi" -o drmhacker.pro
tmake drmhacker.pro -o Makefile.ezx
make -f Makefile.ezx clean
make -f Makefile.ezx
```

## Build for E680/A780

TODO: how to install toolchain/SDK, link

```
progen CONFIG+=thread LIBS="-lezxjaid-xscale-r" -o drmhacker_e680.pro
tmake drmhacker_e680.pro -o Makefile.e680
make -f Makefile.e680 clean
make -f Makefile.e680
```

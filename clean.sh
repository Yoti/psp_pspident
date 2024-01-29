#!/usr/bin/env bash
cd ident_pbp/
make clean
cd ../
cd kernel_prx/
make clean
cd ../
rm -rf PSP/
if test -f EBOOT.PBP; then
	rm EBOOT.PBP
fi
if test -f kernel.prx; then
	rm kernel.prx
fi

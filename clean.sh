#!/usr/bin/env bash
if test -d PSP; then
	rm -rf PSP/
fi
cd ident_pbp/
make clean>/dev/null
if test -f data.psp; then
	rm -f data.psp
fi
if test -f EBOOT_DEC.PBP; then
	rm -f EBOOT_DEC.PBP
fi
if test -f EBOOT_ENC.PBP; then
	rm -f EBOOT_ENC.PBP
fi
cd ../

#!/usr/bin/env bash
if test -d PSP; then
	rm -rf PSP/
fi
if test -f EBOOT.PBP; then
	rm -f EBOOT.PBP
fi
cd ident_pbp/
make clean >nul
if test -f data.psp; then
	rm -f data.psp
fi
cd ../

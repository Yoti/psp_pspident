#!/usr/bin/env bash
if test -d PSP; then
	rm -rf PSP/
fi
cd ident_pbp/
make clean>/dev/null
if test -f data.psp; then
	rm -f data.psp
fi
cd ../

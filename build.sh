#!/usr/bin/env bash
rm -f EBOOT.PBP
cd ident_pbp/
make clean
make
cp EBOOT.PBP ../
cd ../
rm -rf PSP/
mkdir -p PSP/GAME/pspIdent/
cp EBOOT.PBP PSP/GAME/pspIdent/

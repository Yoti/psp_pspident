#!/usr/bin/env bash
rm -f EBOOT.PBP
cd ident_pbp/
make clean
cd ../
rm -f kernel.prx
cd kernel_prx/
make clean
make
psp-build-exports -s kernel_exp.exp
cp kernel.prx ../
cd ../
cd ident_pbp
make
cp EBOOT.PBP ../
cd ../
rm -rf PSP/
mkdir -p PSP/GAME/pspIdent/
cp kernel.prx PSP/GAME/pspIdent/
cp EBOOT.PBP PSP/GAME/pspIdent/
release="pspIdent_v1.0_"$(date +'%Y%m%d-%H%M%S')".zip"
zip -r $release PSP

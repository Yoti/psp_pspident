#!/usr/bin/env bash
rm -f EBOOT.PBP
rm -f EBOOT~.PBP
cd ident_pbp/
make clean
make
mv EBOOT.PBP EBOOT~.PBP
../pencrypt/pencrypt eboot.prx
echo EBOOT.PBP PARAM.SFO ICON0.PNG NULL NULL NULL NULL data.psp NULL
pack-pbp EBOOT.PBP PARAM.SFO ICON0.PNG NULL NULL NULL NULL data.psp NULL
cp EBOOT.PBP ../
cd ../
rm -rf PSP/
mkdir -p PSP/GAME/pspIdent/
cp EBOOT.PBP PSP/GAME/pspIdent/

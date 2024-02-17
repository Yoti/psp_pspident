#!/usr/bin/env bash
rm -f EBOOT.PBP
rm -f EBOOT~.PBP
cd ident_pbp/
make clean>/dev/null
make>/dev/null
rm -f EBOOT.PBP
../pencrypt/pencrypt eboot.prx
echo Building encrypted EBOOT.PBP...
pack-pbp EBOOT.PBP PARAM.SFO ICON0.PNG NULL NULL NULL NULL data.psp NULL
if test -f EBOOT.PBP; then
	echo EBOOT.PBP was successfully encrypted!
fi
cd ../
rm -rf PSP/
mkdir -p PSP/GAME/pspIdent/
mv ident_pbp/EBOOT.PBP PSP/GAME/pspIdent/

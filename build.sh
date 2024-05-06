#!/usr/bin/env bash
cd ident_pbp/
make clean>/dev/null
make>/dev/null
python3 ../psptools/pack_ms_game.py --vanity pspIdentByYotiAndFriendsByYotiAndFriends EBOOT.PBP EBOOT_ENC.PBP
mv EBOOT.PBP EBOOT_DEC.PBP
../pencrypt/pencrypt eboot.prx
echo Building fake-encrypted EBOOT.PBP...
pack-pbp EBOOT.PBP PARAM.SFO ICON0.PNG NULL NULL NULL NULL data.psp NULL
if test -f EBOOT.PBP; then
	echo EBOOT.PBP was successfully fake-encrypted!
fi
cd ../
rm -rf PSP/
mkdir -p PSP/GAME/pspIdent/
cp ident_pbp/EBOOT.PBP PSP/GAME/pspIdent/

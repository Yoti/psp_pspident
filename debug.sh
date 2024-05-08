#!/usr/bin/env bash
cd ident_pbp/
make clean>/dev/null

echo Building decrypted EBOOT.PBP...
make -f Debug
if test -f EBOOT.PBP; then
	mv EBOOT.PBP EBOOT_DEC.PBP
	echo EBOOT.PBP was successfully created!
fi

echo Building encrypted EBOOT.PBP...
python3 ../psptools/pack_ms_game.py --vanity pspIdentByYotiAndFriendsByYotiAndFriends EBOOT_DEC.PBP EBOOT_ENC.PBP
if test -f EBOOT_ENC.PBP; then
	echo EBOOT.PBP was successfully encrypted!
fi

cd ../
rm -rf PSP/
mkdir -p PSP/GAME/pspIdent_Debug/
cp ident_pbp/EBOOT_DEC.PBP PSP/GAME/pspIdent_Debug/EBOOT.PBP

#!/usr/bin/env bash
rm *.zip>/dev/null
release="pspIdent_v3.1_"$(date +'%Y%m%d-%H%M%S')".zip"
zip -r $release PSP

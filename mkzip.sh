#!/usr/bin/env bash
rm *.zip
release="pspIdent_v3.0_"$(date +'%Y%m%d-%H%M%S')".zip"
zip -r $release PSP

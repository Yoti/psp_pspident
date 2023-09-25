#!/usr/bin/env bash
rm *.zip
release="pspIdent_v2.0_"$(date +'%Y%m%d-%H%M%S')".zip"
zip -r $release PSP

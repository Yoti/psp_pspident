#!/usr/bin/env bash
rm *.zip
release="pspIdent_v1.5_"$(date +'%Y%m%d-%H%M%S')".zip"
zip -r $release PSP

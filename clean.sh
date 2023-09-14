#!/usr/bin/env bash
cd ident_pbp/
make clean
cd ../
cd kernel_prx/
make clean
cd ../
rm -rf PSP/
rm EBOOT.PBP
rm kernel.prx

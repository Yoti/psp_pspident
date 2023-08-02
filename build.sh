cd ident_pbp/
make clean
cd ../
cd kernel_prx/
make clean
make
psp-build-exports -s kernel_exp.exp
cp kernel.prx ../
cd ../
cd ident_pbp
make
cp EBOOT.PBP ../

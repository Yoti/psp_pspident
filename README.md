# psp_pspident
New PSPident named pspIdent

Mostly rewrited from scratch

Open source and under MIT license

![README.png](README.png?raw=true "README.png")

## Changelog
### v1.0
* Initial release
### v1.5
* Added shipped FW after current FW (if exists)
* Added PSP region inside model number
* Partial TA-088v1/v2 detection
* Partial TA-096/097 detection
* Basic DevTool/TestTool/AV Tool support
* Screenshots now saved as PNG

## Building
```
cd kernel_prx/
make
psp-build-exports -s kernel_exp.exp
cd ../
cd ident_pbp/
make
```

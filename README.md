# psp_pspident
New PSPident named pspIdent

Mostly rewrited from scratch

Open source and under MIT license

![README.png](README.png?raw=true "README.png")

## Changelog
### v1.0 "Aquavit"
* Initial release
### v1.5 "Bourbon"
* Added shipped FW after current FW (if exists)
* Added PSP region inside model number
* Partial TA-088v1/v2 detection
* Partial TA-096/097 detection
* Basic DevTool/TestTool/AV Tool support
* Screenshots now saved as PNG
### v2.0 "Campari"
* Added `version.txt` file output on screen
* Added Wi-Fi region from IDS (except E1000)
* Added Bluetooth MAC from IDS (only PSP Go)
* Shipped FW now has it's own line on screen
* Partial TA-088v1/v2 detection (new algo)
* Removed duplicated line with generation
* Added QA Flag detection
### v3.0 "Drambuie"
* Added UMD FW version (except Go, by @krazynez)
* PSP Go TA-094 (codename Strider2) detection
* pspIdent now runs on OFW (by @Acid_Snake)

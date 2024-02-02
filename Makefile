TARGET = eboot
OBJS = main.o kernel.o ./lodepng/lodepng.o

UNAME := $(shell uname)


release: all
ifeq ($(UNAME), Linux)
	WINEPREFIX=$(shell pwd)/prefix wine $(shell pwd)/bin/prxEncrypter.exe $(TARGET).prx
else
	$(shell pwd)\bin\prxEncrypter $(TARGET).prx
endif
	pack-pbp $(EXTRA_TARGETS) PARAM.SFO ICON0.PNG NULL NULL NULL NULL data.psp NULL


INCDIR = ./inc
CFLAGS = -O2 -G0 -Wall -Wno-trigraphs
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

PSP_FW_VERSION = 300

BUILD_PRX = 1
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = pspIdent v2.5 by Yoti
PSP_EBOOT_ICON = ICON0.PNG

LIBDIR = ./lib
LIBS = -lpspexploit -lpsprtc
LDFLAGS = -L. 

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

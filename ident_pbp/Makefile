TARGET = eboot
OBJS = main.o kernel.o ../lodepng/lodepng.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall -Wno-trigraphs
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

PSP_FW_VERSION = 271

BUILD_PRX = 1
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = pspIdent v2.5 by Yoti
PSP_EBOOT_ICON = ICON0.PNG

LIBDIR = 
LIBS = -lpspexploit -lpsprtc
LDFLAGS = -L.

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

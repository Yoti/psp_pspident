TARGET = eboot
OBJS = main.o kernel.o ../lodepng/lodepng.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall -Wno-trigraphs -Wno-int-conversion -Wno-incompatible-pointer-types
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

PSP_FW_VERSION = 271

BUILD_PRX = 1
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = pspIdent v3.2 by Yoti and friends
PSP_EBOOT_ICON = ICON0.PNG

LIBDIR = ../kernel_lib
LIBS = -lpspexploit -lpsprtc
LDFLAGS = -L.

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

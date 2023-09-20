TARGET = eboot
OBJS = main.o ../kernel_prx/kernel_prx.S

INCDIR = 
CFLAGS = -O2 -G0 -Wall -Wno-restrict
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

PSP_FW_VERSION = 300

BUILD_PRX = 1
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = pspIdent v1.1 by Yoti
PSP_EBOOT_ICON = ICON0.PNG

LIBDIR = 
LIBS = 
LDFLAGS = -L.

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

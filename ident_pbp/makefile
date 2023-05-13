TARGET = eboot
OBJS = main.o ../kernel_prx/kernel_prx.S

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

PSP_FW_VERSION = 371

BUILD_PRX = 1
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = pspIdent v1.0
#PSP_EBOOT_ICON = ICON0.PNG

LIBDIR = 
LIBS = 
LDFLAGS = -L.

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

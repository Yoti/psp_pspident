TARGET = eboot
OBJS = main.o
OBJS += ../kernel_prx/kernel_prx.S

INCDIR = ./inc ./inc_lib
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ./lib ./inc_lib
LIBS = 
LDFLAGS = -L.

PSP_FW_VERSION = 300
PSP_FW_VERSION_SFO = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = pspIdent v1.0
#PSP_EBOOT_ICON = icon0.png
#PSP_EBOOT_PIC1 = pic1.png
#PSP_EBOOT_UNKPNG = pic0.png

#ENCRYPT = 1
BUILD_PRX = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

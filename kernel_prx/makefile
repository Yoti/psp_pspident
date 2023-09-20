TARGET = kernel
OBJS = kernel_prx.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION = 300

BUILD_PRX = 1
PRX_EXPORTS = kernel_exp.exp
USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

LIBDIR = 
LIBS = 
LDFLAGS = -nostartfiles

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

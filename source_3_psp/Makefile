TARGET = intraFontTest
OBJS = fonttest.o common/callbacks.o common/vram.o common/gu.o eight_input.o

INCDIR =
# Map作成追加
# CFLAGS = -O2 -G0 -Wall -g
CFLAGS = -O2 -G0 -Wl,-Map=$(TARGET).map -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
# map 生成追加
# LDFLAGS = 
LDFLAGS =
LIBS = -lintraFont -lpspgum -lpspgu -lm -lstdc++

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = intraFont Test

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak


# minimalistPSPSDK(windows) では ビルドできません。
# → -std=c++11 を使用するため。
#    gcc 4.3.
# psptoolchain を 使用して、linux上の環境でビルドしています。
#    ver 4.8.5 使用
TARGET = intraFontTest
BUILD = _build
OBJS =	fonttest.o common/callbacks.o \
		common/vram.o common/gu.o \
		eight_input.o intraFont/intraFont.o \
		intraFont/libccc.o

INCDIR =
# Map作成追加
# CFLAGS = -O2 -G0 -Wall -g
CFLAGS = -O2 -G0 -Wl,-Map=$(TARGET).map -g
# "-fshort-wchar" wchar 4バイト → 2バイト化 : これで (L"日本語") のようなリテラル 有効となる (参考:http://youkosodododo.blog54.fc2.com/blog-entry-17.html)
# -->  他のライブラリなども 同じオプションでビルドされていないとダメとのこと。。。(swprintf 使えず。元に戻す)
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -std=c++11
ASFLAGS = $(CFLAGS)
LIBDIR =
# map 生成追加
# LDFLAGS = 
LDFLAGS =
LIBS = -lpspgum -lpspgu -lm -lstdc++

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = intraFont Test

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak


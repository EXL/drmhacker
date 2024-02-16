# This Makefile was edited by EXL, 16-Feb-2022
# Default MotoMAGX platform is Motorola ZINE ZN5.

####### Config
APPNAME     := playm_ZN5

####### SDK and Toolchains
TOOLPREFIX  := /arm-eabi
ARMLIB      := $(TOOLPREFIX)/arm-linux-gnueabi/lib

####### Default Platform
QTDIR       := $(TOOLPREFIX)/lib/qt-zn5
EZXDIR      := $(TOOLPREFIX)/lib/ezx-zn5
LINKLIB     := -lezx_mp_playerengine -lqte-mt
DIRECTIV    := -DEZX_ZN5 $(APP_DEFINES)
TARGET       = $(APPNAME)

####### Build Tools and Options
MOC          = $(QTDIR)/bin/moc
CC           = $(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
CXX          = $(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
STRIP        = $(TOOLPREFIX)/bin/arm-linux-gnueabi-strip
INCPATH      = -I. -I.. -I$(QTDIR)/include -I$(EZXDIR)/include -I$(TOOLPREFIX)/arm-linux-gnueabi/include
CFLAGS       = -pipe -Wall -W -O2 $(DIRECTIV) $(INCPATH)
CXXFLAGS     = -pipe -DQWS -fno-exceptions -fno-rtti -Wall -W -O2 $(DIRECTIV) $(INCPATH)
LDLAGS       = -Wl,-rpath-link,$(EZXDIR)/lib
LIBS         = -L$(EZXDIR)/lib -L$(ARMLIB) -L$(QTDIR)/lib $(LINKLIB)

####### Files
SOURCES      = playm.cpp
OBJECTS      = playm.o
SRCMOC       = playm.moc

all: $(SRCMOC) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)
	$(STRIP) -s $(TARGET)

moc: $(SRCMOC)

tar: $(TARGET)
	-tar -cvf $(TARGET).tar $(TARGET)

rmobj:
	-rm -f $(OBJECTS)
	-rm -f *~ core
	$(RM) *.o
	$(RM) *.moc

clean: rmobj
	$(RM) $(TARGET) *.tar

#--------- moc's -----------------------------
playm.moc: playm.cpp
	$(MOC) playm.cpp -o playm.moc

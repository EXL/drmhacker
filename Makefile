# This Makefile edited by EXL, 14-Jul-2021
#
# App can compile for platform:
#    PLATFORM=EZX-Z6
#    PLATFORM=EZX-V8
#    PLATFORM=EZX-Z6W
#    PLATFORM=EZX-U9
#    PLATFORM=EZX-ZN5 (default)
#    PLATFORM=EZX-EM30
#    PLATFORM=EZX-E8
#    PLATFORM=EZX-VE66
#    PLATFORM=EZX-EM35
#
# For example - compile this source for Motorla ZN5:
#
# cd /PROJECT_FOLDER/
# make clean
# make PLATFORM=EZX-ZN5
#

####### Config
APPNAME     := undcf
APP_DEFINES :=

####### SDK and Toolchains
TOOLPREFIX  := /arm-eabi
ARMLIB      := $(TOOLPREFIX)/arm-linux-gnueabi/lib

####### Default Platform
ifeq ($(PLATFORM),)
PLATFORM     = EZX-ZN5
endif

####### Platforms
ifeq ($(PLATFORM),EZX-Z6)
QTDIR       := $(TOOLPREFIX)/lib/qt-2.3.8
EZXDIR      := $(TOOLPREFIX)/lib/ezx-z6
LINKLIB     := -lm -lqte-mt -lezxpm -lezxappbase -llighting
DIRECTIV    := -DEZX_Z6 $(APP_DEFINES)
TARGET       = $(APPNAME)_Z6
endif
ifeq ($(PLATFORM),EZX-V8)
QTDIR       := $(TOOLPREFIX)/lib/qt-v8
EZXDIR      := $(TOOLPREFIX)/lib/ezx-v8
LINKLIB     := -lm -lqte-mt -lezxpm -lezxappbase
DIRECTIV    := -DEZX_V8 $(APP_DEFINES)
TARGET       = $(APPNAME)_V8
endif
ifeq ($(PLATFORM),EZX-Z6W)
QTDIR       := $(TOOLPREFIX)/lib/qt-z6w
EZXDIR      := $(TOOLPREFIX)/lib/ezx-z6w
LINKLIB     := -lm -lqte-mt -lezxappbase
DIRECTIV    := -DEZX_Z6W $(APP_DEFINES)
TARGET       = $(APPNAME)_Z6W
endif
ifeq ($(PLATFORM),EZX-U9)
QTDIR       := $(TOOLPREFIX)/lib/qt-zn5
EZXDIR      := $(TOOLPREFIX)/lib/ezx-u9
LINKLIB     := -lm -lqte-mt -lezxappbase -llighting
DIRECTIV    := -DEZX_U9 $(APP_DEFINES)
TARGET       = $(APPNAME)_U9
endif
ifeq ($(PLATFORM),EZX-ZN5)
QTDIR       := $(TOOLPREFIX)/lib/qt-zn5
EZXDIR      := $(TOOLPREFIX)/lib/ezx-zn5
LINKLIB     := -lqte-mt -ldrmfwudaclient
DIRECTIV    := -DEZX_ZN5 $(APP_DEFINES)
TARGET       = $(APPNAME)_ZN5
endif
ifeq ($(PLATFORM),EZX-EM30)
ARMLIB      := $(TOOLPREFIX)/arm-linux-gnueabi/lib_E8
QTDIR       := $(TOOLPREFIX)/lib/qt-em30
EZXDIR      := $(TOOLPREFIX)/lib/ezx-em30
EZXDIR2     := $(TOOLPREFIX)/lib/ezx-e8
LINKLIB     := -lm -lqte-mt -lezxappbase
DIRECTIV    := -DEZX_EM30 $(APP_DEFINES)
TARGET       = $(APPNAME)_EM30
endif
ifeq ($(PLATFORM),EZX-E8)
ARMLIB      := $(TOOLPREFIX)/arm-linux-gnueabi/lib_E8
QTDIR       := $(TOOLPREFIX)/lib/qt-e8
EZXDIR      := $(TOOLPREFIX)/lib/ezx-e8
LINKLIB     := -lm -lqte-mt -lezxappbase
DIRECTIV    := -DEZX_E8 $(APP_DEFINES)
TARGET       = $(APPNAME)_E8
endif
ifeq ($(PLATFORM),EZX-VE66)
ARMLIB      := $(TOOLPREFIX)/arm-linux-gnueabi/lib_E8
QTDIR       := $(TOOLPREFIX)/lib/qt-em35
EZXDIR      := $(TOOLPREFIX)/lib/ezx-ve66
LINKLIB     := -lm -lqte-mt -lezxappbase
DIRECTIV    := -DEZX_VE66 $(APP_DEFINES)
TARGET       = $(APPNAME)_VE66
endif
ifeq ($(PLATFORM),EZX-EM35)
ARMLIB      := $(TOOLPREFIX)/arm-linux-gnueabi/lib_E8
QTDIR       := $(TOOLPREFIX)/lib/qt-em35
EZXDIR      := $(TOOLPREFIX)/lib/ezx-em35
LINKLIB     := -lm -lqte-mt -lezxappbase
DIRECTIV    := -DEZX_EM35 $(APP_DEFINES)
TARGET       = $(APPNAME)_EM35
endif

####### Build Tools and Options
MOC          = $(QTDIR)/bin/moc
CC           = $(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
CXX          = $(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
STRIP        = $(TOOLPREFIX)/bin/arm-linux-strip
INCPATH      = -I../src/ -I$(QTDIR)/include -I$(EZXDIR)/include -I $(TOOLPREFIX)/arm-linux-gnueabi/include
CFLAGS       = -pipe -Wall -W -O2 -DNO_DEBUG $(DIRECTIV) $(INCPATH)
CXXFLAGS     = -pipe -DQWS -fno-exceptions -fno-rtti -Wall -W -O2 -DNO_DEBUG $(DIRECTIV) $(INCPATH)
LDLAGS       = -Wl,-rpath-link,$(EZXDIR)/lib
LIBS         = -L$(EZXDIR)/lib -L$(ARMLIB) -L$(QTDIR)/lib $(LINKLIB)

####### Files
SOURCES      = undcf.cpp
OBJECTS      = undcf.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)
	$(STRIP) -s $(TARGET)
	mv $(TARGET) $(APPNAME)

rmobj:
	-rm -f $(OBJECTS)
	-rm -f *~ core
	$(RM) *.o

clean: rmobj
	$(RM) $(APPNAME) $(TARGET) $(APPNAME)_* *.tar

#############################################################################
# Makefile for building drmhacker_ezx
# Generated by tmake at 05:31, 2022/01/27
#     Project: drmhacker_ezx
#    Template: app
#############################################################################

####### Compiler, tools and options

CC	=	arm-linux-gnu-gcc
CXX	=	arm-linux-gnu-g++
STRIP = arm-linux-gnu-strip
CFLAGS	=	-pipe -DEZX_E6 -Wall -W -O2 -mcpu=iwmmxt -mtune=iwmmxt -D_REENTRANT -DQT_THREAD_SUPPORT -DNO_DEBUG
CXXFLAGS=	-pipe -DEZX_E6 -DQWS -fno-exceptions -fno-rtti -Wall -W -O2 -mcpu=iwmmxt -mtune=iwmmxt -D_REENTRANT -DQT_THREAD_SUPPORT -DNO_DEBUG
INCPATH	=	-I$(QTDIR)/include -I$(EZXDIR)/include
LINK	=	arm-linux-gnu-g++
LFLAGS	=	-Wl,-rpath-link,$(QTDIR)/lib,-rpath-link,$(EZXDIR)/lib
LIBS	=	$(SUBLIBS) -L$(QTDIR)/lib -L$(EZXDIR)/lib -L$(EZXDIR)/lib/ezx/lib -lezxappbase -lezxaid -lezxtapi -lpthread -lqte-mt
MOC	=	$(QTDIR)/bin/moc
UIC	=	$(QTDIR)/bin/uic

TAR	=	tar -cf
GZIP	=	gzip -9f

####### Files

HEADERS =	drmfile.h
SOURCES =	main.cpp
OBJECTS =	main.o
INTERFACES =	
UICDECLS =	
UICIMPLS =	
SRCMOC	=	
OBJMOC	=	
DIST	=	
TARGET	=	drmhacker_ezx
INTERFACE_DECL_PATH = .

####### Implicit rules

.SUFFIXES: .cpp .cxx .cc .C .c

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules


all: $(TARGET)

$(TARGET): $(UICDECLS) $(OBJECTS) $(OBJMOC) 
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(LIBS)
	$(STRIP) -s $(TARGET)

moc: $(SRCMOC)

tmake: Makefile

Makefile: drmhacker_ezx.pro
	tmake drmhacker_ezx.pro -o Makefile

dist:
	$(TAR) drmhacker_ezx.tar drmhacker_ezx.pro $(SOURCES) $(HEADERS) $(INTERFACES) $(DIST)
	$(GZIP) drmhacker_ezx.tar

clean:
	-rm -f $(OBJECTS) $(OBJMOC) $(SRCMOC) $(UICIMPLS) $(UICDECLS) $(TARGET)
	-rm -f *~ core

####### Sub-libraries


###### Combined headers


####### Compile

main.o: main.cpp \
		drmfile.h


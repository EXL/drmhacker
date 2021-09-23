#!/bin/sh

. /home/native/.profile

if [ -z $1 ]
then
	echo "EZX DRM hacker by Borman v1.0"
	echo "wrapper script"
	echo "Usage: `basename $0` <path_to_protected_java>"
	exit
fi

DIR=`pwd`
mkdir -p $DIR/java
JAVANAME=`basename $1`
JAVANUM=`echo -n $1 | sed -e 's/.*MIDlet\([0-9]*\).*/\1/g'`
NEWJAVANAME=`echo -n $JAVANAME | sed -e s/\.drm/$JAVANUM/g`
echo -n "Hacking [$JAVANAME]->[$NEWJAVANAME]... "
(./drmhacker_ezx $1 $DIR/java/$NEWJAVANAME > /dev/null && echo "ok") || echo "failed"

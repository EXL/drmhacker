#!/bin/sh

. /etc/initservices/services/ezxenv.sh

cd /usr/SYSqtapp/drmfs
./drmfs &

cd /usr/SYSqtapp/drmdaemon
./udadaemon &

cd /


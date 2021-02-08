#***************************************************************
#  Program:      Makefile
#  Version:      20210122
#  Author:       Sifan S. Kahale
#  Description:  INDI PowerStar Ports
#***************************************************************
CFLAGS = -O2 -Wall -lrt -std=c++11
CC = g++ 

all: hid control psports

hid:
	cc -Wall -g -fpic -c -Ihidapi `pkg-config libusb-1.0 --cflags` hid.c -o hid.o
	
control:
	$(CC) $(CFLAGS)  -g -fpic -c -Ihidapi `pkg-config libusb-1.0 --cflags` PScontrol.cpp -o PScontrol.o

psports:
	$(CC) $(CFLAGS) -I/usr/include -I/usr/include/libindi -c PSports.cpp
	
	$(CC) $(CFLAGS) -rdynamic hid.o PScontrol.o PSports.o `pkg-config libusb-1.0 --libs` -lpthread -o indi_powerstarports -lpigpio -lindidriver -lindiAlignmentDriver -lrt

clean:
	@rm -rf *.o indi_powerstarports

install:
	\cp -f indi_powerstarports /usr/bin/
	\cp -f indi_powerstarports.xml /usr/share/indi/
	service indiwebmanager stop
	service indiwebmanager start


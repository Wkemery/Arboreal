##############################
# Makefile
# Author: Arboreal 
# Date: OCT 12, 2017
#
# Senior Design project for Arboreal
# 
##############################

CC=g++
CFLAGS = -ggdb -Wall -gstabs -std=c++11


all: driver

driver: driver.cpp client.h bitvector.h bitvector.cpp disk.h disk.cpp diskmanager.h diskmanager.cpp partitionmanager.h partitionmanager.cpp filesystem.h filesystem.cpp Trees.h Trees.cpp
	$(CC) $(CFLAGS) -o driver driver.cpp bitvector.cpp disk.cpp diskmanager.cpp partitionmanager.cpp filesystem.cpp Trees.cpp

clean: 
	/bin/rm -f *.o driver DISK1

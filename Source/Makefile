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

 
bitvector.o: bitvector.cpp  bitvector.h
	$(CC) $(CFLAGS) -c bitvector.cpp

bitvector_driver: bitvector.o
	$(CC) $(CFLAGS) bitvector_driver.cpp bitvector.o -o bitvector_driver

disk.o: disk.cpp  disk.h
	$(CC) $(CFLAGS) -c disk.cpp

diskmanager.o: diskmanager.cpp  diskmanager.h
	$(CC) $(CFLAGS) -c diskmanager.cpp

partitionmanager.o: partitionmanager.cpp  partitionmanager.h
	$(CC) $(CFLAGS) -c partitionmanager.cpp

filesystem.o: filesystem.cpp  filesystem.h
	$(CC) $(CFLAGS) -c filesystem.cpp

driver: driver.cpp client.h bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o
	$(CC) $(CFLAGS) -o driver driver.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o

clean: 
	rm -f bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o driver1 driver2 driver3 driver4 driver5 driver6 driver bitvector_driver DISK1

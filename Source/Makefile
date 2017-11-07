##############################
# Makefile
# Author: Arboreal 
# Date: OCT 12, 2017
#
# Senior Design project for Arboreal
# 
##############################

CXX=g++
XXFLAGS = -ggdb -Wall -gstabs -std=c++11

CPPFILES=driver.cpp bitvector.cpp disk.cpp diskmanager.cpp partitionmanager.cpp filesystem.cpp Trees.cpp
HFILES= bitvector.h disk.h disk.cpp diskmanager.h partitionmanager.h filesystem.h Trees.h
all: driver

driver: $(CPPFILES) $(HFILES)
	$(CXX) $(XXFLAGS) -o driver $(CPPFILES)

clean: 
	/bin/rm -f *.o driver DISK1

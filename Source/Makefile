##############################
# Makefile
# Author: Arboreal 
# Date: OCT 12, 2017
#
# Senior Design project for Arboreal
# 
##############################

CXX=g++
XXFLAGS = -ggdb -Wall -std=c++11 

CPPFILES=driver.cpp disk.cpp diskmanager.cpp partitionmanager.cpp filesystem.cpp Trees.cpp Arboreal_Exceptions.cpp
HFILES=disk.h disk.cpp diskmanager.h partitionmanager.h filesystem.h Trees.h Arboreal_Exceptions.h
all: driver

driver: $(CPPFILES) $(HFILES)
	$(CXX) $(XXFLAGS) -o driver $(CPPFILES)

format: format.cpp
	$(CXX) $(XXFLAGS) -o format format.cpp

clean: 
	/bin/rm -f *.o driver format DISK1

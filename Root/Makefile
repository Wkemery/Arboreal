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

daemon: ConnectionDaemon.cpp
	$(CXX) $(XXFLAGS) -o daemon ConnectionDaemon.cpp

driver: $(CPPFILES) $(HFILES)
	$(CXX) $(XXFLAGS) -o driver $(CPPFILES)

format: format.cpp
	$(CXX) $(XXFLAGS) -o format format.cpp

cli: clean cli_driver.cpp cli.h
	$(CXX) $(XXFLAGS) cli.cpp cli_driver.cpp -o cli

liason: clean liason.cpp
	$(CXX) $(XXFLAGS) liason.cpp -o liason

clean: 
	/bin/rm -f *.o driver cli liason daemon format DISK1 *.pipe

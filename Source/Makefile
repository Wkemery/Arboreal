###################################################################################################
#
# Makefile
# Makefile for "Arboreal"
# By: "Arboreal" Team
# 
#  Mon. | Feb. 5th | 2018 | 8:30 AM
#
###################################################################################################

CXX=g++
XXFLAGS = -ggdb -Wall -std=c++11 

CPPFILES=driver.cpp disk.cpp diskmanager.cpp partitionmanager.cpp filesystem.cpp Trees.cpp Arboreal_Exceptions.cpp
OBJFILES=driver.o disk.o diskmanager.o partitionmanager.o filesystem.o Trees.o Arboreal_Exceptions.o
HFILES=disk.h disk.cpp diskmanager.h partitionmanager.h filesystem.h Trees.h Arboreal_Exceptions.h types.h

all: driver

driver: $(OBJFILES) $(HFILES)
	$(CXX) $(XXFLAGS) -o driver $(OBJFILES)

format: format.cpp
	$(CXX) $(XXFLAGS) -o format format.cpp

cli: clean cli_driver.cpp cli.h
	$(CXX) $(XXFLAGS) cli.cpp cli_driver.cpp -o cli

liason: clean liason.cpp
	$(CXX) $(XXFLAGS) liason.cpp -o liason

driver.o: driver.cpp
	$(CXX) $(XXFLAGS) -c driver.cpp

disk.o: disk.cpp
	$(CXX) $(XXFLAGS) -c disk.cpp

diskmanager.o: diskmanager.cpp
	$(CXX) $(XXFLAGS) -c diskmanager.cpp

partitionmanager.o: partitionmanager.cpp
	$(CXX) $(XXFLAGS) -c partitionmanager.cpp

filesystem.o: filesystem.cpp
	$(CXX) $(XXFLAGS) -c filesystem.cpp

Trees.o: Trees.cpp
	$(CXX) $(XXFLAGS) -c Trees.cpp

Arboreal_Exceptions.o: Arboreal_Exceptions.cpp
	$(CXX) $(XXFLAGS) -c Arboreal_Exceptions.cpp

clean:
	/bin/rm -f *.o driver cli format DISK1 *.pipe liason

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
HFILES=disk.h disk.cpp diskmanager.h partitionmanager.h filesystem.h Trees.h Arboreal_Exceptions.h
all: driver

driver: $(CPPFILES) $(HFILES)
	$(CXX) $(XXFLAGS) -o driver $(CPPFILES)

format: format.cpp
	$(CXX) $(XXFLAGS) -o format format.cpp

cli: clean-cli cli_driver.cpp cli.h
	$(CXX) $(XXFLAGS) cli.cpp cli_driver.cpp -o cli

liason: clean-lson liason.cpp
	$(CXX) $(XXFLAGS) liason.cpp -o liason

clean-cli:
	/bin/rm -f cli

clean-lson:
	/bin/rm -f liason

clean: 
	/bin/rm -f *.o driver cli format DISK1 liason



	
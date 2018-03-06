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
XXFLAGS= -ggdb -Wall -std=c++11 -pthread
VALGRIND= valgrind --leak-check=yes --log-file=/home/wyatt/ValgrindLogs/%p.log
VALOPS= --leak-check=full --show-leak-kinds=all
CPPFILES=disk.cpp diskmanager.cpp partitionmanager.cpp filesystem.cpp Trees.cpp Arboreal_Exceptions.cpp File.cpp\
         ConnectionDaemon.cpp
OBJFILES=disk.o diskmanager.o partitionmanager.o filesystem.o Trees.o Arboreal_Exceptions.o File.o
HFILES=disk.h diskmanager.h partitionmanager.h filesystem.h Trees.h Arboreal_Exceptions.h types.h File.h

all: filesystem

filesystem: $(OBJFILES)  cli liason
	$(CXX) $(XXFLAGS) -o daemon $(CPPFILES)

driver: $(OBJFILES)
	$(CXX) $(XXFLAGS) -o driver driver.cpp $(OBJFILES)

format: format.cpp
	$(CXX) $(XXFLAGS) -o format format.cpp

cli: clean cli_driver.cpp cli.h Arboreal_Exceptions.h
	$(CXX) $(XXFLAGS) cli.cpp cli_driver.cpp -o cli Arboreal_Exceptions.cpp

liason: clean liason.cpp Arboreal_Exceptions.h
	$(CXX) $(XXFLAGS) liason.cpp -o liason Arboreal_Exceptions.cpp

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

word_count: 
	/usr/bin/wc *.h *.cpp

reset:
	/bin/rm -f *.o driver DISK1
	make
	./format diskInfo.d
	./driver 6
	./driver 7
	./driver 8

rest:
	/bin/rm -f driver DISK1
	make
	make format
	./format diskInfo.d
	./driver 6
	./driver 7
	./driver 8

mem_test: mem_prep mem_test_6_8 mem_test_9 mem_test_10 mem_test_11 mem_test_12
	
mem_prep:
	/bin/rm -f driver DISK1
	make
	make format
	./format diskInfo.d
	
mem_test_6_8:
	$(VALGRIND) ./driver 6
	$(VALGRIND) ./driver 7
	$(VALGRIND) ./driver 8

mem_test_9: rest
	$(VALGRIND) ./driver 9

mem_test_10: rest
	$(VALGRIND) ./driver 10

mem_test_11: rest
	$(VALGRIND) ./driver 11

mem_test_12: rest
	$(VALGRIND) ./driver 12

clean:
	/bin/rm -f *.o driver cli format daemon DISK1 *.pipe liason

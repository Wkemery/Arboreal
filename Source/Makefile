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


OBJFILES=disk.o diskmanager.o partitionmanager.o filesystem.o Trees.o Arboreal_Exceptions.o File.o
HFILES=disk.h diskmanager.h partitionmanager.h filesystem.h Trees.h Arboreal_Exceptions.h types.h File.h
CPPFILES=disk.cpp diskmanager.cpp partitionmanager.cpp filesystem.cpp Trees.cpp Arboreal_Exceptions.cpp File.cpp\
         ConnectionDaemon.cpp

FS_PATH=Filesystem
FS_H=$(FS_PATH)/Arboreal_Exceptions.h $(FS_PATH)/File.h $(FS_PATH)/types.h
FS_CPP=$(FS_PATH)/Arboreal_Exceptions.cpp $(FS_PATH)/File.cpp
BACKEND_PATH=Filesystem/Backend
BACKEND_H=$(BACKEND_PATH)/disk.h $(BACKEND_PATH)/diskmanager.h $(BACKEND_PATH)/filesystem.h \
  $(BACKEND_PATH)/partitionmanager.h $(BACKEND_PATH)/Trees.h
BACKEND_CPP=$(BACKEND_PATH)/disk.cpp $(BACKEND_PATH)/diskmanager.cpp $(BACKEND_PATH)/filesystem.cpp \
  $(BACKEND_PATH)/partitionmanager.cpp $(BACKEND_PATH)/Trees.cpp

CLI_PATH=CLI/
CLI_H=$(CLI_PATH)/cli.h $(FS_PATH)/Arboreal_Exceptions.h
CLI_CPP=$(CLI_PATH)/cli.cpp $(CLI_PATH)/cli_driver.cpp $(FS_PATH)/Arboreal_Exceptions.cpp

LIASON_CPP=$(CLI_PATH)/liason.cpp $(FS_PATH)/Arboreal_Exceptions.cpp
LIASON_H=$(FS_PATH)/Arboreal_Exceptions.h

VALGRIND= valgrind --leak-check=yes --log-file=/home/wyatt/ValgrindLogs/%p.log
VALOPS= --leak-check=full --show-leak-kinds=all

all: daemon cli liason driver format

daemon: $(BACKEND_H) $(BACKEND_CPP) $(FS_H) $(FS_CPP)
	$(CXX) $(XXFLAGS) -o daemon $(FS_PATH)/ConnectionDaemon.cpp $(BACKEND_CPP) $(FS_CPP)

driver: $(BACKEND_H) $(BACKEND_CPP) $(FS_H) $(FS_CPP)
	$(CXX) $(XXFLAGS) -o driver $(FS_PATH)/driver.cpp $(BACKEND_CPP) $(FS_CPP)

cli: $(CLI_CPP) $(CLI_H)
	$(CXX) $(XXFLAGS) -o cli $(CLI_CPP)

liason: $(LIASON_H) $(LIASON_CPP)
	$(CXX) $(XXFLAGS) -o liason $(LIASON_CPP)

format: format.cpp
	$(CXX) $(XXFLAGS) -o format format.cpp



##########################################
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

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



DAEMON_PATH=Filesystem
DRIVER_PATH=Filesystem
CLI_PATH=CommandLineInterface
LIAISON_PATH=LiaisonProcess
FORMAT_PATH=FSFormat

FILESYS_PATH=Filesystem/DaemonDependancies/FileSystem
DISK_PATH=Filesystem/DaemonDependancies/Disk
DISKM_PATH=Filesystem/DaemonDependancies/DiskManager
FILE_PATH=Filesystem/DaemonDependancies/File
PM_PATH=Filesystem/DaemonDependancies/PartitionManager
TREES_PATH=Filesystem/DaemonDependancies/Trees
TYPES_PATH=Filesystem/DaemonDependancies/Types

SHARED_CPPATH=SharedCPPFiles
SHARED_HPATH=SharedHeaders

BACKEND_H=$(FILESYS_PATH)/FileSystem.h $(FILE_PATH)/File.h $(DISK_PATH)/Disk.h \
		  $(DISKM_PATH)/DiskManager.h $(PM_PATH)/PartitionManager.h $(TREES_PATH)/Trees.h \
		  $(TYPES_PATH)/types.h
BACKEND_CPP=$(FILESYS_PATH)/FileSystem.cpp $(FILE_PATH)/File.cpp $(DISK_PATH)/Disk.cpp \
		  	$(DISKM_PATH)/DiskManager.cpp $(PM_PATH)/PartitionManager.cpp $(TREES_PATH)/Trees.cpp

CLI_H=$(CLI_PATH)/CLHeaders/Cli.h $(CLI_PATH)/CLDependancies/cli_helper.hpp \
	  $(SHARED_HPATH)/Arboreal_Exceptions.h $(SHARED_HPATH)/ErrorCodes.h \
	  $(SHARED_HPATH)/CommandCodes.h $(SHARED_HPATH)/Print.h $(SHARED_HPATH)/DebugMessages.hpp
CLI_CPP=$(CLI_PATH)/Cli.cpp $(SHARED_CPPATH)/Arboreal_Exceptions.cpp \


LIAISON_H=$(LIAISON_PATH)/LiaisonDependancies/liason_helper.hpp \
	      $(SHARED_HPATH)/Arboreal_Exceptions.h $(SHARED_HPATH)/Print.h \
	      $(SHARED_HPATH)/Parser.h $(SHARED_HPATH)/DebugMessages.hpp
LIAISON_CPP=$(LIAISON_PATH)/liaison.cpp $(SHARED_CPPATH)/Arboreal_Exceptions.cpp \
	        $(SHARED_CPPATH)/Parser.cpp


DAEMON_H=$(DAEMON_PATH)/DaemonHeaders/daemon.h $(SHARED_HPATH)/CommandCodes.h \
		 $(SHARED_HPATH)/Arboreal_Exceptions.h $(BACKEND_H)
DAEMON_CPP=$(DAEMON_PATH)/daemon.cpp $(BACKEND_CPP) $(SHARED_CPPATH)/Arboreal_Exceptions.cpp \
		   $(SHARED_CPPATH)/Parser.cpp

# DRIVER_H=$(BACKEND_H)
# DRIVER_CPP=$(DRIVER_PATH)/driver.cpp $(BACKEND_CPP)


VALGRIND= valgrind --leak-check=yes --log-file=/home/wyatt/ValgrindLogs/%p.log
VALOPS= --leak-check=full --show-leak-kinds=all

RUN_FORMAT= ./format

all: clean commandline liaison format daemon runformat

daemon: $(DAEMON_H) $(DAEMON_PATH)
	$(CXX) $(XXFLAGS) -o daemon $(DAEMON_CPP)

# driver: $(DRIVER_H) $(DRIVER_CPP)
# 	$(CXX) $(XXFLAGS) -o driver $(DRIVER_CPP)

commandline: $(CLI_H) $(CLI_CPP)
	$(CXX) $(XXFLAGS) -o commandline $(CLI_CPP)

liaison: $(LIAISON_H) $(LIAISON_CPP)
	$(CXX) $(XXFLAGS) -o liaison $(LIAISON_CPP)

format: $(FORMAT_PATH)/format.cpp
	$(CXX) $(XXFLAGS) -o format $(FORMAT_PATH)/format.cpp



##########################################
driver.o: driver.cpp
	$(CXX) $(XXFLAGS) -c driver.cpp

disk.o: Disk.cpp
	$(CXX) $(XXFLAGS) -c Disk.cpp

diskmanager.o: DiskManager.cpp
	$(CXX) $(XXFLAGS) -c DiskManager.cpp

partitionmanager.o: PartitionManager.cpp
	$(CXX) $(XXFLAGS) -c PartitionManager.cpp

filesystem.o: FileSystem.cpp
	$(CXX) $(XXFLAGS) -c FileSystem.cpp

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

runformat: $(RUN_FORMAT) 
	$(RUN_FORMAT) diskInfo.d

.PHONY clean:
	/bin/rm -rf *.o driver commandline format daemon DISK1 *.dSYM liaison core*

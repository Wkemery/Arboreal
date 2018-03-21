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
	  $(SHARED_HPATH)/CommandCodes.h $(SHARED_HPATH)/Print.h
CLI_CPP=$(CLI_PATH)/Cli.cpp $(SHARED_CPPATH)/Arboreal_Exceptions.cpp


LIAISON_H=$(LIAISON_PATH)/LiaisonDependancies/liason_helper.hpp \
	      $(SHARED_HPATH)/Arboreal_Exceptions.h $(SHARED_HPATH)/Print.h \
	      $(SHARED_HPATH)/Parser.h $(SHARED_HPATH)/DebugMessages.h
LIAISON_CPP=$(LIAISON_PATH)/liaison.cpp $(SHARED_CPPATH)/Arboreal_Exceptions.cpp \
	        $(SHARED_CPPATH)/Parser.cpp $(SHARED_CPPATH)/DebugMessages.cpp


DAEMON_H=$(DAEMON_PATH)/DaemonHeaders/daemon.h $(SHARED_HPATH)/CommandCodes.h \
		 $(SHARED_HPATH)/Arboreal_Exceptions.h $(BACKEND_H)
DAEMON_CPP=$(DAEMON_PATH)/daemon.cpp $(BACKEND_CPP) $(SHARED_CPPATH)/Arboreal_Exceptions.cpp \
		   $(SHARED_CPPATH)/Parser.cpp

DRIVER_H=$(BACKEND_H)
DRIVER_CPP=$(DRIVER_PATH)/driver.cpp $(BACKEND_CPP)


VALGRIND= valgrind --leak-check=yes --log-file=/home/wyatt/ValgrindLogs/%p.log
VALOPS= --leak-check=full --show-leak-kinds=all

RUN_FORMAT= ./format

all: commandline liaison format daemon runformat

daemon: $(DAEMON_H) $(DAEMON_PATH)
	$(CXX) $(XXFLAGS) -o daemon $(DAEMON_CPP)

driver: $(DRIVER_H) $(DRIVER_CPP)
	$(CXX) $(XXFLAGS) -o driver $(DRIVER_CPP)

commandline: $(CLI_H) $(CLI_CPP)
	$(CXX) $(XXFLAGS) -o commandline $(CLI_CPP)

liaison: $(LIAISON_H) $(LIAISON_CPP)
	$(CXX) $(XXFLAGS) -o liaison $(LIAISON_CPP)

format: $(FORMAT_PATH)/format.cpp
	$(CXX) $(XXFLAGS) -o format $(FORMAT_PATH)/format.cpp

runformat: format
	./format diskInfo.d

.PHONY clean:
	/bin/rm -rf *.o driver commandline format daemon DISK1 *.dSYM liaison core*

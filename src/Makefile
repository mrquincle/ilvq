#!/bin/make

# Common configuration structure used by me... The default.mk file contains macross that makes everything
# compile directly. You can "overwrite" them by using the same macro's with different fields in local.mk
# that you have to create yourself and is not stored in the repository.
-include default.mk
-include local.mk

# We need files to compile :-)
SRC=ILVQ.cpp ILVQ_XSZ.cpp

# One of the possible macros is RUNONPC, when this one is disabled everything that involves plotting,
# debugging info, and other stuff is disabled.
ifeq ($(RUNONPC),true)
SRC+=Plot.cpp DataDecorator.cpp
endif

# Main executable .cpp files can be found in
MAINPATH=../main

# Object files will go to directory
OBJECTPATH=../objects

# Final binaries go to directory
BINPATH=../bin

# Header files
INCPATH=../inc

# Feature: we will automatically take the most recent file in the "main" directory as target
# $touch main/test.cpp if you e.g. want that file to compile
EXE=$(shell cd $(MAINPATH); ls -1ltr *.c* | tail -n 1 | cut -d ':' -f 2 | cut -d ' ' -f 2 | cut -d '.' -f 1)
EXE_EXT=$(shell cd $(MAINPATH); ls -1ltr *.c* | tail -n 1 | cut -d ':' -f 2 | cut -d ' ' -f 2)
SRC+=$(EXE_EXT)

# Default flags
CXXFLAGS = -O2  -Wall
CFLAGS = -O2  -Wall -std=gnu99

# Update path with path to cross-compiler if necessary
PATH:=$(PATH):$(COMPILER_PATH)

# Just use normal compiler, uncomment if you want to cross-compile. This will add a -DRUNONPC flag to
# gcc or g++ which can subsequently be used in the code itself. 
ifeq ($(RUNONPC),true)
TARGET=
CFLAGS += -DRUNONPC
CXXFLAGS += -DRUNONPC
endif

# Definition of the compiler, linker, assembler, etc.
CC = $(COMPILER_PREFIX)gcc
CXX = $(COMPILER_PREFIX)g++ 
AR = $(COMPILER_PREFIX)ar
OBJDUMP = $(COMPILER_PREFIX)objdump
SIZE = $(COMPILER_PREFIX)size
STRIP = $(COMPILER_PREFIX)strip

# Add include directories for the compiler/linker
IPATH = .
IPATH += $(ADDITIONAL_INCLUDE_PATHS)
IPATH += $(INCPATH)

LDFLAGS_ADD := $(foreach lib, $(ADDITIONAL_LIBRARY_PATHS), -L$(lib))
CXXFLAGS += $(patsubst %, -I%, $(IPATH))
LDFLAGS = -rdynamic $(LDFLAGS_ADD) -lpthread 

# Add program specific libraries
ifeq ($(RUNONPC),true)
LDFLAGS += -lplplotcxxd
endif

######################

TOBJECTS = $(SRC:%.cpp=$(OBJECTPATH)/%.o)
OBJECTS = $(TOBJECTS:%.c=$(OBJECTPATH)/%.o)

all: $(OBJECTPATH) $(BINPATH) $(BINPATH)/$(EXE) strip

$(OBJECTPATH):
	mkdir -p $(OBJECTPATH)

$(BINPATH):
	mkdir -p $(BINPATH)

$(BINPATH)/$(EXE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJECTPATH)/%.o:$(MAINPATH)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJECTPATH)/%.o:%.cpp $(INCPATH)/%.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJECTPATH)/%.o:%.c $(INCPATH)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

#$(OBJECTPATH)/%.o:$(INCPATH)%.h
#	#do nothing

objdump:
	$(OBJDUMP) -hS $(BINPATH)/$(EXE) > $(OBJECTPATH)/$(EXE).lst

strip:
	$(STRIP) $(BINPATH)/$(EXE)
	
clean:
	rm -f $(BINPATH)/$(EXE) $(OBJECTS) 
	rmdir --ignore-fail-on-non-empty $(OBJECTPATH) 
	rmdir --ignore-fail-on-non-empty $(BINPATH)


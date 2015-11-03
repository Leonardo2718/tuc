# compiler, tools, and options
CXX			= g++
CXXFLAGS	= -Wall -std=c++14 -iquote../../include
LIBS		= /lib/libboost_unit_test_framework.a

SRCDIR		= ../../src
INCLUDEDIR	= ../../include

SOURCES	= $(SRCDIR)/*
HEADERS	= $(INCLUDEDIR)/*



# make rules

all: tuc_test

tuc_test: tuc_test.cpp Makefile $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) tuc_test.cpp ../../obj/text_entity.o ../../obj/grammar.o $(LIBS) -o "$@"

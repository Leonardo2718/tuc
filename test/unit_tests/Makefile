# compiler, tools, and options
CXX			= g++
CXXFLAGS	= -Wall -std=c++14 -iquote../../include
LIBS		= /lib/libboost_unit_test_framework.a

SRCDIR		= ../../src
INCLUDEDIR	= ../../include

SOURCES	= $(SRCDIR)/*
HEADERS	= $(INCLUDEDIR)/*

TESTFILES	= lexer_tests.cpp parser_tests.cpp tuc_unit_tests.cpp
TUCFILES	= text_entity.cpp grammar.cpp syntax_tree.cpp compiler_exceptions.cpp

TESTOBJS	= $(addprefix obj/,$(subst .cpp,.o,$(TESTFILES)))
TUCOBJS		= $(addprefix obj/__tuc_,$(subst .cpp,.o,$(TUCFILES)))


# make rules

all: lexer_tests parser_tests

#%_tests: %_tests.cpp tuc_unit_tests.hpp Makefile $(SOURCES) $(HEADERS)
#	$(CXX) $(CXXFLAGS) $< ../../src/text_entity.cpp ../../src/grammar.cpp ../../src/syntax_tree.cpp \
#	../../src/compiler_exceptions.cpp $(LIBS) -o "$@"

%_tests: obj/%_tests.o obj/tuc_unit_tests.o tuc_unit_tests.hpp Makefile $(TUCOBJS)
	$(CXX) $(CXXFLAGS) $< $(TUCOBJS) $(LIBS) -o "$@"



complete_test: tuc_unit_tests.hpp Makefile $(TESTOBJS) $(TUCOBJS)
	$(CXX) $(CXXFLAGS) $(TESTOBJS) $(TUCOBJS) $(LIBS) -o "$@"



obj/__tuc_%.o:$(SRCDIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c "$<" -o "$@"

obj/%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c "$<" -o "$@"

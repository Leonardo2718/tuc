# compiler, tools, and options
CXX			= g++
CXXFLAGS	= -Wall -std=c++14 -iquote../../include #-lboost_unit_test_framework
LIBS		= -lboost_unit_test_framework

SRCDIR		= ../../src
INCLUDEDIR	= ../../include

SOURCES	= $(SRCDIR)/*
HEADERS	= $(INCLUDEDIR)/*

TESTFILES	= lexer_tests.cpp parser_tests.cpp tuc_unit_tests.cpp
TUCFILES	= text_entity.cpp grammar.cpp lexer.cpp syntax_tree.cpp parser.cpp compiler_exceptions.cpp node_type.cpp

TESTOBJS	= $(addprefix obj/,$(subst .cpp,.o,$(TESTFILES)))
TUCOBJS		= $(addprefix obj/__tuc_,$(subst .cpp,.o,$(TUCFILES)))



# make rules

complete_test: tuc_unit_tests.hpp Makefile $(TESTOBJS) $(TUCOBJS)
	$(CXX) $(CXXFLAGS) $(TESTOBJS) $(TUCOBJS) $(LIBS) -o "$@"



all: lexer_tests parser_tests

%_tests: obj/%_tests.o obj/tuc_unit_tests.o tuc_unit_tests.hpp Makefile $(TUCOBJS)
	$(CXX) $(CXXFLAGS) -DSTANDALONE $< $(TUCOBJS) obj/tuc_unit_tests.o $(LIBS) -o "$@"



obj/__tuc_%.o:$(SRCDIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c "$<" -o "$@"

obj/%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c "$<" -o "$@"



run: complete_test
	./complete_test

clean:
	rm obj/*

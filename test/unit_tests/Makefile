# compiler, tools, and options
CXX			= g++
CXXFLAGS	= -Wall -std=c++14 -iquote../../include
LIBS		= /lib/libboost_unit_test_framework.a

SRCDIR		= ../../src
INCLUDEDIR	= ../../include



# make rules

all: lexer_test

lexer_test: lexer_test.cpp $(INCLUDEDIR)/grammar.hpp $(SRCDIR)/grammar.cpp $(INCLUDEDIR)/lexer.hpp
	$(CXX) $(CXXFLAGS) lexer_test.cpp $(SRCDIR)/grammar.cpp $(LIBS) -o "$@"

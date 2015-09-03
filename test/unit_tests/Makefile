# compiler, tools, and options
CXX			= g++
CXXFLAGS	= -Wall -std=c++14 -iquote../../include

SRCDIR		= ../../src
INCLUDEDIR	= ../../include



# make rules

all: lexer_test

lexer_test: lexer_test.cpp $(INCLUDEDIR)/grammar.hpp $(SRCDIR)/grammar.cpp $(INCLUDEDIR)/lexer.hpp
	$(CXX) $(CXXFLAGS) lexer_test.cpp $(SRCDIR)/grammar.cpp -o "$@"

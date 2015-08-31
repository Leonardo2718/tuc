# compiler, tools, and options
CXX			= g++
CXXFLAGS	= -Wall -std=c++14 -iquote../include



# make rules

all: lexer_test

lexer_test: lexer_test.cpp ../include/grammar.hpp ../src/grammar.cpp ../include/lexer.hpp
	$(CXX) $(CXXFLAGS) lexer_test.cpp ../src/grammar.cpp -o "$@"

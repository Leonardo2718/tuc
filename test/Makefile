# compiler, tools, and options
CXX			= g++
CXXFLAGS	= -Wall -std=c++14 -iquote../include



# make rules

all: lexer_test

lexer_test: lexer_test.cpp ../src/lexer.cpp ../include/lexer.hpp
	$(CXX) $(CXXFLAGS) lexer_test.cpp ../src/lexer.cpp -o "$@"

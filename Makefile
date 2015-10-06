# compiler, tools, and options
CXX			= g++
CXXFLAGS	= -Wall -std=c++14 -iquoteinclude

# prerequisite files
HEADERS		= include/grammar.hpp include/lexer.hpp include/syntax_tree.hpp include/asm_translator.hpp
SOURCES		= src/tuc.cpp src/grammar.cpp src/syntax_tree.cpp src/asm_translator.cpp
OBJS		= $(subst src,obj,$(subst .cpp,.o,$(SOURCES)))



# make rules

tuc: $(OBJS)
	$(CXX) $(OBJS) -o "$@"

obj/%.o: src/%.cpp $(HEADERS) Makefile
	$(CXX) $(CXXFLAGS) -c "$<" -o "$@"

clean:
	rm $(OBJS)

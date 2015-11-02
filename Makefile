# compiler, tools, and options
CXX			= g++
CXXFLAGS	= -Wall -std=c++14 -iquoteinclude

# prerequisite files
HEADERS		= include/grammar.hpp include/lexer.hpp include/syntax_tree.hpp include/asm_generator.hpp \
	include/symbol_table.hpp include/compiler_exceptions.hpp include/text_entity.hpp
SOURCES		= src/tuc.cpp src/grammar.cpp src/syntax_tree.cpp src/asm_generator.cpp src/symbol_table.cpp \
	src/compiler_exceptions.cpp src/text_entity.cpp
OBJS		= $(subst src,obj,$(subst .cpp,.o,$(SOURCES)))



# make rules

tuc: $(OBJS)
	$(CXX) $(OBJS) -o "$@"

obj/%.o: src/%.cpp $(HEADERS) Makefile
	$(CXX) $(CXXFLAGS) -c "$<" -o "$@"

clean:
	rm $(OBJS)

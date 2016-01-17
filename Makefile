# compiler, tools, and options
CXX			= g++
CXXFLAGS	= -Wall -std=c++14 -iquoteinclude

# prerequisite files
HEADERFILES = grammar.hpp lexer.hpp syntax_tree.hpp parser.hpp asm_generator.hpp symbol_table.hpp \
	compiler_exceptions.hpp text_entity.hpp node_type.hpp u_language.hpp

SOURCEFILES = tuc.cpp grammar.cpp lexer.cpp syntax_tree.cpp parser.cpp asm_generator.cpp symbol_table.cpp \
	compiler_exceptions.cpp text_entity.cpp node_type.cpp

HEADERS		= $(addprefix include/,$(HEADERFILES))
SOURCES		= $(addprefix src/,$(SOURCEFILES))
OBJS		= $(subst src,obj,$(subst .cpp,.o,$(SOURCES)))



# make rules

tuc: $(OBJS)
	$(CXX) $(OBJS) -o "$@"

obj/%.o: src/%.cpp $(HEADERS) Makefile
	$(CXX) $(CXXFLAGS) -c "$<" -o "$@"

clean:
	rm $(OBJS)

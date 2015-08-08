# compiler, tools, and options
CXX			= g++
CXXFLAGS	= -Wall -std=c++14 -iquoteinclude

# prerequisite files
HEADERS		=
SOURCES		= src/toc.cpp
OBJS		= $(subst src,bin,$(subst .cpp,.o,$(SOURCES)))



# make rules

tuc: $(OBJS)
	$(CXX) $(OBJS) -o "$@"

bin/%.o: src/%.cpp $(HEADERS) Makefile
	$(CXX) $(CXXFLAGS) -c "$<" -o "$@"

clean:
	rm $(OBJS)

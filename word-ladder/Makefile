CXX = clang++

CXXSTDFLAGS ?= -std=c++14
DEBUGFLAGS ?= -g
CXXFLAGS := $(ADDITIONAL_FLAGS) -Wall -Wextra $(CXXSTDFLAGS) $(DEBUGFLAGS)

OBJECTS =
ELFS = preprocess query

all : $(ELFS) $(OBJECTS)

clean :
	{ rm $(ELFS) $(OBJECTS) ; true; }

preprocess: preprocess.cpp
query: query.cpp

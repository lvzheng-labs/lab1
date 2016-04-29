CXX = clang++

CXXSTDFLAGS ?= -std=c++11
DEBUGFLAGS ?= -g
CXXFLAGS := $(ADDITIONAL_FLAGS) -Wall -Wextra $(CXXSTDFLAGS) $(DEBUGFLAGS)

OBJECTS =
ELFS = writer

all : $(ELFS) $(OBJECTS)

clean :
	{ rm $(ELFS) $(OBJECTS) ; true; }

writer: writer.cpp

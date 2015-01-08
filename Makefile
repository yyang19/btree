CC=gcc
CXX=g++
RM=rm -rf
CPP_FILES := $(wildcard *.cpp)
OBJS := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

CPPFLAGS=-g -O0 --coverage -Wall
LDFLAGS=-g -O0 --coverage 
LDLIBS=

all: main

main: $(OBJS)
	    $(CXX) $(LDFLAGS) -o main $(OBJS) $(LDLIBS) 

obj/%.o: %.cpp
	   g++ -c $(CPPFLAGS) -o $@ $<
clean:
	    $(RM) $(OBJS)

dist-clean: clean
	    $(RM) main out obj/*

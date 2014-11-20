# Filename: 	Makefile
# Author:	Matt Arriola
# Description:	Makefile for hyphenation problem solution


########################################
# Variables

# C++ compiler
CXX		:= g++

# C++ compiler flags
CXXFLAGS	:= -g -Wall -std=c++11

# Linker
LINK		:= $(CXX)

# Linker flags
LDFLAGS		:=

# Libraries
LDLIBS		:=


########################################
# Rules

hyphens : hyphens.o

all : hyphens

% : %.o
	$(LINK) $(LDFLAGS) $^ -o $@ $(LDLIBS)

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@


########################################

.PHONY : clean
clean:
	$(RM) a.out core hyphens
	$(RM) *.o

.PHONY : tidy
tidy:
	$(RM) *.o

# Makefile
#
# Copyright (c) 2018 Jonathan Lemos
#
# This software may be modified and distributed under the terms
# of the MIT license.  See the LICENSE file for details.

NAME=cloudsync
VERSION=0.1\ beta
SHELL=/bin/sh

CXX:=g++
CXXFLAGS:=-Wall -Wextra -pedantic -std=c++17 -DPROG_NAME=\"$(NAME)\" -DPROG_VERSION=\"$(VERSION)\" -pthread
DBGFLAGS:=-g
RELEASEFLAGS:=-O2
TESTFLAGS:=-lgtest
LDFLAGS:=-lcryptopp -lmega

DIRECTORIES=$(shell find . -type d 2>/dev/null -not -path './os*' -not -path 'git/*' | sed -re 's|^.*\.git.*$$||;s|.*/sdk.*$$||;s|^.*/tests.*$$||' | awk 'NF')
FILES=$(foreach directory,$(DIRECTORIES),$(shell ls $(directory) | egrep '^.*\.cpp$$' | sed -re 's|^.*main.cpp$$||;s|^(.+)\.cpp$$|$(directory)/\1|' | awk 'NF'))
TESTS=$(shell find tests -type f -name '*.cpp' -not -path './tests/test_ext*' 2>/dev/null | sed -re 's|^(.+)\.cpp$$|\1|' | awk 'NF')
FRAMEWORKOBJECTS=tests/test_ext.dbg.o

SOURCEFILES=$(foreach file,$(FILES),$(file).cpp)
OBJECTS=$(foreach file,$(FILES),$(file).o)
DBGOBJECTS=$(foreach file,$(FILES),$(file).dbg.o)
TESTOBJECTS=$(foreach test,$(TESTS),$(test).dbg.o)
TESTEXECS=$(foreach test,$(TESTS),$(test).x)

.PHONY: q
q:
	@echo $(FILES)

release: main.o $(OBJECTS)
	$(CC) -o $(NAME) main.o $(OBJECTS) $(RELEASEFLAGS) $(CXXFLAGS) $(LDFLAGS)

debug: main.dbg.o $(DBGOBJECTS)
	$(CC) -o $(NAME) main.dbg.o $(DBGOBJECTS) $(DBGFLAGS) $(CXXFLAGS) $(LDFLAGS)

.PHONY: framework
framework:
	$(CC) -o tests/test_ext.dbg.o $(DBGFLAGS) $(CXXFLAGS) $(LDFLAGS)

.PHONY: docs
docs:
	doxygen Doxyfile

test: framework $(TESTEXECS) $(TESTOBJECTS)
	@echo "Made all tests"

%.x: %.dbg.o $(DBGOBJECTS)
	$(CXX) -o $@ $< $(DBGOBJECTS) $(FRAMEWORKOBJECTS) $(DBGFLAGS) $(TESTFLAGS) $(CXXFLAGS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(RELEASEFLAGS)

%.dbg.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(DBGFLAGS)

.PHONY: clean
clean:
	rm -f *.o $(NAME) main.c.* vgcore.* $(TESTOBJECTS) $(DBGOBJECTS) $(OBJECTS) $(TESTEXECS) $(FRAMEWORKOBJECTS)
	rm -rf docs

.PHONY: linecount
linecount:
	wc -l Makefile README.md $(foreach dir,$(DIRECTORIES),$(dir)/*.hpp $(dir)/*.cpp) $(foreach test,$(TESTS),$(test).cpp) 2>/dev/null | sort -k2,2

.PHONY: linecount_notests
linecount_notests:
	wc -l Makefile README.md $(foreach dir,$(DIRECTORIES),$(dir)/*.hpp $(dir)/*.cpp) 2>/dev/null | sort -k2,2

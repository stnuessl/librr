#
# The MIT License (MIT)
#
# Copyright (c) <2016> Steffen Nüssle
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

CXX		:= /usr/bin/g++
HDR		:= $(shell find ./src/lib/ -iname *.hpp)
CPPFLAGS	:= -Wall -Werror -Wpedantic
CXXFLAGS	+= -O2 -fno-exceptions
DESTDIR 	:= /usr/local/include/librr
TESTSRC 	:= ./src/test/main.cpp
TESTBIN 	:= ./src/test/main

all: test

install:
	@mkdir -p $(DESTDIR)
	cp $(HDR) $(DESTDIR)
uninstall:
	rm -f $(addprefix $(DESTDIR)/, $(notdir $(HDR)))
clean:
	rm -f $(TESTBIN)
test:
	$(CXX) -std=c++11 $(CPPFLAGS) $(CXXFLAGS) -o $(TESTBIN) $(TESTSRC)
	@$(TESTBIN)
	
.PHONY: test install uninstall

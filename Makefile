#
# Copyright 2011 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
#
# This file is part of the agentXcpp library.
#
# AgentXcpp is free software: you can redistribute it and/or modify
# it under the terms of the AgentXcpp library license, version 1, which 
# consists of the GNU General Public License and some additional 
# permissions.
#
# AgentXcpp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# See the AgentXcpp library license in the LICENSE file of this package 
# for more details.
#

.PHONY: all documentation library clean
CXXFLAGS += -I src/ -l boost_system -l pthread
CXXFLAGS += -g


all: library main

library:
	make -C src/


main : main.cpp library
	$(CXX) $< -o $@ src/agentxcpp.a $(CXXFLAGS)


documentation:
	make -C doc/

clean:
	make -C src/ clean
	make -C doc/ clean

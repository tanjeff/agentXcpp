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

OBJS=oid.o \
     Octet_String.o \
     Integer.o \
     varbind.o \
     Counter32.o \
     Counter64.o \
     Gauge32.o \
     TimeTicks.o \
     Opaque.o \
     IpAddress.o \
     PDU.o \
     OpenPDU.o \
     ClosePDU.o \
     RegisterPDU.o \
     session.o

CXXFLAGS=-g -O0

agentxcpp.a: $(OBJS)
	$(AR) -r $@ $(OBJS)

%.o : %.cpp %.h
	$(CXX) $< -o $@ -c $(CXXFLAGS)



. PHONY: clean

clean:
	rm -f $(OBJS)
	rm -f agentxcpp.a

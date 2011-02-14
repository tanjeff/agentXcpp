#
# Copyright 2011 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
#
# This file is part of agentXcpp.
#
# AgentXcpp is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# AgentXcpp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with AgentXcpp.  If not, see <http://www.gnu.org/licenses/>.
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

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

# Build library and documentation
SConscript(['src/SConstruct',
	    'doc/SConstruct'])

# Build the test program
Program('main.cpp',
	LIBS=['agentxcpp', 'boost_system', 'pthread'],
	LIBPATH='src/',
	CPPPATH='src/')


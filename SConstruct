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

#import os


# Define the install prefix

env = DefaultEnvironment()

# Add installation path's to environment
env['prefix']     = ARGUMENTS.get('prefix', "install-root")
if env['prefix'][0] != '/':
    # We need a "#" to refer to /this/ dir from subsidiary SConscripts
    env['prefix'] = "#" + env['prefix']
env['libdir']     = env['prefix'] + ARGUMENTS.get('libdir', "/lib")
env['docdir']     = env['prefix'] + ARGUMENTS.get('docdir', "/share/doc/agentxcpp")
env['includedir'] = env['prefix'] + ARGUMENTS.get('includedir', "/include")

# Build library, documentation and examples, export the environment
SConscript(['src/SConscript',
	    'doc/SConscript'], 'env')

# Build unit tests with their own environment
SConscript(['unit_tests/SConscript'])


Alias("doc_api", "doc/api")
Alias("doc_internals", "doc/internals")
Alias("agentxcpp", "src")

# What to build by default
Default('agentxcpp', 'doc_api')


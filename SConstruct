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

# Our Environment
env = Environment(tools = ['default', 'doxygen'])

# Define the install prefix
# Add installation path's to environment:
env['prefix']     = ARGUMENTS.get('prefix', "install-root")
if env['prefix'][0] != '/':
    # We need a "#" to refer to /this/ dir from subsidiary SConscripts
    env['prefix'] = "#" + env['prefix']
env['libdir']     = env['prefix'] + ARGUMENTS.get('libdir', "/lib")
env['docdir']     = env['prefix'] + ARGUMENTS.get('docdir', "/share/doc/agentxcpp")
env['includedir'] = env['prefix'] + ARGUMENTS.get('includedir', "/include")

# Build library, documentation and examples, export the environment
env.SConscript(['src/SConscript',
		'doc/SConscript',
	        'unit_tests/SConscript'], 'env')


# Define aliases for some things which can be built (bug in SCons 
# 2.0.1: the Default() function does not work properly, using aliases 
# solves the problem)
Alias("doc_api", "doc/api")
Alias("doc_internals", "doc/internals")
Alias("agentxcpp", "src")
Alias("unit_tests", "unit_tests/testsuite")


# What to build by default
Default('agentxcpp', 'unit_tests')


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


#################################################
## Our Environment
env = DefaultEnvironment()


#################################################
## Command-line magic

# --prefix magic
default_prefix = 'install-root'
AddOption('--prefix', nargs=1, action='store', dest='prefix', type='string',
	  help='installation prefix (default: ' + default_prefix + ')',
	  default=default_prefix)
env['prefix'] = GetOption('prefix')
# Make relative path absolute
if env['prefix'][0] != '/':
    env['prefix'] = GetLaunchDir() + '/' + env['prefix']

# --libdir magic
AddOption('--libdir', nargs=1, action='store', dest='libdir', type='string',
	  help='installation directory for libraries ' +
	  '(default: <PREFIX>/lib)',
	  default=env['prefix'] + '/lib')
env['libdir'] = GetOption('libdir')
# Make relative path absolute
if env['libdir'][0] != '/':
    env['libdir'] = GetLaunchDir() + '/' + env['libdir']

# --docdir magic
AddOption('--docdir', nargs=1, action='store', dest='docdir', type='string',
	  help='installation directory for documentation ' +
	  '(default: <PREFIX>/share/doc/agentxcpp)',
	  default=env['prefix'] + '/share/doc/agentxcpp')
env['docdir'] = GetOption('docdir')
# Make relative path absolute
if env['docdir'][0] != '/':
    env['docdir'] = GetLaunchDir() + "/" + env['docdir']

# --includedir magic
AddOption('--includedir', nargs=1, action='store', dest='includedir', 
	type='string',
	  help='installation directory for header files ' +
	  '(default: <PREFIX>/include)',
	  default=env['prefix'] + '/include')
env['includedir'] = GetOption('includedir')
# Make relative path absolute
if env['includedir'][0] != '/':
    env['includedir'] = GetLaunchDir() + "/" + env['includedir']


#################################################
## Include SCronscripts from subdirectories

# (export env to them):
env.SConscript(['src/SConscript',
		'doc/SConscript',
	        'unit_tests/SConscript'], 'env')


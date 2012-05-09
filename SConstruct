#
# Copyright 2011-2012 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
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

import subprocess
import sys
import os

#################################################
## Our Environment
env = DefaultEnvironment()


#################################################
## Command-line magic

# TODO:
# - check whether '/' works with all systems
# - should the user be allowed to use '#dir' as path name?

# --prefix magic
default_prefix = '#install-root'
AddOption('--prefix', nargs=1, action='store', dest='prefix', type='string',
	  help='installation prefix (default: ' + default_prefix + ')',
	  default=default_prefix)
env['prefix'] = GetOption('prefix')
# Make relative path absolute
if env['prefix'][0] != '/' and env['prefix'][0] != '#':
    env['prefix'] = GetLaunchDir() + '/' + env['prefix']

# --libdir magic
AddOption('--libdir', nargs=1, action='store', dest='libdir', type='string',
	  help='installation directory for libraries ' +
	  '(default: <PREFIX>/lib)',
	  default=env['prefix'] + '/lib')
env['libdir'] = GetOption('libdir')
# Make relative path absolute
if env['libdir'][0] != '/' and env['libdir'][0] != '#':
    env['libdir'] = GetLaunchDir() + '/' + env['libdir']

# --docdir magic
AddOption('--docdir', nargs=1, action='store', dest='docdir', type='string',
	  help='installation directory for documentation ' +
	  '(default: <PREFIX>/share/doc/agentxcpp)',
	  default=env['prefix'] + '/share/doc/agentxcpp')
env['docdir'] = GetOption('docdir')
# Make relative path absolute
if env['docdir'][0] != '/' and env['docdir'][0] != '#':
    env['docdir'] = GetLaunchDir() + "/" + env['docdir']

# --includedir magic
AddOption('--includedir', nargs=1, action='store', dest='includedir', 
	  type='string',
	  help='installation directory for header files ' +
	  '(default: <PREFIX>/include/agentxcpp)',
	  default=env['prefix'] + '/include/agentxcpp')
env['includedir'] = GetOption('includedir')
# Make relative path absolute
if env['includedir'][0] != '/' and env['includedir'][0] != '#':
    env['includedir'] = GetLaunchDir() + "/" + env['includedir']

# --with_libs magic
AddOption('--with_libs', nargs=1, action='store', dest='with_libs', 
	type='string',
	  help='Colon-separated list of directories. For each ' +
          'directory $DIR the $DIR/include is added to the ' +
          'compilers header search path and $DIR/lib is added ' +
          'to the library search path.',
	  default=None)
with_libs = GetOption('with_libs')
if with_libs != None:
    # only if --with-libs was provided:
    with_libs = with_libs.split(':')
    for dir in with_libs:
        # For each DIR:
        # - Make relative path absolute
        # - Fail if DIR/include or DIR/lib is not a directory
        #   otherwise:
        #   - Append DIR/inlcude to CPPPATH
        #   - Append DIR/lib to LIBPATH
        if dir[0] != '/' and dir[0] != '#':
            dir = GetLaunchDir() + "/" + dir
        includepath = dir + "/include"
        libpath = dir + "/lib"
        if not os.path.isdir(includepath):
            print includepath + " is not a directory."
            sys.exit()
        elif not os.path.isdir(libpath):
            print libpath + " is not a directory."
            sys.exit()
        else:
            env.Append(CPPPATH = [includepath])
            env.Append(LIBPATH = [libpath])

#################################################
## Obtain description of current version

# Get current revision
# We ask git for a description of the current revision and add it to the 
# environment.
#
# Note: subprocess.check_output() would be more appropriiate, but doesn't
#       exist in Python 2.4 :-(
proc = subprocess.Popen(["git", "describe", "--always", "--dirty"],
                        stdout=subprocess.PIPE)
(out,err) = proc.communicate()
env['revision'] = out.strip()

#################################################
## Check dependencies

conf = Configure(env)

# Check for boost::asio (header-only lib)
if not conf.CheckHeader('boost/asio.hpp', '<>', 'C++'):
    print """
The boost::asio library is required to build agentXcpp.
Note: For Linux, install a package named libboost-dev (debian/ubuntu) or boost 
      (ArchLinux)."""
    Exit(1)

# Check for boost::bind (header-only lib)
if not conf.CheckHeader('boost/bind.hpp', '<>', 'C++'):
    print """
The boost::bind library is required to build agentXcpp.
Note: For Linux, install a package named libboost-dev (debian/ubuntu) or boost 
      (ArchLinux)."""
    Exit(1)

# Check for boost::smart_ptr (header-only lib)
if not conf.CheckHeader('boost/shared_ptr.hpp', '<>', 'C++'):
    print """
The boost::smart_ptr library is required to build agentXcpp.
Note: For Linux, install a package named libboost-dev (debian/ubuntu) or boost 
      (ArchLinux)."""
    Exit(1)

# Check for boost::test
if not conf.CheckLibWithHeader('boost_unit_test_framework', 
    'boost/test/unit_test.hpp', 'C++', autoadd=0):
    print """
The boost::test library is required to build agentXcpp.
Note: For Linux, install packages named libboost-dev and libboost-test-dev 
      (debian/ubuntu) or boost (ArchLinux)."""
    Exit(1)

env = conf.Finish()


#################################################
## Include SCronscripts from subdirectories

# (export env to them):
env.SConscript(['src/SConscript',
		'doc/SConscript',
	        'unit_tests/SConscript'], 'env')


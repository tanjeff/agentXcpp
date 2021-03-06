#
# Copyright 2011-2016 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>
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
import getversion


#################################################
## Define a Check for executables

def CheckExe(context, executable):
    context.Message("Checking for executable " + executable[0])
    log = open('config.log', 'a')
    log.write('Checking for ' + executable[0] + ':')
    log.write('')
    try:
        result = subprocess.call(executable, stdout=log, stderr=log)
    except:
        # Call failed, maybe the executable is not available.
        # We set result to -1 to indicate that the call failed.
        result = -1
    log.close()
    if result == 0:
        success = True
    else:
        success = False
    context.Result(success)
    return success






#################################################
## Our Environment
env = DefaultEnvironment()

# We need the Qt4 tool
#env['QT4DIR'] = ''
env.Tool('qt4')
env.EnableQt4Modules(['QtCore',
                      'QtNetwork'])

env.Tool('doxygen')



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

# --with-packages magic
# Note: the commandline parameter is --with-packages (with a dash), but the
#       local variable is  with_packages (with underscore) due to python naming
#       requirements.
AddOption('--with-packages', nargs=1, action='store', dest='with-packages', 
	type='string',
	  help='Colon-separated list of directories. For each ' +
          'directory $DIR the $DIR/include is added to the ' +
          'compilers header search path and $DIR/lib is added ' +
          'to the library search path.',
	  default=None)
with_packages = GetOption('with-packages')
if with_packages != None:
    # only if --with-packages was provided:
    with_packages = with_packages.split(':')
    for dir in with_packages:
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

# Add current revision to the environment
env['revision'] = getversion.getVersion()


#################################################
## Check dependencies (except when the user wants help)

if not GetOption('help'):
    conf = Configure(env, custom_tests={'CheckExe' : CheckExe})

    # Check for C++ compiler
    if env['CXX'] == None:
        print """
    Scons didn't find a usable C++ compiler.
    Note: For Linux, install a package named 'build-essential' or 'g++'."""
        Exit(1)

    # Check for doxygen executable
    # Note: we call 'doxygen --version' so no input file is required
    if not conf.CheckExe(['doxygen', '--version']):
        print """
    The doxygen program is required to build agentXcpp's documentation.
    Note: For Linux, install a package named 'doxygen'."""
        Exit(1)

    # Check for dot executable
    # Note: we call 'dot -V' so no input file is required
    if not conf.CheckExe(['dot', '-V']):
        print """
    The dot program is required to build agentXcpp's documentation.
    Note: For Linux, install a package named 'graphviz'."""
        Exit(1)

    env = conf.Finish()


#################################################
## Include SCronscripts from subdirectories

# (export env to them):
env.SConscript(['src/SConscript',
		'doc/SConscript'], 'env')


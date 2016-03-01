About this README
=================

This README gives a short overview of the agentXcpp project, introduces the 
repository's directory structure and describes how to build the documentation. 
Further information can then be found in the documentation.


About agentXcpp
===============

AgentXcpp is a C++ implementation of the AgentX protocol (RFC 2741).

The AgentX protocol is used to implement SNMP MIBs as subagents, which are 
separate processes connecting to a master agent. The master agent in turn 
provides a SNMP interface to the outside world.

You may visit the project's website: [agentxcpp.org](http://agentxcpp.org)


Status of the project
---------------------

All basic functions are implemented.

This version supports SNMP GET and SET requests as well as notifications.
Tables are also supported. Contexts (used by SNMPv3) are not yet supported. 

Read the ChangeLog file to learn more.


Directory Structure
===================


The top level directory contains the following elements:

  * __artwork/__

    Here is the agentXcpp logo, and possibly other artwork.

  * __AUTHORS__

    Contains the author(s) of the agentXcpp library.

  * __ChangeLog__

    The Changelog.

  * __create-release.py__

    A script to create an agentXcpp release.

  * __doc/__

    This directory contains the documentation. The documentation is created 
    using Doxygen and most of the content is contained in the source code.  The 
    doc/ directory contains the doxygen configs, some additional documentation 
    files (*.dox) and the RFC 2741.

  * __getversion.py__

    A script to reliably obtain the version of this agentXcpp copy.

  * __LICENSE__

    Contains the AgentXcpp library LICENSE, which consists of the GPL and an 
    additional linking exception.

  * __GPLv3__

    The GNU GENERAL PUBLIC LICENSE, Version 3.

  * __README__

    This file.

  * __SConstruct__

    AgentXcpp is built using SCons. This file defines how to build the library 
    and the documentation.

  * __site\_scons/__

    This directory contains SCons supplements. The software contained in this 
    directory is published under another license than agentXcpp, as it is not 
    part of the agentXcpp software.  Distribution is done for convenience.


  * __src/__

    This directory contains the source code of the library.



Building the Documentation
==========================


Prerequisites
-------------

To build agentXcpp, the following is needed:

- Standard build environment (compiler, linker, etc.)
- The QT4 library (devel package, e.g.  libqt4-dev on linux systems)
- Doxygen, to build the documentation
- The `dot` tool from the graphviz package, which is used by doxygen to 
  generate diagrams
- SCons, to control the build process


Building
--------

There are two types of HTML documentation: The "API documentation" and the 
"internals documentation".  The API documentation describes the AgentXcpp API 
and is intended for the user of the library. The internals documentation is the 
complete reference of the library, describing all internals.

You can build the documentation using SCons or without SCons.

 1. With SCons:

    cd to the top level directory (the one containing this README) and type

        scons

    This builds the library as well as both documentations.

 1. Without SCons:
    cd into the `doc/` directory and type

        doxygen api.doxyfile

    respectively

        doxygen internals.doxyfile

    These commands build only the corresponding documentation.

After building, the documentation can be displayed by pointing a web browser to 
`doc/api/html/index.html` respectively `doc/internals/html/index.html`.


License
=======

Copyright 2011-2016 Tanjeff-Nicolai Moos <tanjeff@cccmz.de>

This file is part of the agentXcpp library.

AgentXcpp is free software: you can redistribute it and/or modify
it under the terms of the AgentXcpp library license, version 1, which 
consists of the GNU General Public License and some additional 
permissions.

AgentXcpp is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

See the AgentXcpp library license in the LICENSE file of this package 
for more details.

IMPORTANT: The software contained in the site\_scons directory is distributed 
under its own licence. See the files there for details.


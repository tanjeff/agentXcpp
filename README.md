![agentXcpp Logo](artwork/github_logo.png)

About this README
=================

This README gives a short overview of the agentXcpp project and describes how 
to build its documentation from source. It also introduces this repository's 
directory structure.

About agentXcpp
===============

What is agentXcpp ?
-------------------

AgentXcpp is a C++ implementation of the AgentX protocol (RFC 2741). The AgentX 
protocol is used to implement SNMP MIBs as subagents, which are separate 
processes connecting to a master agent. The master agent in turn provides an 
SNMP interface to the outside world.

You may visit the project's website: [agentxcpp.org](http://agentxcpp.org)


Status of the project
---------------------

All basic functions are implemented.

This version supports SNMP GET and SET requests as well as notifications.
Tables are also supported. Contexts (used by SNMPv3) are not yet supported. 

Read the ChangeLog file to learn more.

License
-------

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

**IMPORTANT:** The software contained in the `site\_scons` directory is 
distributed under its own license. See the files there for details.


The Documentation
=================


There are two types of documentation: The "API documentation" and the 
"internals documentation".

The API documentation describes the AgentXcpp API and is intended for the user 
of the library. It contains not only the complete API reference, but also a 
tutorial to get started.

The internals documentation explains all internals of the library, including 
descriptions of the used concepts, how the build system is configured and how 
releases should be created. Of course, it contains also the reference of the 
whole code.

The documentation is [published on the AgentXcpp 
Homepage](http://agentxcpp.org), but it can also be build from source, which is 
described in the following.




Building the Documentation
--------------------------

### Prerequisites

To build agentXcpp, the following is needed:

- Standard build environment (compiler, linker, etc.)
- The QT4 library (devel package, e.g.  libqt4-dev on linux systems)
- Doxygen, to build the documentation
- The `dot` tool from the graphviz package, which is used by doxygen to 
  generate diagrams
- SCons, to control the build process
You can build the documentation using SCons or without SCons.

### Build Commands

 1. With SCons:
    cd into the `doc/` directory and type

        scons

    This builds both documentations (API and internal).

 1. Without SCons:
    cd into the `doc/` directory and type

        doxygen api.doxyfile

    respectively

        doxygen internals.doxyfile

After building, the documentation can be displayed by pointing a web browser to 
`doc/api/html/index.html` respectively `doc/internals/html/index.html`.



Directory Structure of this Repository
======================================


This top level directory contains the following elements:

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





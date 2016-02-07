#!/usr/bin/env python

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
import os

# This function returns a string describing the current
# source tree version.
#
# First, it tries 'git describe'. If that fails, it tries to
# read the 'version' file located in the same directory
# as this script. If that also fails, it returns the string
# "unknown".
def getVersion():
    # 1) Try to get a version string from git
    try:
        # Note: subprocess.check_output() would maybe be more appropriate, but doesn't
        #       exist in Python 2.4
        proc = subprocess.Popen(["git", "describe", "--always", "--dirty", "--tags"],
                stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        (out,err) = proc.communicate()
        if proc.returncode == 0:
            # only if call succeeded:
            return out.strip()
    except:
        # git describe failed - maybe we are outside a git repo.
        # -> continue execution
        pass

    # 2) If 1. failed: Try 'version' file
    scriptDir = os.path.dirname(os.path.realpath(__file__))
    versionFilePath = os.path.join(scriptDir, "version")
    try:
        with open(versionFilePath) as versionFile:
            version = versionFile.readline()
            return version.strip()
    except:
        # Failed.
        pass

    # 3) If 1. and 2. failed, we don't know the version
    return "unknown"

# If this script is called directy (i.e. it was not imported
# by another script): print version string.
if __name__ == "__main__":
    print(getVersion())

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
import gzip
import sys
import shutil
import tarfile
import zipfile
import os
import getversion

# Note: This script uses subprocess.Popen() in favor of subprocess.check_output(),
#       because the latter is not available in Python 2.4.


###################
# Some information
version = getversion.getVersion()
scriptDir = os.path.dirname(os.path.realpath(__file__))
delivDir = os.path.join(scriptDir, "deliv")
versionFileName = os.path.join(scriptDir, 'version')
prefix = 'agentxcpp-' + version + '/'


###################
# Check if the repo is clean.
# We consider the repo clean if no tracked files were changed
# and no files are in the git staging area. Ignored and untracked
# files are allowed.
clean = True
try:
    proc = subprocess.Popen(["git", "status", "--porcelain"],
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    (out,err) = proc.communicate()
    for line in out.split('\n'):
        if line.startswith('??') or line == "":
            continue # ignore untracked files
        clean = False
    if clean == False:
        print("Your repository is dirty. Use 'git status'.")
except:
    print("Can't determine repository state. Are you in a git repo?")
    clean = False
if clean == False:
    # No delivery if repo is dirty
    sys.exit(-1)


###################
# Remove and re-create delivery directory
if os.path.exists(delivDir):
    print("Deleting " + os.path.relpath(delivDir) + "/")
    shutil.rmtree(delivDir)
os.mkdir(delivDir)


###################
# Create version file
with open(versionFileName, 'w') as versionFile:
    versionFile.write(version)
    versionFile.write('\n')

###################
# Create tar file
print("Creating tar file...")
tarName = os.path.join(delivDir, 'agentxcpp-' + version + '.tar')
tgzName = os.path.join(delivDir, 'agentxcpp-' + version + '.tgz')
proc = subprocess.Popen(["git", "archive",
                         "--format=tar",
                         '--prefix=' + prefix,
                         version,
                         '-o', tarName],
                        stdout=subprocess.PIPE)
(out,err) = proc.communicate()

# Add version file to tarfile
with tarfile.open(tarName, "a") as tar:
    tar.add(versionFileName, prefix + os.path.basename(versionFileName))

# Compress tar file, removing the uncompressed file
with open(tarName, 'rb') as f_in, gzip.open(tgzName, 'wb') as f_out:
    shutil.copyfileobj(f_in, f_out)
os.remove(tarName)


###################
# Create ZIP file
print("Creating zip file...")
zipName = os.path.join(delivDir,'agentxcpp-' + version + '.zip')
proc = subprocess.Popen(["git", "archive",
                         "--format=zip",
                         '--prefix=' + prefix,
                         version,
                         '-o', zipName],
                        stdout=subprocess.PIPE)
(out,err) = proc.communicate()

# Add version file to zipfile
with zipfile.ZipFile(zipName, "a") as f:
    f.write(versionFileName, prefix + os.path.basename(versionFileName))


###################
# Coda

# Remove version file
os.remove(versionFileName)

print("")
print("Created delivery for version " + version)
print("The files to deliver are now located in " + os.path.relpath(delivDir) + '/\n')
 

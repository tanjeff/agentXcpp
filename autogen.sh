#!/bin/sh
# Generate world

set -e

aclocal && \
            autoheader &&  \
            autoconf && \
            libtoolize --automake -c && \
            automake --add-missing --copy

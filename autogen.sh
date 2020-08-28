#!/bin/bash

# Bail on error
set -e

# Clean up
echo ">>> cleaning up..." 1>&2
if test -d mtree-[0-9].[0-9].[0-9]; then
    chmod -R u+w mtree-[0-9].[0-9].[0-9]
fi
find . -type d -name .deps -print0 | xargs -0 rm -fr
rm -rf  \
    aclocal.m4 \
    autom4te.cache \
    config/* \
    config.guess \
    config.log \
    config.status \
    config.sub \
    configure \
    mtree \
    mtree-[0-9].[0-9].[0-9] \
    mtree-[0-9].[0-9].[0-9].tar.gz \
    Makefile \
    Makefile.in \
    config.h* \
    stamp-h1 \
    *.o

if [ "$1" = '-n' ]; then
    exit 0
fi

# Regnenerate
echo ">>> applying autotools..." 1>&2
autoreconf -vfi -I .

if [ "$1" != '-c' ]; then
    exit 0
fi

# Reconfigure
echo ">>> running ./configure script..." 1>&2
./configure

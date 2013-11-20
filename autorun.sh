#!/bin/sh

die() { echo "$@"; exit 1; }

aclocal || die "Can't execute aclocal" 

libtoolize --automake --force || die "Can't execute libtoolize"

autoconf || die "Can't execute autoconf"
automake --add-missing --copy --force || die "Can't execute automake"

./configure

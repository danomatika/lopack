#!/bin/sh
#
# generate autotools files
#

PATH=/sw/bin:$PATH

PWD=${0%/*}

case `uname -s` in
    MINGW*)
        # autoreconf doesn't always work on MinGW
        aclocal --force -I m4 && \
        libtoolize --install --force && \
        autoconf --force && \
        automake --add-missing --copy --force-missing && \
        true
    ;;
    *)
        automake --add-missing
        autoreconf --install --force --verbose
    ;;
esac

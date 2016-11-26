#! /bin/bash
#
# generate autotools files
#

mkdir -p config m4
autoreconf --install --force --verbose

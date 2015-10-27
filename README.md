lopack
======

an oscpack-inspired C++ wrapper for the liblo OSC library

Copyright (c) [Dan Wilcox](http://danomatika.com) 2009, 2015

Description
-----------

This is an [oscpack](http://www.rossbencina.com/code/oscpack)-style wrapper for the [liblo](http://liblo.sourceforge.net/) Open Sound Control (OSC) library. You should be able to use this as a drop in replacement in most cases.

Features include:

* OscReceiver server class
* OscSender class w/ C++ stream based interface for building & sending messages
* OscObject class for subclasses based message handling, nestable within address spaces
* ReceivedMessage class with smart message parsing and argument introspection
* support for all argument types used by liblo (and as defined by the official OSC spec)

Documentation
-------------

See the headers in `src/lopack` and the example in `src/lptest` for more info.

Building & Installing
---------------------

You will need liblo installed before building. On Mac OSX, you can use [Homebrew](http://brew.sh) with:

    brew install liblo

This is an automake project, so build the lib & test with:

    ./configure
	make

Run the test program with:

    cd src/lptest
    ./lptest

or

    src/lptest/lptest

Developing
----------

A Premake4 script and IDE files can be found in the prj folder. Premake4 can generate the IDE files from a given lua script. Download Premake4 from <http://industriousone.com/premake>.

Make sure the externals are built by calling the `prj/setupbuild` script which runs configure and calls make in the externals dir.

You can enable a debug build using:

    ./configure --enable-debug

I develop using an IDE, then update the autotools files when the sources are finished. I run `make distcheck` to make sure the distributable package can be built successfully.

Notes
-----

_Note: lopack was originally named "oscframework" in versions prior to 0.2.0_

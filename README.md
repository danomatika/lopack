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
* support for sending & receiving multicast messages

Documentation
-------------

See the headers in `src/lopack` and the example in `src/lptest` for more info.

Building & Installing
---------------------

Download a release tarball from <http://docs.danomatika.com/releases/lopack/>.

Otherwise, if cloning this repo, you will also need to run `autogen.sh` to create the configure script:

    git clone https://github.com/danomatika/lopack.git
    cd lopack
    ./autogen.sh

You will need liblo installed before building. On macOS, you can use [Homebrew](http://brew.sh) with:

    brew install liblo

On Ubuntu/Debian, you would do the following:

    sudo apt-get install liblo-dev

This is an automake project, so build the lib & test with:

    ./configure
	make

Run the test program with:

    cd src/lptest
    ./lptest

or

    src/lptest/lptest

Install via:

    sudo make install

Developing
----------

A Premake4 script and IDE files can be found in the prj folder. Premake4 can generate IDE files from a given lua script. Download Premake4 from <http://industriousone.com/premake>.

You can enable a debug build using:

    ./configure --enable-debug

I develop using an IDE, then update the autotools files when the sources are finished. I run `make distcheck` to make sure the distributable package can be built successfully.

Notes
-----

_Note: lopack was originally named "oscframework" in versions prior to 0.2.0_

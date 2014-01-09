oscframework
============

Copyright (c) [Dan Wilcox]("http://danomatika.com") 2009,2010

This is an [oscpack](http://www.rossbencina.com/code/oscpack)-style wrapper for [liblo](http://liblo.sourceforge.net/). You should be able to use this as a drop in replacement in most cases.

See the headers in src/oscframework and the example in src/tests for more info.

You will need liblo installed before building. On Mac OSX, you can use [Homebrew](http://brew.sh) with:

    brew install liblo

This is an automake project, so build the lib & test with:

    ./configure
	make

Run the test with:

    cd src/tests
    ./oftests

or

    src/tests/oftests


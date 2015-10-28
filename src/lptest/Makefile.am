# lopack test program

# programs to build, don't install
noinst_PROGRAMS = lptest

# bin sources, headers here because we dont want to install them
lptest_SOURCES = main.cpp TestReceiver.h

# include paths
lptest_CPPFLAGS = $(CXXFLAGS) $(LO_CFLAGS) -I$(top_srcdir)/src

# libs to link, set static to statically link local libtool lib
lptest_LDFLAGS = $(LO_LIBS) -static

# local libraries needed to build (builddir), set path to .la for libtool libs
lptest_LDADD = $(top_builddir)/src/lopack/liblopack.la

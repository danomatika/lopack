# lopack test program

# programs to build, don't install
noinst_PROGRAMS = lptest

# bin sources, headers here because we dont want to install them
lptest_SOURCES = main.cpp TestReceiver.h

# local libraries needed to build (builddir)
lptest_LDADD = -L../lopack -llopack

# include paths
lptest_CPPFLAGS = $(CXXFLAGS) $(LO_CFLAGS) -I$(top_srcdir)/src

# libs to link
lptest_LDFLAGS = $(LO_LIBS)

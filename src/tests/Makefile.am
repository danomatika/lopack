# oscframework tests

# programs to build
noinst_PROGRAMS = oftests

# bin sources, headers here because we dont want to install them
oftests_SOURCES = main.cpp TestReceiver.h

# local libraries needed to build (builddir)
oftests_LDADD = -L../oscframework -loscframework

# include paths
oftests_CPPFLAGS = $(CXXFLAGS) \
		$(LO_CFLAGS) -I$(top_srcdir)/src

# libs to link
oftests_LDFLAGS = $(LO_LIBS)

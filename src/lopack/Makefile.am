# lopack library

# the library to build
lib_LTLIBRARIES = liblopack.la

# lib headers to install
otherincludedir = $(includedir)/$(PACKAGE)
otherinclude_HEADERS = lopack.h \
                       OscReceiver.h \
                       OscObject.h \
                       OscSender.h \
                       OscTypes.h

# libs sources, headers listed here will not be installed
liblopack_la_SOURCES = Log.h \
                       OscReceiver.cpp \
                       OscObject.cpp \
                       OscSender.cpp \
                       OscTypes.cpp

# include paths
AM_CPPFLAGS = $(LO_CFLAGS)

# libs to link
AM_LDFLAGS = $(LO_LIBS)

# make sure to remove include folder
uninstall-hook:
	if test -d ${includedir}/${PACKAGE}; then rmdir ${includedir}/${PACKAGE}; fi

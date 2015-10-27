# oscframework

# the library to build
lib_LTLIBRARIES = liboscframework.la

# lib headers to install
otherincludedir = $(includedir)/$(PACKAGE)
otherinclude_HEADERS = \
		oscframework.h \
		OscReceiver.h \
		OscObject.h \
		OscSender.h \
		OscTypes.h

# libs sources
liboscframework_la_SOURCES = \
		Log.h \
		OscReceiver.cpp \
		OscObject.cpp \
		OscSender.cpp \
		OscTypes.cpp

# include paths
AM_CPPFLAGS = $(CXXFLAGS) $(LO_CFLAGS)

# libs to link
AM_LDFLAGS = $(LO_LIBS)

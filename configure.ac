#########################################
##### Prelude #####

AC_INIT([lopack], [0.3.0], [danomatika@gmail.com])
AC_CONFIG_SRCDIR([src/lopack/lopack.h])
AC_CONFIG_AUX_DIR([./config])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

#########################################
##### Build Options #####

DEBUG_CPPFLAGS="-g -Wall -Werror -Wno-uninitialized -O0 -fvisibility=hidden"
RELEASE_CPPFLAGS="-g -O2"

#########################################
##### Check for programs/libs #####

# check progs
AC_PROG_CXX
AM_PROG_AR # allow non-POSIX libs

# libtool
AC_PROG_LIBTOOL
LT_INIT
AC_SUBST([LIBTOOL_DEPS])

# using c++ compiler and linker
AC_LANG_CPLUSPLUS

# check for headers
AC_HEADER_STDC
	
# check for headers & libs
PKG_CHECK_MODULES(LO, liblo >= 0.23, [],
	AC_MSG_ERROR([lo library >= 0.23 not found]))

#########################################
##### Options #####

# debug build switch
# from http://www.bioinf.uni-freiburg.de/~mmann/HowTo/automake.html#debug
AC_MSG_CHECKING([whether to enable the debug build])
AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug],
		[enable debug build [default=no]])],
	[enable_debug="$enableval"],
	[enable_debug=no])
AC_MSG_RESULT([$enable_debug])

if test x"$enable_debug" = x"yes"; then
	AC_DEFINE([DEBUG], [], [Debug Mode])
	CPPFLAGS="$DEBUG_CPPFLAGS $CPPFLAGS"
else
	AC_DEFINE([NDEBUG], [], [Release Mode])
	CPPFLAGS="$RELEASE_CPPFLAGS $CPPFLAGS"
fi

#########################################
##### Externals #####

# externals with configure scripts
AC_CONFIG_SUBDIRS([])

#########################################
##### Output files #####

AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/lopack/Makefile
	src/lptest/Makefile
])
AC_OUTPUT

#########################################
##### Print a summary for the user #####

AC_MSG_RESULT([
	$PACKAGE $VERSION is now configured

	Source directory:     $srcdir
	Installation prefix:  $prefix
	C++ compiler:         $CXX $CPPFLAGS
	Static lib:           $enable_static
	Shared lib:           $enable_shared
	Debug build:          $enable_debug
])

#########################################
##### Prelude #####

AC_INIT([lopack], [0.3.2], [danomatika@gmail.com])
AC_CONFIG_SRCDIR([src/lopack/lopack.h])
AC_CONFIG_AUX_DIR([./config])
AM_INIT_AUTOMAKE([foreign])

#########################################
##### Build settings #####

DEBUG_CXXFLAGS="-O0 -Wall -Werror -Wno-uninitialized -fvisibility=hidden"

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
##### Build options #####

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
	CXXFLAGS="$CXXFLAGS $DEBUG_CXXFLAGS"
else
	AC_DEFINE([NDEBUG], [], [Release Mode])
fi

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
	C++ compiler:         $CXX $CXXFLAGS $LDFLAGS
	Static lib:           $enable_static
	Shared lib:           $enable_shared
	Debug build:          $enable_debug
])

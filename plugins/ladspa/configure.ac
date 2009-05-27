#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([ladspa-bs2b], [0.2], [sebastian@pipping.org])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.10.1 -Wall foreign subdir-objects
                  dist-zip dist-bzip2 dist-lzma])
AC_CONFIG_SRCDIR([src/plugin.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_DISABLE_STATIC  # Must come before AC_PROG_LIBTOOL
AC_PROG_LIBTOOL


AC_ARG_WITH(effect-level, [
===========================================================================
Extra options:
AS_HELP_STRING([--with-effect-level=VAL], [Override default effect level])
AS_HELP_STRING([], [(one of the BS2B_*LEVEL constants from bs2b.h)])
===========================================================================], [
	if test "x${withval}" = "x"; then
		AC_MSG_ERROR([No value given for option --with-effect-level])
	fi
	LB_EFFECT_LEVEL=${withval}
], [
	LB_EFFECT_LEVEL=BS2B_DEFAULT_CLEVEL
])
AC_DEFINE_UNQUOTED(LB_EFFECT_LEVEL, ${LB_EFFECT_LEVEL}, [Effect level to apply])


# Checks for libraries.
BS2B_REQUIRED=3.0.0
PKG_CHECK_MODULES([BS2B], [
	libbs2b >= ${BS2B_REQUIRED}
], [], [
	AC_MSG_ERROR(Please install libbs2b ${BS2B_REQUIRED} or later.)
])

# Checks for header files.
AC_CHECK_HEADERS([ladspa.h])

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strdup])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([ladspa-bs2b], [0.1], [sebastian@pipping.org])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.10.1 -Wall foreign subdir-objects
                  dist-zip dist-bzip2 dist-lzma])
AC_CONFIG_SRCDIR([src/plugin.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_DISABLE_STATIC  # Must come before AC_PROG_LIBTOOL
AC_PROG_LIBTOOL

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

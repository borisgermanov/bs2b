#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([audacious-bs2b], [0.9.0], [sebastian@pipping.org])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.10.1 -Wall foreign subdir-objects
                  dist-zip dist-bzip2 dist-lzma])
AC_CONFIG_SRCDIR([src/plugin.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_DISABLE_STATIC  # Must come before AC_PROG_LIBTOOL
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([AUDACIOUS], [
	audacious >= 1.5.1
	glib-2.0 >= 2.14
	gtk+-2.0 >= 2.10
], [], [
	AC_MSG_ERROR(Please install Audacious 1.5.1 or later.)
])
AUDACIOUS_EFFECT_PLUGIN_DIR=`pkg-config --variable=effect_plugin_dir audacious | sed 's|/$||'`
AC_SUBST([AUDACIOUS_EFFECT_PLUGIN_DIR])


# Checks for libraries.
PKG_CHECK_MODULES([BS2B], [
	libbs2b >= 2.2.1
], [], [
	AC_MSG_ERROR(Please install libbs2b 2.2.1 or later.)
])


AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

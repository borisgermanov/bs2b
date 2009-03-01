#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([xmms-bs2b], [0.4.0], [sebastian@pipping.org])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.10.1 -Wall foreign subdir-objects
                  dist-zip dist-bzip2 dist-lzma])
AC_CONFIG_SRCDIR([src/plugin.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AM_DISABLE_STATIC
PKG_PROG_PKG_CONFIG


# Add parameters
XMMS_BS2B_ANY_INC_PATH_SPECIFIC=0
AC_ARG_WITH(glibconfig, [
Header locations:
  --with-glibconfig=DIR   add path of glibconfig.h (glib 1.x) to CPPFLAGS], [
	if test "x${withval}" = "x"; then
		AC_MSG_ERROR([No path given for option --with-glibconfig])
	fi
	CPPFLAGS="-I${withval} ${CPPFLAGS}"
	XMMS_BS2B_ANY_INC_PATH_SPECIFIC=1
], [])

AC_ARG_WITH(glib, [  --with-glib=DIR         add path of glib.h (glib 1.x) to CPPFLAGS], [
	if test "x${withval}" = "x"; then
		AC_MSG_ERROR([No path given for option --with-glib])
	fi
	CPPFLAGS="-I${withval} ${CPPFLAGS}"
	XMMS_BS2B_ANY_INC_PATH_SPECIFIC=1
], [])

AC_ARG_WITH(plugin, [  --with-plugin=DIR       add path of plugin.h (XMMS 1.x) to CPPFLAGS], [
	if test "x${withval}" = "x"; then
		AC_MSG_ERROR([No path given for option --with-plugin])
	fi
	CPPFLAGS="-I${withval} ${CPPFLAGS}"
	XMMS_BS2B_ANY_INC_PATH_SPECIFIC=1
], [])

AC_ARG_WITH(math, [  --with-math=DIR         add path of math.h to CPPFLAGS], [
	if test "x${withval}" = "x"; then
		AC_MSG_ERROR([No path given for option --with-math])
	fi
	CPPFLAGS="-I${withval} ${CPPFLAGS}"
	XMMS_BS2B_ANY_INC_PATH_SPECIFIC=1
], [])

AC_ARG_WITH(math, [  --with-bs2b=DIR         add path of bs2b.h to CPPFLAGS], [
	if test "x${withval}" = "x"; then
		AC_MSG_ERROR([No path given for option --with-bs2b])
	fi
	CPPFLAGS="-I${withval} ${CPPFLAGS}"
	XMMS_BS2B_ANY_INC_PATH_SPECIFIC=1
], [])


# Adjust prefix default, overridable with --prefix
if test "x$prefix" = "xNONE"; then
    # XMMS doesn't look in /usr/local
    prefix=/usr
fi
libdir='${prefix}/lib/xmms/Effect'
AC_MSG_NOTICE([
  Plugin will be installed to "$libdir" with \${prefix} as "${prefix}".])


# Checks for libraries.
PKG_CHECK_EXISTS([libbs2b], [], [
    AC_MSG_ERROR(Please install libbs2b.)
])


# Default header location fallback
if test "x${XMMS_BS2B_ANY_INC_PATH_SPECIFIC}" = "x0"; then
    AC_MSG_NOTICE([
  Assuming Debian defaults for header locations.
  Run './configure --help' for details.])
    CPPFLAGS="\
-I/usr/lib/glib/include/ \
-I/usr/include/glib-1.2/ \
-I/usr/include/xmms/ \
-I/usr/include/bs2b \
-I/usr/include/ \
$CPPFLAGS"
fi

AC_MSG_NOTICE([Further checks and compilation will use \$CPPFLAGS as
  "$CPPFLAGS"])


# Checks for header files.
AC_CHECK_HEADERS(math.h,,
AC_MSG_ERROR(Header 'math.h' not found. You can use --with-math=DIR to specify its location.))

AC_CHECK_HEADER(glibconfig.h,,
AC_MSG_ERROR(Header 'glibconfig.h' (glib 1.x) not found. You can use --with-glibconfig=DIR to specify its location.))

AC_CHECK_HEADER(glib.h,,
AC_MSG_ERROR(Header 'glib.h' (glib 1.x) not found. You can use --with-glib=DIR to specify its location.),
[#ifdef HAVE_GLIBCONFIG_H
# include <glibconfig.h>
#endif])

AC_CHECK_HEADERS(plugin.h,,
AC_MSG_ERROR(Header 'plugin.h' (XMMS 1.x) not found. You can use --with-plugin=DIR to specify its location.),
[#ifdef HAVE_GLIB_H
# include <glib.h>
#endif])


# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

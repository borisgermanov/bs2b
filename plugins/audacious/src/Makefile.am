lib_LTLIBRARIES = bs2b.la

libdir = @AUDACIOUS_EFFECT_PLUGIN_DIR@

AM_CFLAGS = @AUDACIOUS_CFLAGS@ @BS2B_CFLAGS@

bs2b_la_LDFLAGS = @AUDACIOUS_LIBS@ @BS2B_LIBS@ \
	-module -avoid-version -nostdlib

bs2b_la_SOURCES = \
	plugin.c

BUILT_SOURCES = case.c.inc

CLEANFILES = $(BUILT_SOURCES)

nodist_bs2b_la_SOURCES = $(BUILT_SOURCES)

EXTRA_DIST = \
	case.txt \
	case.awk \
	case.sh

case.c.inc: ${srcdir}/case.txt ${srcdir}/case.awk ${srcdir}/case.sh
	INPUT_DIR="${srcdir}/" OUTPUT_DIR="${builddir}/" "${srcdir}/case.sh"

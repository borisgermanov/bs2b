lib_LTLIBRARIES = bs2b.la

libdir = $(prefix)@AUDACIOUS_EFFECT_PLUGIN_DIR@

AM_CFLAGS = @AUDACIOUS_CFLAGS@ @BS2B_CFLAGS@

bs2b_la_LDFLAGS = @AUDACIOUS_LIBS@ @BS2B_LIBS@ \
	-module -avoid-version -nostdlib

bs2b_la_SOURCES = \
	plugin.c

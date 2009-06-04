ladspa_LTLIBRARIES = bs2b.la

ladspadir = $(libdir)/ladspa

AM_CFLAGS = @BS2B_CFLAGS@

# 5.2.1. Special functions _init and _fini
# http://tldp.org/HOWTO/Program-Library-HOWTO/miscellaneous.html#INIT-AND-FINI-OBSOLETE
bs2b_la_LDFLAGS = @BS2B_LIBS@ \
	-module -Wc,-nostartfiles -avoid-version

bs2b_la_SOURCES = \
	plugin.c

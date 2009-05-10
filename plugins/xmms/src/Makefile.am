lib_LTLIBRARIES = libxmmsbs2b.la

libxmmsbs2b_la_LDFLAGS = -module -avoid-version -nostdlib -lbs2b

libxmmsbs2b_la_SOURCES = \
	plugin.c

BUILT_SOURCES = case.c

CLEANFILES = $(BUILT_SOURCES)

nodist_libxmmsbs2b_la_SOURCES = $(BUILT_SOURCES)

EXTRA_DIST = \
	case.txt \
	case.awk \
	case.sh

case.c: ${srcdir}/case.txt ${srcdir}/case.awk ${srcdir}/case.sh
	INPUT_DIR="${srcdir}/" OUTPUT_DIR="${builddir}/" "${srcdir}/case.sh"


AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
AM_LDFLAGS = $(CODE_COVERAGE_LDFLAGS)

noinst_LTLIBRARIES = libport.la

# libport specific enum types
PORT_ENUMS = \
	$(srcdir)/mm-port.h \
	$(srcdir)/mm-port-serial-at.h

mm-port-enums-types.h: Makefile.am $(PORT_ENUMS) $(top_srcdir)/build-aux/mm-enums-template.h
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--fhead "#include \"mm-port.h\"\n#include \"mm-port-serial-at.h\"\n#ifndef __MM_PORT_ENUMS_TYPES_H__\n#define __MM_PORT_ENUMS_TYPES_H__\n" \
		--template $(top_srcdir)/build-aux/mm-enums-template.h \
		--ftail "#endif /* __MM_PORT_ENUMS_TYPES_H__ */\n" \
		$(PORT_ENUMS) > $@

mm-port-enums-types.c: Makefile.am $(top_srcdir)/build-aux/mm-enums-template.c mm-port-enums-types.h
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--fhead "#include \"mm-port-enums-types.h\"" \
		--template $(top_srcdir)/build-aux/mm-enums-template.c \
		$(PORT_ENUMS) > $@

libport_la_CPPFLAGS = \
	$(MM_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src/helpers \
	-I$(top_builddir)/src/helpers \
	-I$(top_srcdir)/libmm-glib \
	-I${top_srcdir}/libmm-glib/generated \
	-I${top_builddir}/libmm-glib/generated

nodist_libport_la_SOURCES = \
	mm-port-enums-types.h \
	mm-port-enums-types.c

libport_la_SOURCES = \
	mm-port.c \
	mm-port.h \
	mm-port-serial.c \
	mm-port-serial.h \
	mm-port-serial-at.c \
	mm-port-serial-at.h \
	mm-port-serial-qcdm.c \
	mm-port-serial-qcdm.h \
	mm-port-serial-gps.c \
	mm-port-serial-gps.h \
	mm-serial-parsers.c \
	mm-serial-parsers.h

# Additional QMI support in libserial
if WITH_QMI
libport_la_SOURCES += \
	mm-port-qmi.c \
	mm-port-qmi.h
libport_la_CPPFLAGS += $(QMI_CFLAGS)
endif

# Additional MBIM support in libserial
if WITH_MBIM
libport_la_SOURCES += \
	mm-port-mbim.c \
	mm-port-mbim.h
libport_la_CPPFLAGS += $(MBIM_CFLAGS)
endif

# Request to build enum types before anything else
BUILT_SOURCES = \
	mm-port-enums-types.h \
	mm-port-enums-types.c

CLEANFILES = $(BUILT_SOURCES)

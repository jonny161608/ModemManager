
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
AM_LDFLAGS = $(CODE_COVERAGE_LDFLAGS)

noinst_LTLIBRARIES = libmmifacecdma.la

libmmifacecdma_la_CPPFLAGS = \
	$(MM_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src/port \
	-I$(top_builddir)/src/port \
	-I$(top_srcdir)/src/helpers \
	-I$(top_builddir)/src/helpers \
	-I$(top_srcdir)/src/iface-modem \
	-I$(top_srcdir)/libmm-glib \
	-I${top_srcdir}/libmm-glib/generated \
	-I${top_builddir}/libmm-glib/generated

if WITH_QMI
libmmifacecdma_la_CPPFLAGS += $(QMI_CFLAGS)
endif

if WITH_MBIM
libmmifacecdma_la_CPPFLAGS += $(MBIM_CFLAGS)
endif

libmmifacecdma_la_SOURCES = \
	mm-iface-modem-cdma.h \
	mm-iface-modem-cdma.c

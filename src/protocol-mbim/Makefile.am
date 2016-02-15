
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
AM_LDFLAGS = $(CODE_COVERAGE_LDFLAGS)

noinst_LTLIBRARIES =

if WITH_MBIM

noinst_LTLIBRARIES += libmmprotocolmbim.la

libmmprotocolmbim_la_CPPFLAGS = \
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
	-I$(top_srcdir)/src/iface-3gpp \
	-I$(top_srcdir)/src/iface-messaging \
	-I$(top_srcdir)/src/protocol-at \
	-I$(top_srcdir)/libmm-glib \
	-I${top_srcdir}/libmm-glib/generated \
	-I${top_builddir}/libmm-glib/generated \
	$(MBIM_CFLAGS)

if WITH_QMI
libmmprotocolmbim_la_CPPFLAGS += $(QMI_CFLAGS)
endif

libmmprotocolmbim_la_SOURCES = \
	mm-sms-mbim.h \
	mm-sms-mbim.c \
	mm-sim-mbim.h \
	mm-sim-mbim.c \
	mm-bearer-mbim.h \
	mm-bearer-mbim.c \
	mm-broadband-modem-mbim.h \
	mm-broadband-modem-mbim.c

endif

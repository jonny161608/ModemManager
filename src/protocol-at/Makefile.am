
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
AM_LDFLAGS = $(CODE_COVERAGE_LDFLAGS)

noinst_LTLIBRARIES = libmmprotocolat.la

libmmprotocolat_la_CPPFLAGS = \
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
	-I$(top_srcdir)/src/iface-cdma \
	-I$(top_srcdir)/src/iface-simple \
	-I$(top_srcdir)/src/iface-messaging \
	-I$(top_srcdir)/src/iface-location \
	-I$(top_srcdir)/src/iface-signal \
	-I$(top_srcdir)/src/iface-firmware \
	-I$(top_srcdir)/src/iface-oma \
	-I$(top_srcdir)/src/iface-voice \
	-I$(top_srcdir)/src/iface-time \
	-I$(top_srcdir)/libmm-glib \
	-I${top_srcdir}/libmm-glib/generated \
	-I${top_builddir}/libmm-glib/generated

if WITH_QMI
libmmprotocolat_la_CPPFLAGS += $(QMI_CFLAGS)
endif

if WITH_MBIM
libmmprotocolat_la_CPPFLAGS += $(MBIM_CFLAGS)
endif

libmmprotocolat_la_SOURCES = \
	mm-sim.h \
	mm-sim.c \
	mm-sms.h \
	mm-sms.c \
	mm-broadband-bearer.h \
	mm-broadband-bearer.c \
	mm-broadband-modem.h \
	mm-broadband-modem.c

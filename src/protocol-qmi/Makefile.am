
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
AM_LDFLAGS = $(CODE_COVERAGE_LDFLAGS)

noinst_LTLIBRARIES =

if WITH_QMI

noinst_LTLIBRARIES += libmmprotocolqmi.la

libmmprotocolqmi_la_CPPFLAGS = \
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
	-I$(top_srcdir)/libmm-glib \
	-I${top_srcdir}/libmm-glib/generated \
	-I${top_builddir}/libmm-glib/generated \
	$(QMI_CFLAGS)

if WITH_MBIM
libmmprotocolqmi_la_CPPFLAGS += $(MBIM_CFLAGS)
endif

libmmprotocolqmi_la_SOURCES = \
	mm-sms-qmi.h \
	mm-sms-qmi.c \
	mm-sim-qmi.h \
	mm-sim-qmi.c \
	mm-bearer-qmi.h \
	mm-bearer-qmi.c \
	mm-broadband-modem-qmi.h \
	mm-broadband-modem-qmi.c

endif

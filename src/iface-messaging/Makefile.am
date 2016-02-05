
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
AM_LDFLAGS = $(CODE_COVERAGE_LDFLAGS)

noinst_LTLIBRARIES = libmmifacemessaging.la

libmmifacemessaging_la_CPPFLAGS = \
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
	-I${top_builddir}/libmm-glib/generated

if WITH_QMI
libmmifacemessaging_la_CPPFLAGS += $(QMI_CFLAGS)
endif

if WITH_MBIM
libmmifacemessaging_la_CPPFLAGS += $(MBIM_CFLAGS)
endif

libmmifacemessaging_la_SOURCES = \
	mm-base-sms.h \
	mm-base-sms.c \
	mm-sms-list.h \
	mm-sms-list.c \
	mm-iface-modem-messaging.h \
	mm-iface-modem-messaging.c

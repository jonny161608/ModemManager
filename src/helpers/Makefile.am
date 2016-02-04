
AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
AM_LDFLAGS = $(CODE_COVERAGE_LDFLAGS)

noinst_LTLIBRARIES = libhelpers.la

libhelpers_la_CPPFLAGS = \
	$(MM_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/libmm-glib \
	-I${top_srcdir}/libmm-glib/generated \
	-I${top_builddir}/libmm-glib/generated

libhelpers_la_LIBADD = \
	$(top_builddir)/libmm-glib/libmm-glib.la

libhelpers_la_SOURCES = \
	mm-utils.h \
	mm-error-helpers.c \
	mm-error-helpers.h \
	mm-modem-helpers.c \
	mm-modem-helpers.h \
	mm-charsets.c \
	mm-charsets.h \
	mm-sms-part.h \
	mm-sms-part.c \
	mm-sms-part-3gpp.h \
	mm-sms-part-3gpp.c \
	mm-sms-part-cdma.h \
	mm-sms-part-cdma.c

# Additional QMI support
if WITH_QMI
libhelpers_la_SOURCES += \
	mm-modem-helpers-qmi.c \
	mm-modem-helpers-qmi.h
libhelpers_la_CPPFLAGS += $(QMI_CFLAGS)
endif

# Additional MBIM support
if WITH_MBIM
libhelpers_la_SOURCES += \
	mm-modem-helpers-mbim.c \
	mm-modem-helpers-mbim.h
libhelpers_la_CPPFLAGS += $(MBIM_CFLAGS)
endif

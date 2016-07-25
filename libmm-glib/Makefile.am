SUBDIRS = generated . tests

AM_CFLAGS  = $(CODE_COVERAGE_CFLAGS)
AM_LDFLAGS = $(CODE_COVERAGE_LDFLAGS)

lib_LTLIBRARIES = libmm-glib.la

libmm_glib_la_SOURCES = \
	libmm-glib.h \
	mm-helpers.h \
	mm-helper-types.h \
	mm-helper-types.c \
	mm-manager.h \
	mm-manager.c \
	mm-object.h \
	mm-object.c \
	mm-modem.h \
	mm-modem.c \
	mm-modem-3gpp.h \
	mm-modem-3gpp.c \
	mm-modem-cdma.h \
	mm-modem-cdma.c \
	mm-modem-simple.h \
	mm-modem-simple.c \
	mm-sim.h \
	mm-sim.c \
	mm-bearer.h \
	mm-bearer.c \
	mm-common-helpers.h \
	mm-common-helpers.c \
	mm-simple-status.h \
	mm-simple-status.c \
	mm-simple-connect-properties.h \
	mm-simple-connect-properties.c \
	mm-bearer-properties.h \
	mm-bearer-properties.c \
	mm-bearer-ip-config.h \
	mm-bearer-ip-config.c \
	mm-bearer-stats.h \
	mm-bearer-stats.c \
	mm-unlock-retries.h \
	mm-unlock-retries.c \
	mm-cdma-manual-activation-properties.h \
	mm-cdma-manual-activation-properties.c \
	mm-kernel-event-properties.h \
	mm-kernel-event-properties.c \
	$(NULL)

if WITH_INTERFACE_3GPP_USSD
libmm_glib_la_SOURCES += \
	mm-modem-3gpp-ussd.h \
	mm-modem-3gpp-ussd.c \
	$(NULL)
endif

if WITH_INTERFACE_LOCATION
libmm_glib_la_SOURCES += \
	mm-modem-location.h \
	mm-modem-location.c \
	mm-location-common.h \
	mm-location-3gpp.h \
	mm-location-3gpp.c \
	mm-location-gps-raw.h \
	mm-location-gps-raw.c \
	mm-location-gps-nmea.h \
	mm-location-gps-nmea.c \
	mm-location-cdma-bs.h \
	mm-location-cdma-bs.c \
	$(NULL)
endif

if WITH_INTERFACE_MESSAGING
libmm_glib_la_SOURCES += \
	mm-sms.h \
	mm-sms.c \
	mm-modem-messaging.h \
	mm-modem-messaging.c \
	mm-sms-properties.h \
	mm-sms-properties.c \
	$(NULL)
endif

if WITH_INTERFACE_TIME
libmm_glib_la_SOURCES += \
	mm-modem-time.h \
	mm-modem-time.c \
	mm-network-timezone.h \
	mm-network-timezone.c \
	$(NULL)
endif

if WITH_INTERFACE_VOICE
libmm_glib_la_SOURCES += \
	mm-call.h \
	mm-call.c \
	mm-modem-voice.h \
	mm-modem-voice.c \
	mm-call-properties.h \
	mm-call-properties.c \
	$(NULL)
endif

if WITH_INTERFACE_SIGNAL
libmm_glib_la_SOURCES += \
	mm-modem-signal.h \
	mm-modem-signal.c \
	mm-signal.h \
	mm-signal.c \
	$(NULL)
endif

if WITH_INTERFACE_OMA
libmm_glib_la_SOURCES += \
	mm-modem-oma.h \
	mm-modem-oma.c \
	$(NULL)
endif

if WITH_INTERFACE_FIRMWARE
libmm_glib_la_SOURCES += \
	mm-modem-firmware.h \
	mm-modem-firmware.c \
	mm-firmware-properties.h \
	mm-firmware-properties.c \
	$(NULL)
endif

libmm_glib_la_CPPFLAGS = \
	-I$(srcdir) \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-I${top_srcdir}/libmm-glib/generated \
	-I${top_builddir}/libmm-glib/generated \
	-DLIBMM_GLIB_COMPILATION \
	$(AM_CPPFLAGS) \
	$(NULL)

libmm_glib_la_CFLAGS = \
	$(LIBMM_GLIB_CFLAGS) \
	$(AM_CFLAGS) \
	$(NULL)

libmm_glib_la_LIBADD = \
	${top_builddir}/libmm-glib/generated/libmm-generated.la \
	$(LIBMM_GLIB_LIBS) \
	$(NULL)

libmm_glib_la_LDFLAGS = \
	-version-info $(MM_GLIB_LT_CURRENT):$(MM_GLIB_LT_REVISION):$(MM_GLIB_LT_AGE) \
	$(AM_LDFLAGS) \
	$(NULL)

includedir = @includedir@/libmm-glib
include_HEADERS = \
	libmm-glib.h \
	mm-helper-types.h \
	mm-manager.h \
	mm-object.h \
	mm-modem.h \
	mm-modem-3gpp.h \
	mm-modem-cdma.h \
	mm-modem-simple.h \
	mm-sim.h \
	mm-bearer.h \
	mm-simple-status.h \
	mm-simple-connect-properties.h \
	mm-bearer-properties.h \
	mm-bearer-ip-config.h \
	mm-bearer-stats.h \
	mm-unlock-retries.h \
	mm-cdma-manual-activation-properties.h \
	mm-kernel-event-properties.h \
	$(NULL)

if WITH_INTERFACE_3GPP_USSD
include_HEADERS += \
	mm-modem-3gpp-ussd.h \
	$(NULL)
endif

if WITH_INTERFACE_LOCATION
include_HEADERS += \
	mm-modem-location.h \
	mm-location-common.h \
	mm-location-3gpp.h \
	mm-location-gps-nmea.h \
	mm-location-gps-raw.h \
	mm-location-cdma-bs.h \
	$(NULL)
endif

if WITH_INTERFACE_MESSAGING
include_HEADERS += \
	mm-modem-messaging.h \
	mm-sms.h \
	mm-sms-properties.h \
	$(NULL)
endif

if WITH_INTERFACE_TIME
include_HEADERS += \
	mm-modem-time.h \
	mm-network-timezone.h \
	$(NULL)
endif

if WITH_INTERFACE_VOICE
include_HEADERS += \
	mm-modem-voice.h \
	mm-call.h \
	mm-call-properties.h \
	$(NULL)
endif

if WITH_INTERFACE_SIGNAL
include_HEADERS += \
	mm-modem-signal.h \
	mm-signal.h \
	$(NULL)
endif

if WITH_INTERFACE_OMA
include_HEADERS += \
	mm-modem-oma.h \
	$(NULL)
endif

if WITH_INTERFACE_FIRMWARE
include_HEADERS += \
	mm-modem-firmware.h \
	mm-firmware-properties.h \
	$(NULL)
endif

CLEANFILES =

# Introspection

if HAVE_INTROSPECTION

GENERATED_H = \
	mm-enums-types.h \
	mm-errors-types.h \
	mm-gdbus-manager.h \
	mm-gdbus-sim.h \
	mm-gdbus-bearer.h \
	mm-gdbus-modem.h \
	$(NULL)

GENERATED_C = \
	mm-enums-types.c \
	mm-errors-types.c \
	mm-errors-quarks.c \
	mm-gdbus-manager.c \
	mm-gdbus-sim.c \
	mm-gdbus-bearer.c \
	mm-gdbus-modem.c \
	$(NULL)

PUBLIC_H = \
	ModemManager-names.h \
	ModemManager-version.h
	ModemManager-enums.h \
	ModemManager-errors.h \
	ModemManager-compat.h \
	ModemManager.h \
	$(NULL)

if WITH_INTERFACE_3GPP_USSD
PUBLIC_H += ModemManager-enums-3gpp-ussd.h
endif

if WITH_INTERFACE_LOCATION
PUBLIC_H += ModemManager-enums-location.h
endif

if WITH_INTERFACE_MESSAGING
GENERATED_H += mm-gdbus-sms.h
GENERATED_C += mm-gdbus-sms.c
PUBLIC_H    += ModemManager-enums-messaging.h
endif

if WITH_INTERFACE_VOICE
GENERATED_H += mm-gdbus-call.h
GENERATED_C += mm-gdbus-call.c
PUBLIC_H    += ModemManager-enums-voice.h
endif

if WITH_INTERFACE_OMA
PUBLIC_H += ModemManager-enums-oma.h
endif

INTROSPECTION_GIRS = ModemManager-1.0.gir
INTROSPECTION_SCANNER_ARGS = --warn-all
INTROSPECTION_COMPILER_ARGS =

ModemManager-1.0.gir: libmm-glib.la
ModemManager_1_0_gir_INCLUDES = GLib-2.0 GObject-2.0 Gio-2.0
ModemManager_1_0_gir_CFLAGS = $(libmm_glib_la_CPPFLAGS)
ModemManager_1_0_gir_LIBS = libmm-glib.la
ModemManager_1_0_gir_EXPORT_PACKAGES = libmm-glib
ModemManager_1_0_gir_SCANNERFLAGS = \
	--c-include "libmm-glib.h" \
	--identifier-prefix=MM \
	--identifier-prefix=Mm \
	--symbol-prefix=mm \
	$(NULL)
ModemManager_1_0_gir_FILES = \
	$(include_HEADERS) \
	$(filter-out %.h,$(libmm_glib_la_SOURCES)) \
	$(filter %.c,$(libmm_glib_la_SOURCES)) \
	$(addprefix generated/,$(GENERATED_H)) \
	$(addprefix generated/,$(GENERATED_C)) \
	$(addprefix ../include/,$(PUBLIC_H)) \
	$(NULL)

girdir = $(datadir)/gir-1.0
nodist_gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
nodist_typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(nodist_gir_DATA) $(nodist_typelib_DATA)

endif # HAVE_INTROSPECTION

-include $(INTROSPECTION_MAKEFILE)

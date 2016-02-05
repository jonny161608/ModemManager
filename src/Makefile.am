
SUBDIRS = \
	port          \
	helpers       \
	protocol-qmi  \
	protocol-mbim \
	.             \
	tests

udevrulesdir = $(UDEV_BASE_DIR)/rules.d
udevrules_DATA = \
	77-mm-usb-device-blacklist.rules \
	77-mm-pcmcia-device-blacklist.rules \
	77-mm-platform-serial-whitelist.rules \
	77-mm-usb-serial-adapters-greylist.rules \
	80-mm-candidate.rules

AM_CFLAGS = $(CODE_COVERAGE_CFLAGS)
AM_LDFLAGS = $(CODE_COVERAGE_LDFLAGS)

# Daemon specific enum types
DAEMON_ENUMS = \
	$(srcdir)/mm-base-bearer.h \
	$(srcdir)/mm-port-probe.h

mm-daemon-enums-types.h: Makefile.am $(DAEMON_ENUMS) $(top_srcdir)/build-aux/mm-enums-template.h
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--fhead "#include \"mm-base-bearer.h\"\n#include \"mm-port-probe.h\"\n#ifndef __MM_DAEMON_ENUMS_TYPES_H__\n#define __MM_DAEMON_ENUMS_TYPES_H__\n" \
		--template $(top_srcdir)/build-aux/mm-enums-template.h \
		--ftail "#endif /* __MM_DAEMON_ENUMS_TYPES_H__ */\n" \
		$(DAEMON_ENUMS) > $@

mm-daemon-enums-types.c: Makefile.am $(top_srcdir)/build-aux/mm-enums-template.c mm-daemon-enums-types.h
	$(AM_V_GEN) $(GLIB_MKENUMS) \
		--fhead "#include \"mm-daemon-enums-types.h\"" \
		--template $(top_srcdir)/build-aux/mm-enums-template.c \
		$(DAEMON_ENUMS) > $@

sbin_PROGRAMS = ModemManager

ModemManager_CPPFLAGS = \
	$(MM_CFLAGS) \
	$(GUDEV_CFLAGS) \
	-I$(srcdir)/port \
	-I$(builddir)/port \
	-I$(srcdir)/helpers \
	-I$(builddir)/helpers \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/libmm-glib \
	-I$(top_builddir)/libmm-glib \
	-I${top_srcdir}/libmm-glib/generated \
	-I${top_builddir}/libmm-glib/generated \
	-I${top_srcdir}/libmm-glib/generated/tests \
	-I${top_builddir}/libmm-glib/generated/tests \
	-DPLUGINDIR=\"$(pkglibdir)\"

ModemManager_LDADD = \
	$(MM_LIBS) \
	$(GUDEV_LIBS) \
	$(builddir)/helpers/libhelpers.la \
	$(builddir)/port/libport.la \
	$(top_builddir)/libqcdm/src/libqcdm.la \
	$(top_builddir)/libmm-glib/generated/tests/libmm-test-generated.la

if WITH_QMI
ModemManager_CPPFLAGS += -I$(srcdir)/protocol-qmi $(QMI_CFLAGS)
ModemManager_LDADD += $(builddir)/protocol-qmi/libmmprotocolqmi.la $(QMI_LIBS)
endif

if WITH_MBIM
ModemManager_CPPFLAGS += -I$(srcdir)/protocol-mbim $(MBIM_CFLAGS)
ModemManager_LDADD += $(builddir)/protocol-mbim/libmmprotocolmbim.la $(MBIM_LIBS)
endif

nodist_ModemManager_SOURCES = \
	mm-daemon-enums-types.h \
	mm-daemon-enums-types.c

ModemManager_SOURCES = \
	main.c \
	mm-context.h \
	mm-context.c \
	mm-log.c \
	mm-log.h \
	mm-private-boxed-types.h \
	mm-private-boxed-types.c \
	mm-auth.h \
	mm-auth.c \
	mm-auth-provider.h \
	mm-auth-provider.c \
	mm-base-manager.c \
	mm-base-manager.h \
	mm-device.c \
	mm-device.h \
	mm-plugin-manager.c \
	mm-plugin-manager.h \
	mm-base-sim.h \
	mm-base-sim.c \
	mm-base-bearer.h \
	mm-base-bearer.c \
	mm-broadband-bearer.h \
	mm-broadband-bearer.c \
	mm-bearer-list.h \
	mm-bearer-list.c \
	mm-base-modem-at.h \
	mm-base-modem-at.c \
	mm-base-modem.h \
	mm-base-modem.c \
	mm-base-sms.h \
	mm-base-sms.c \
	mm-base-call.h \
	mm-base-call.c \
	mm-sms-list.h \
	mm-sms-list.c \
	mm-call-list.h \
	mm-call-list.c \
	mm-iface-modem.h \
	mm-iface-modem.c \
	mm-iface-modem-3gpp.h \
	mm-iface-modem-3gpp.c \
	mm-iface-modem-3gpp-ussd.h \
	mm-iface-modem-3gpp-ussd.c \
	mm-iface-modem-cdma.h \
	mm-iface-modem-cdma.c \
	mm-iface-modem-simple.h \
	mm-iface-modem-simple.c \
	mm-iface-modem-location.h \
	mm-iface-modem-location.c \
	mm-iface-modem-messaging.h \
	mm-iface-modem-messaging.c \
	mm-iface-modem-voice.h \
	mm-iface-modem-voice.c \
	mm-iface-modem-time.h \
	mm-iface-modem-time.c \
	mm-iface-modem-firmware.h \
	mm-iface-modem-firmware.c \
	mm-iface-modem-signal.h \
	mm-iface-modem-signal.c \
	mm-iface-modem-oma.h \
	mm-iface-modem-oma.c \
	mm-broadband-modem.h \
	mm-broadband-modem.c \
	mm-port-probe.h \
	mm-port-probe.c \
	mm-port-probe-at.h \
	mm-port-probe-at.c \
	mm-plugin.c \
	mm-plugin.h

# Additional Polkit support
if WITH_POLKIT
ModemManager_SOURCES += \
	mm-auth-provider-polkit.c \
	mm-auth-provider-polkit.h
ModemManager_LDADD += $(POLKIT_LIBS)
ModemManager_CPPFLAGS += $(POLKIT_CFLAGS)
endif

# Additional suspend/resume support via systemd
if SUSPEND_RESUME_SYSTEMD
ModemManager_SOURCES += mm-sleep-monitor.h mm-sleep-monitor-systemd.c
endif

# Additional suspend/resume support via upower
if SUSPEND_RESUME_UPOWER
ModemManager_SOURCES += mm-sleep-monitor.h mm-sleep-monitor-upower.c
endif

EXTRA_DIST = \
	$(udevrules_DATA)

# Request to build enum types before anything else
BUILT_SOURCES = \
	mm-daemon-enums-types.h \
	mm-daemon-enums-types.c

CLEANFILES = $(BUILT_SOURCES)

bin_PROGRAMS = mmcli

mmcli_CPPFLAGS = \
	$(MMCLI_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/libmm-glib \
	-I${top_srcdir}/libmm-glib/generated \
	-I${top_builddir}/libmm-glib/generated \
	$(NULL)

mmcli_SOURCES = \
	mmcli.h \
	mmcli.c \
	mmcli-common.h \
	mmcli-common.c \
	mmcli-manager.c \
	mmcli-modem.c \
	mmcli-modem-3gpp.c \
	mmcli-modem-cdma.c \
	mmcli-modem-simple.c \
	mmcli-modem-voice.c \
	mmcli-modem-firmware.c \
	mmcli-modem-signal.c \
	mmcli-modem-oma.c \
	mmcli-bearer.c \
	mmcli-sim.c \
	mmcli-call.c \
	$(NULL)

if WITH_INTERFACE_LOCATION
mmcli_SOURCES += mmcli-modem-location.c
endif

if WITH_INTERFACE_MESSAGING
mmcli_SOURCES += \
	mmcli-modem-messaging.c \
	mmcli-sms.c \
	$(NULL)
endif

if WITH_INTERFACE_TIME
mmcli_SOURCES += mmcli-modem-time.c
endif

mmcli_LDADD = \
	$(MMCLI_LIBS) \
	$(top_builddir)/libmm-glib/libmm-glib.la \
	$(NULL)

if WITH_UDEV
mmcli_CPPFLAGS += $(GUDEV_CFLAGS)
mmcli_LDADD    += $(GUDEV_LIBS)
endif

completiondir = $(datadir)/bash-completion/completions

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(completiondir)
	$(INSTALL_DATA) $(srcdir)/mmcli-completion $(DESTDIR)$(completiondir)/mmcli

uninstall-hook:
	rm -f $(DESTDIR)$(completiondir)/mmcli

EXTRA_DIST = mmcli-completion

XSLTPROC = xsltproc --xinclude --nonet

XMLS = $(wildcard $(top_srcdir)/introspection/*.xml)

includedir = @includedir@/ModemManager

include_HEADERS = \
	ModemManager-enums.h \
	ModemManager-errors.h \
	ModemManager-compat.h \
	ModemManager.h \
	$(NULL)

nodist_include_HEADERS = \
	ModemManager-names.h \
	ModemManager-version.h \
	$(NULL)

if WITH_INTERFACE_3GPP_USSD
include_HEADERS += ModemManager-enums-3gpp-ussd.h
endif

if WITH_INTERFACE_LOCATION
include_HEADERS += ModemManager-enums-location.h
endif

if WITH_INTERFACE_MESSAGING
include_HEADERS += ModemManager-enums-messaging.h
endif

if WITH_INTERFACE_VOICE
include_HEADERS += ModemManager-enums-voice.h
endif

if WITH_INTERFACE_OMA
include_HEADERS += ModemManager-enums-oma.h
endif

if WITH_INTERFACE_FIRMWARE
include_HEADERS += ModemManager-enums-firmware.h
endif

ModemManager-names.h: $(XMLS) $(top_srcdir)/build-aux/header-generator.xsl
	$(AM_V_GEN) $(XSLTPROC) --path $(top_srcdir)/introspection $(top_srcdir)/build-aux/header-generator.xsl $(top_builddir)/introspection/all.xml > $@

CLEANFILES = \
	ModemManager-names.h

EXTRA_DIST = \
	ModemManager-version.h.in

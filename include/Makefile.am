XSLTPROC = xsltproc --xinclude --nonet

XMLS = $(wildcard $(top_srcdir)/introspection/*.xml)

includedir = @includedir@/ModemManager

include_HEADERS = \
	ModemManager-names.h \
	ModemManager-enums.h \
	ModemManager-errors.h \
	ModemManager-compat.h \
	ModemManager-version.h \
	ModemManager.h

if WITH_INTERFACE_LOCATION
include_HEADERS += ModemManager-enums-location.h
endif

if WITH_INTERFACE_MESSAGING
include_HEADERS += ModemManager-enums-messaging.h
endif

ModemManager-names.h: $(XMLS) $(top_srcdir)/build-aux/header-generator.xsl
	$(AM_V_GEN) $(XSLTPROC) $(top_srcdir)/build-aux/header-generator.xsl $(top_builddir)/introspection/all.xml > $@

CLEANFILES = \
	ModemManager-names.h

EXTRA_DIST = \
	ModemManager-version.h.in


SUBDIRS = . tests

# DBus Introspection files
xmldir = $(datadir)/dbus-1/interfaces
xml_DATA = \
	org.freedesktop.ModemManager1.xml \
	org.freedesktop.ModemManager1.Modem.xml \
	org.freedesktop.ModemManager1.Modem.Modem3gpp.xml \
	org.freedesktop.ModemManager1.Modem.ModemCdma.xml \
	org.freedesktop.ModemManager1.Modem.Simple.xml \
	org.freedesktop.ModemManager1.Sim.xml \
	org.freedesktop.ModemManager1.Bearer.xml \
	org.freedesktop.ModemManager1.Modem.Messaging.xml \
	org.freedesktop.ModemManager1.Sms.xml \
	org.freedesktop.ModemManager1.Modem.Modem3gpp.Ussd.xml \
	org.freedesktop.ModemManager1.Modem.Firmware.xml \
	org.freedesktop.ModemManager1.Modem.Oma.xml \
	org.freedesktop.ModemManager1.Modem.Signal.xml \
	org.freedesktop.ModemManager1.Modem.Time.xml \
	org.freedesktop.ModemManager1.Modem.Voice.xml \
	org.freedesktop.ModemManager1.Call.xml \
	$(NULL)

if WITH_INTERFACE_LOCATION
xml_DATA += org.freedesktop.ModemManager1.Modem.Location.xml
endif

all.xml: $(xml_DATA) Makefile
	$(AM_V_GEN)	\
		rm -f $@; \
		echo "<tp:spec" >> $@; \
		echo "   xmlns:tp=\"http://telepathy.freedesktop.org/wiki/DbusSpec#extensions-v0\"" >> $@; \
		echo "   xmlns:xi=\"http://www.w3.org/2001/XInclude\">" >> $@; \
		for single in $(xml_DATA); do \
			echo "   <xi:include href=\"$$single\"/>" >> $@; \
		done; \
		echo "</tp:spec>" >> $@

BUILT_SOURCES = all.xml
CLEANFILES = all.xml

EXTRA_DIST = \
	$(xml_DATA) \
	$(NULL)

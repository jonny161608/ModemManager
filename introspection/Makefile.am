
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
	org.freedesktop.ModemManager1.Sms.xml \
	org.freedesktop.ModemManager1.Modem.Modem3gpp.Ussd.xml \
	$(NULL)

if WITH_INTERFACE_LOCATION
xml_DATA += org.freedesktop.ModemManager1.Modem.Location.xml
endif

if WITH_INTERFACE_MESSAGING
xml_DATA += org.freedesktop.ModemManager1.Modem.Messaging.xml
endif

if WITH_INTERFACE_TIME
xml_DATA += org.freedesktop.ModemManager1.Modem.Time.xml
endif

if WITH_INTERFACE_VOICE
xml_DATA += \
	org.freedesktop.ModemManager1.Modem.Voice.xml \
	org.freedesktop.ModemManager1.Call.xml \
	$(NULL)
endif

if WITH_INTERFACE_SIGNAL
xml_DATA += org.freedesktop.ModemManager1.Modem.Signal.xml
endif

if WITH_INTERFACE_OMA
xml_DATA += org.freedesktop.ModemManager1.Modem.Oma.xml
endif

if WITH_INTERFACE_FIRMWARE
xml_DATA += org.freedesktop.ModemManager1.Modem.Firmware.xml
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

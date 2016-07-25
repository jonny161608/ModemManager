/*
 * ModemManager Interface Specification
 * version 0.8
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 * Copyright (C) 2008 - 2009 Novell, Inc.
 * Copyright (C) 2009 - 2013 Red Hat, Inc.
 * Copyright (C) 2011 - 2013 Google, Inc.
 * Copyright (C) 2011 - 2013 Lanedo Gmbh

 */

#ifndef _MODEM_MANAGER_H_
#define _MODEM_MANAGER_H_

#define __MODEM_MANAGER_H_INSIDE__

/* Public header with DBus Interface, Method, Signal and Property names */
#include <ModemManager-names.h>

/* Public header with version info */
#include <ModemManager-version.h>

/* Public headers with enumerations and flags */
#include <ModemManager-enums.h>
#if MM_INTERFACE_3GPP_USSD_SUPPORTED
# include <ModemManager-enums-3gpp-ussd.h>
#endif
#if MM_INTERFACE_LOCATION_SUPPORTED
# include <ModemManager-enums-location.h>
#endif
#if MM_INTERFACE_MESSAGING_SUPPORTED
# include <ModemManager-enums-messaging.h>
#endif
#if MM_INTERFACE_VOICE_SUPPORTED
# include <ModemManager-enums-voice.h>
#endif
#if MM_INTERFACE_OMA_SUPPORTED
# include <ModemManager-enums-oma.h>
#endif
#if MM_INTERFACE_FIRMWARE_SUPPORTED
# include <ModemManager-enums-firmware.h>
#endif

/* Public header with errors */
#include <ModemManager-errors.h>

/* Public header with compability types and methods */
#include <ModemManager-compat.h>

#endif /*  _MODEM_MANAGER_H_ */

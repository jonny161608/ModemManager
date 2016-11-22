/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details:
 *
 * Copyright (C) 2011 Red Hat, Inc.
 * Copyright (C) 2011 Google, Inc.
 * Copyright (C) 2016 Aleksander Morgado <aleksander@aleksander.es>
 */

#ifndef _MODEMMANAGER_ENUMS_LOCATION_H_
#define _MODEMMANAGER_ENUMS_LOCATION_H_

#if !defined (__MODEM_MANAGER_H_INSIDE__)
#error "Only <ModemManager.h> can be included directly."
#endif

/**
 * SECTION:mm-enums-location
 * @short_description: Location-specific enumerations and types in the API.
 *
 * This section defines enumerations and types that are used in the
 * ModemManager Location interface.
 **/

/**
 * MMModemLocationSource:
 * @MM_MODEM_LOCATION_SOURCE_NONE: None.
 * @MM_MODEM_LOCATION_SOURCE_3GPP_LAC_CI: Location Area Code and Cell ID.
 * @MM_MODEM_LOCATION_SOURCE_GPS_RAW: GPS location given by predefined keys.
 * @MM_MODEM_LOCATION_SOURCE_GPS_NMEA: GPS location given as NMEA traces.
 * @MM_MODEM_LOCATION_SOURCE_CDMA_BS: CDMA base station position.
 * @MM_MODEM_LOCATION_SOURCE_GPS_UNMANAGED: No location given, just GPS module setup.
 * @MM_MODEM_LOCATION_SOURCE_AGPS: A-GPS location requested.
 *
 * Sources of location information supported by the modem.
 */
typedef enum { /*< underscore_name=mm_modem_location_source >*/
    MM_MODEM_LOCATION_SOURCE_NONE          = 0,
    MM_MODEM_LOCATION_SOURCE_3GPP_LAC_CI   = 1 << 0,
    MM_MODEM_LOCATION_SOURCE_GPS_RAW       = 1 << 1,
    MM_MODEM_LOCATION_SOURCE_GPS_NMEA      = 1 << 2,
    MM_MODEM_LOCATION_SOURCE_CDMA_BS       = 1 << 3,
    MM_MODEM_LOCATION_SOURCE_GPS_UNMANAGED = 1 << 4,
    MM_MODEM_LOCATION_SOURCE_AGPS          = 1 << 5,
} MMModemLocationSource;

#endif /*  _MODEMMANAGER_ENUMS_LOCATION_H_ */

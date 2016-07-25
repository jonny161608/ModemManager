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

#ifndef _MODEMMANAGER_ENUMS_3GPP_USSD_H_
#define _MODEMMANAGER_ENUMS_3GPP_USSD_H_

#if !defined (__MODEM_MANAGER_H_INSIDE__)
#error "Only <ModemManager.h> can be included directly."
#endif

/**
 * SECTION:mm-enums-3gpp-ussd
 * @short_description: Common enumerations and types in the API.
 *
 * This section defines enumerations and types that are used in the
 * ModemManager 3GPP USSD interface.
 **/

/**
 * MMModem3gppUssdSessionState:
 * @MM_MODEM_3GPP_USSD_SESSION_STATE_UNKNOWN: Unknown state.
 * @MM_MODEM_3GPP_USSD_SESSION_STATE_IDLE: No active session.
 * @MM_MODEM_3GPP_USSD_SESSION_STATE_ACTIVE: A session is active and the mobile is waiting for a response.
 * @MM_MODEM_3GPP_USSD_SESSION_STATE_USER_RESPONSE: The network is waiting for the client's response.
 *
 * State of a USSD session.
 */
typedef enum { /*< underscore_name=mm_modem_3gpp_ussd_session_state >*/
    MM_MODEM_3GPP_USSD_SESSION_STATE_UNKNOWN       = 0,
    MM_MODEM_3GPP_USSD_SESSION_STATE_IDLE          = 1,
    MM_MODEM_3GPP_USSD_SESSION_STATE_ACTIVE        = 2,
    MM_MODEM_3GPP_USSD_SESSION_STATE_USER_RESPONSE = 3,
} MMModem3gppUssdSessionState;

#endif /*  _MODEMMANAGER_ENUMS_3GPP_USSD_H_ */

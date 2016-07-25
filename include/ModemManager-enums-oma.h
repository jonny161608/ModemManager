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

#ifndef _MODEMMANAGER_ENUMS_OMA_H_
#define _MODEMMANAGER_ENUMS_OMA_H_

#if !defined (__MODEM_MANAGER_H_INSIDE__)
#error "Only <ModemManager.h> can be included directly."
#endif

/**
 * SECTION:mm-enums-oma
 * @short_description: Common enumerations and types in the API.
 *
 * This section defines enumerations and types that are used in the
 * OMA ModemManager interface.
 **/

/**
 * MMOmaFeature:
 * @MM_OMA_FEATURE_NONE: None.
 * @MM_OMA_FEATURE_DEVICE_PROVISIONING: Device provisioning service.
 * @MM_OMA_FEATURE_PRL_UPDATE: PRL update service.
 * @MM_OMA_FEATURE_HANDS_FREE_ACTIVATION: Hands free activation service.
 *
 * Features that can be enabled or disabled in the OMA device management support.
 */
typedef enum { /*< underscore_name=mm_oma_feature >*/
    MM_OMA_FEATURE_NONE                  = 0,
    MM_OMA_FEATURE_DEVICE_PROVISIONING   = 1 << 0,
    MM_OMA_FEATURE_PRL_UPDATE            = 1 << 1,
    MM_OMA_FEATURE_HANDS_FREE_ACTIVATION = 1 << 2,
} MMOmaFeature;

/**
 * MMOmaSessionType:
 * @MM_OMA_SESSION_TYPE_UNKNOWN: Unknown session type.
 * @MM_OMA_SESSION_TYPE_CLIENT_INITIATED_DEVICE_CONFIGURE: Client-initiated device configure.
 * @MM_OMA_SESSION_TYPE_CLIENT_INITIATED_PRL_UPDATE: Client-initiated PRL update.
 * @MM_OMA_SESSION_TYPE_CLIENT_INITIATED_HANDS_FREE_ACTIVATION: Client-initiated hands free activation.
 * @MM_OMA_SESSION_TYPE_NETWORK_INITIATED_DEVICE_CONFIGURE: Network-initiated device configure.
 * @MM_OMA_SESSION_TYPE_NETWORK_INITIATED_PRL_UPDATE: Network-initiated PRL update.
 * @MM_OMA_SESSION_TYPE_DEVICE_INITIATED_PRL_UPDATE: Device-initiated PRL update.
 * @MM_OMA_SESSION_TYPE_DEVICE_INITIATED_HANDS_FREE_ACTIVATION: Device-initiated hands free activation.
 *
 * Type of OMA device management session.
 */
typedef enum { /*< underscore_name=mm_oma_session_type >*/
    MM_OMA_SESSION_TYPE_UNKNOWN                                = 0,
    MM_OMA_SESSION_TYPE_CLIENT_INITIATED_DEVICE_CONFIGURE      = 10,
    MM_OMA_SESSION_TYPE_CLIENT_INITIATED_PRL_UPDATE            = 11,
    MM_OMA_SESSION_TYPE_CLIENT_INITIATED_HANDS_FREE_ACTIVATION = 12,
    MM_OMA_SESSION_TYPE_NETWORK_INITIATED_DEVICE_CONFIGURE     = 20,
    MM_OMA_SESSION_TYPE_NETWORK_INITIATED_PRL_UPDATE           = 21,
    MM_OMA_SESSION_TYPE_DEVICE_INITIATED_PRL_UPDATE            = 30,
    MM_OMA_SESSION_TYPE_DEVICE_INITIATED_HANDS_FREE_ACTIVATION = 31,
} MMOmaSessionType;

/**
 * MMOmaSessionState:
 * @MM_OMA_SESSION_STATE_FAILED: Failed.
 * @MM_OMA_SESSION_STATE_UNKNOWN: Unknown.
 * @MM_OMA_SESSION_STATE_STARTED: Started.
 * @MM_OMA_SESSION_STATE_RETRYING: Retrying.
 * @MM_OMA_SESSION_STATE_CONNECTING: Connecting.
 * @MM_OMA_SESSION_STATE_CONNECTED: Connected.
 * @MM_OMA_SESSION_STATE_AUTHENTICATED: Authenticated.
 * @MM_OMA_SESSION_STATE_MDN_DOWNLOADED: MDN downloaded.
 * @MM_OMA_SESSION_STATE_MSID_DOWNLOADED: MSID downloaded.
 * @MM_OMA_SESSION_STATE_PRL_DOWNLOADED: PRL downloaded.
 * @MM_OMA_SESSION_STATE_MIP_PROFILE_DOWNLOADED: MIP profile downloaded.
 * @MM_OMA_SESSION_STATE_COMPLETED: Session completed.
 *
 * State of the OMA device management session.
 */
typedef enum { /*< underscore_name=mm_oma_session_state >*/
    MM_OMA_SESSION_STATE_FAILED                 = -1,
    MM_OMA_SESSION_STATE_UNKNOWN                = 0,
    MM_OMA_SESSION_STATE_STARTED                = 1,
    MM_OMA_SESSION_STATE_RETRYING               = 2,
    MM_OMA_SESSION_STATE_CONNECTING             = 3,
    MM_OMA_SESSION_STATE_CONNECTED              = 4,
    MM_OMA_SESSION_STATE_AUTHENTICATED          = 5,
    MM_OMA_SESSION_STATE_MDN_DOWNLOADED         = 10,
    MM_OMA_SESSION_STATE_MSID_DOWNLOADED        = 11,
    MM_OMA_SESSION_STATE_PRL_DOWNLOADED         = 12,
    MM_OMA_SESSION_STATE_MIP_PROFILE_DOWNLOADED = 13,
    MM_OMA_SESSION_STATE_COMPLETED              = 20,
} MMOmaSessionState;

/**
 * MMOmaSessionStateFailedReason:
 * @MM_OMA_SESSION_STATE_FAILED_REASON_UNKNOWN: No reason or unknown.
 * @MM_OMA_SESSION_STATE_FAILED_REASON_NETWORK_UNAVAILABLE: Network unavailable.
 * @MM_OMA_SESSION_STATE_FAILED_REASON_SERVER_UNAVAILABLE: Server unavailable.
 * @MM_OMA_SESSION_STATE_FAILED_REASON_AUTHENTICATION_FAILED: Authentication failed.
 * @MM_OMA_SESSION_STATE_FAILED_REASON_MAX_RETRY_EXCEEDED: Maximum retries exceeded.
 * @MM_OMA_SESSION_STATE_FAILED_REASON_SESSION_CANCELLED: Session cancelled.
 *
 * Reason of failure in the OMA device management session.
 */
typedef enum { /*< underscore_name=mm_oma_session_state_failed_reason >*/
    MM_OMA_SESSION_STATE_FAILED_REASON_UNKNOWN               = 0,
    MM_OMA_SESSION_STATE_FAILED_REASON_NETWORK_UNAVAILABLE   = 1,
    MM_OMA_SESSION_STATE_FAILED_REASON_SERVER_UNAVAILABLE    = 2,
    MM_OMA_SESSION_STATE_FAILED_REASON_AUTHENTICATION_FAILED = 3,
    MM_OMA_SESSION_STATE_FAILED_REASON_MAX_RETRY_EXCEEDED    = 4,
    MM_OMA_SESSION_STATE_FAILED_REASON_SESSION_CANCELLED     = 5
} MMOmaSessionStateFailedReason;

#endif /*  _MODEMMANAGER_ENUMS_OMA_H_ */

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

#ifndef _MODEMMANAGER_ENUMS_VOICE_H_
#define _MODEMMANAGER_ENUMS_VOICE_H_

#if !defined (__MODEM_MANAGER_H_INSIDE__)
#error "Only <ModemManager.h> can be included directly."
#endif

/**
 * SECTION:mm-enums-voice
 * @short_description: Common enumerations and types in the API.
 *
 * This section defines enumerations and types that are used in the
 * ModemManager Voice interface.
 **/

/**
 * MMCallState:
 * @MM_CALL_STATE_UNKNOWN: default state for a new outgoing call.
 * @MM_CALL_STATE_DIALING: outgoing call started. Wait for free channel.
 * @MM_CALL_STATE_RINGING_IN: outgoing call attached to GSM network, waiting for an answer.
 * @MM_CALL_STATE_RINGING_OUT: incoming call is waiting for an answer.
 * @MM_CALL_STATE_ACTIVE: call is active between two peers.
 * @MM_CALL_STATE_HELD: held call (by +CHLD AT command).
 * @MM_CALL_STATE_WAITING: waiting call (by +CCWA AT command).
 * @MM_CALL_STATE_TERMINATED: call is terminated.
 *
 * State of Call
 */
typedef enum { /*< underscore_name=mm_call_state >*/
    MM_CALL_STATE_UNKNOWN       = 0,
    MM_CALL_STATE_DIALING       = 1,
    MM_CALL_STATE_RINGING_OUT   = 2,
    MM_CALL_STATE_RINGING_IN    = 3,
    MM_CALL_STATE_ACTIVE        = 4,
    MM_CALL_STATE_HELD          = 5,
    MM_CALL_STATE_WAITING       = 6,
    MM_CALL_STATE_TERMINATED    = 7
} MMCallState;

/**
 * MMCallStateReason:
 * @MM_CALL_STATE_REASON_UNKNOWN: Default value for a new outgoing call.
 * @MM_CALL_STATE_REASON_OUTGOING_STARTED: Outgoing call is started.
 * @MM_CALL_STATE_REASON_INCOMING_NEW: Received a new incoming call.
 * @MM_CALL_STATE_REASON_ACCEPTED: Dialing or Ringing call is accepted.
 * @MM_CALL_STATE_REASON_TERMINATED: Call is correctly terminated.
 * @MM_CALL_STATE_REASON_REFUSED_OR_BUSY: Remote peer is busy or refused call
 * @MM_CALL_STATE_REASON_ERROR: Wrong number or generic network error.
 */
typedef enum { /*< underscore_name=mm_call_state_reason >*/
    MM_CALL_STATE_REASON_UNKNOWN            = 0,
    MM_CALL_STATE_REASON_OUTGOING_STARTED   = 1,
    MM_CALL_STATE_REASON_INCOMING_NEW       = 2,
    MM_CALL_STATE_REASON_ACCEPTED           = 3,
    MM_CALL_STATE_REASON_TERMINATED         = 4,
    MM_CALL_STATE_REASON_REFUSED_OR_BUSY    = 5,
    MM_CALL_STATE_REASON_ERROR              = 6
} MMCallStateReason;

/**
 * MMCallDirection:
 * @MM_CALL_DIRECTION_UNKNOWN: unknown.
 * @MM_CALL_DIRECTION_INCOMING: call from network.
 * @MM_CALL_DIRECTION_OUTGOING: call to network.
 */
typedef enum { /*< underscore_name=mm_call_direction >*/
    MM_CALL_DIRECTION_UNKNOWN   = 0,
    MM_CALL_DIRECTION_INCOMING  = 1,
    MM_CALL_DIRECTION_OUTGOING  = 2
} MMCallDirection;

#endif /*  _MODEMMANAGER_ENUMS_VOICE_H_ */

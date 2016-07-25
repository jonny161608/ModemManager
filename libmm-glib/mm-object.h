/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * libmm -- Access modem status & information from glib applications
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
 * Copyright (C) 2011 - 2012 Aleksander Morgado <aleksander@gnu.org>
 * Copyright (C) 2012 Google, Inc.
 */

#ifndef _MM_OBJECT_H_
#define _MM_OBJECT_H_

#if !defined (__LIBMM_GLIB_H_INSIDE__) && !defined (LIBMM_GLIB_COMPILATION)
#error "Only <libmm-glib.h> can be included directly."
#endif

#include <ModemManager.h>

#include "mm-gdbus-modem.h"
#include "mm-modem.h"
#include "mm-modem-3gpp.h"
#if MM_INTERFACE_3GPP_USSD_SUPPORTED
# include "mm-modem-3gpp-ussd.h"
#endif
#include "mm-modem-cdma.h"
#include "mm-modem-simple.h"
#if MM_INTERFACE_LOCATION_SUPPORTED
# include "mm-modem-location.h"
#endif
#if MM_INTERFACE_MESSAGING_SUPPORTED
# include "mm-modem-messaging.h"
#endif
#if MM_INTERFACE_TIME_SUPPORTED
# include "mm-modem-time.h"
#endif
#if MM_INTERFACE_VOICE_SUPPORTED
# include "mm-modem-voice.h"
#endif
#if MM_INTERFACE_FIRMWARE_SUPPORTED
# include "mm-modem-firmware.h"
#endif
#if MM_INTERFACE_SIGNAL_SUPPORTED
# include "mm-modem-signal.h"
#endif
#if MM_INTERFACE_OMA_SUPPORTED
# include "mm-modem-oma.h"
#endif

G_BEGIN_DECLS

#define MM_TYPE_OBJECT            (mm_object_get_type ())
#define MM_OBJECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MM_TYPE_OBJECT, MMObject))
#define MM_OBJECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MM_TYPE_OBJECT, MMObjectClass))
#define MM_IS_OBJECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MM_TYPE_OBJECT))
#define MM_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), MM_TYPE_OBJECT))
#define MM_OBJECT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MM_TYPE_OBJECT, MMObjectClass))

typedef struct _MMObject MMObject;
typedef struct _MMObjectClass MMObjectClass;

/**
 * MMObject:
 *
 * The #MMObject structure contains private data and should only be accessed
 * using the provided API.
 */
struct _MMObject {
    /*< private >*/
    MmGdbusObjectProxy parent;
    gpointer unused;
};

struct _MMObjectClass {
    /*< private >*/
    MmGdbusObjectProxyClass parent;
};

GType mm_object_get_type (void);

const gchar *mm_object_get_path (MMObject *self);
gchar       *mm_object_dup_path (MMObject *self);

MMModem          *mm_object_get_modem            (MMObject *self);
MMModem3gpp      *mm_object_get_modem_3gpp       (MMObject *self);
MMModemCdma      *mm_object_get_modem_cdma       (MMObject *self);
MMModemSimple    *mm_object_get_modem_simple     (MMObject *self);

MMModem          *mm_object_peek_modem           (MMObject *self);
MMModem3gpp      *mm_object_peek_modem_3gpp      (MMObject *self);
MMModemCdma      *mm_object_peek_modem_cdma      (MMObject *self);
MMModemSimple    *mm_object_peek_modem_simple    (MMObject *self);

#if MM_INTERFACE_3GPP_USSD_SUPPORTED
MMModem3gppUssd  *mm_object_get_modem_3gpp_ussd  (MMObject *self);
MMModem3gppUssd  *mm_object_peek_modem_3gpp_ussd (MMObject *self);
#endif

#if MM_INTERFACE_LOCATION_SUPPORTED
MMModemLocation  *mm_object_get_modem_location   (MMObject *self);
MMModemLocation  *mm_object_peek_modem_location  (MMObject *self);
#endif

#if MM_INTERFACE_MESSAGING_SUPPORTED
MMModemMessaging *mm_object_get_modem_messaging  (MMObject *self);
MMModemMessaging *mm_object_peek_modem_messaging (MMObject *self);
#endif

#if MM_INTERFACE_TIME_SUPPORTED
MMModemTime      *mm_object_get_modem_time       (MMObject *self);
MMModemTime      *mm_object_peek_modem_time      (MMObject *self);
#endif

#if MM_INTERFACE_VOICE_SUPPORTED
MMModemVoice     *mm_object_get_modem_voice      (MMObject *self);
MMModemVoice     *mm_object_peek_modem_voice     (MMObject *self);
#endif

#if MM_INTERFACE_SIGNAL_SUPPORTED
MMModemSignal    *mm_object_get_modem_signal     (MMObject *self);
MMModemSignal    *mm_object_peek_modem_signal    (MMObject *self);
#endif

#if MM_INTERFACE_OMA_SUPPORTED
MMModemOma       *mm_object_get_modem_oma        (MMObject *self);
MMModemOma       *mm_object_peek_modem_oma       (MMObject *self);
#endif

#if MM_INTERFACE_FIRMWARE_SUPPORTED
MMModemFirmware  *mm_object_get_modem_firmware   (MMObject *self);
MMModemFirmware  *mm_object_peek_modem_firmware  (MMObject *self);
#endif

G_END_DECLS

#endif /* _MM_OBJECT_H_ */

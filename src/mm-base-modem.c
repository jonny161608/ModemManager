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
 * Copyright (C) 2008 - 2009 Novell, Inc.
 * Copyright (C) 2009 - 2011 Red Hat, Inc.
 * Copyright (C) 2011 Google, Inc.
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gudev/gudev.h>

#include <ModemManager.h>
#include <mm-errors-types.h>
#include <mm-gdbus-modem.h>

#include "mm-context.h"
#include "mm-base-modem.h"

#include "mm-log.h"
#include "mm-port-enums-types.h"
#include "mm-serial-parsers.h"
#include "mm-modem-helpers.h"
#include "mm-bearer-list.h"
#include "mm-sms-list.h"
#include "mm-call-list.h"

#include "mm-iface-modem.h"
#include "mm-iface-modem-3gpp.h"
#include "mm-iface-modem-3gpp-ussd.h"
#include "mm-iface-modem-cdma.h"
#include "mm-iface-modem-simple.h"
#include "mm-iface-modem-location.h"
#include "mm-iface-modem-messaging.h"
#include "mm-iface-modem-voice.h"
#include "mm-iface-modem-time.h"
#include "mm-iface-modem-firmware.h"
#include "mm-iface-modem-signal.h"
#include "mm-iface-modem-oma.h"

static void iface_modem_init           (MMIfaceModem          *iface) { };
static void iface_modem_3gpp_init      (MMIfaceModem3gpp      *iface) { };
static void iface_modem_3gpp_ussd_init (MMIfaceModem3gppUssd  *iface) { };
static void iface_modem_cdma_init      (MMIfaceModemCdma      *iface) { };
static void iface_modem_simple_init    (MMIfaceModemSimple    *iface) { };
static void iface_modem_location_init  (MMIfaceModemLocation  *iface) { };
static void iface_modem_messaging_init (MMIfaceModemMessaging *iface) { };
static void iface_modem_voice_init     (MMIfaceModemVoice     *iface) { };
static void iface_modem_time_init      (MMIfaceModemTime      *iface) { };
static void iface_modem_signal_init    (MMIfaceModemSignal    *iface) { };
static void iface_modem_oma_init       (MMIfaceModemOma       *iface) { };
static void iface_modem_firmware_init  (MMIfaceModemFirmware  *iface) { };

G_DEFINE_ABSTRACT_TYPE_WITH_CODE (
    MMBaseModem, mm_base_modem, MM_GDBUS_TYPE_OBJECT_SKELETON,
    G_IMPLEMENT_INTERFACE (MM_TYPE_IFACE_MODEM,           iface_modem_init)
    G_IMPLEMENT_INTERFACE (MM_TYPE_IFACE_MODEM_3GPP,      iface_modem_3gpp_init)
    G_IMPLEMENT_INTERFACE (MM_TYPE_IFACE_MODEM_3GPP_USSD, iface_modem_3gpp_ussd_init)
    G_IMPLEMENT_INTERFACE (MM_TYPE_IFACE_MODEM_CDMA,      iface_modem_cdma_init)
    G_IMPLEMENT_INTERFACE (MM_TYPE_IFACE_MODEM_SIMPLE,    iface_modem_simple_init)
    G_IMPLEMENT_INTERFACE (MM_TYPE_IFACE_MODEM_LOCATION,  iface_modem_location_init)
    G_IMPLEMENT_INTERFACE (MM_TYPE_IFACE_MODEM_MESSAGING, iface_modem_messaging_init)
    G_IMPLEMENT_INTERFACE (MM_TYPE_IFACE_MODEM_VOICE,     iface_modem_voice_init)
    G_IMPLEMENT_INTERFACE (MM_TYPE_IFACE_MODEM_TIME,      iface_modem_time_init)
    G_IMPLEMENT_INTERFACE (MM_TYPE_IFACE_MODEM_SIGNAL,    iface_modem_signal_init)
    G_IMPLEMENT_INTERFACE (MM_TYPE_IFACE_MODEM_OMA,       iface_modem_oma_init)
    G_IMPLEMENT_INTERFACE (MM_TYPE_IFACE_MODEM_FIRMWARE,  iface_modem_firmware_init))

enum {
    PROP_0,
    PROP_VALID,
    PROP_MAX_TIMEOUTS,
    PROP_DEVICE,
    PROP_DRIVERS,
    PROP_PLUGIN,
    PROP_VENDOR_ID,
    PROP_PRODUCT_ID,
    PROP_CONNECTION,
    PROP_MODEM_DBUS_SKELETON,
    PROP_MODEM_3GPP_DBUS_SKELETON,
    PROP_MODEM_3GPP_USSD_DBUS_SKELETON,
    PROP_MODEM_CDMA_DBUS_SKELETON,
    PROP_MODEM_SIMPLE_DBUS_SKELETON,
    PROP_MODEM_LOCATION_DBUS_SKELETON,
    PROP_MODEM_MESSAGING_DBUS_SKELETON,
    PROP_MODEM_VOICE_DBUS_SKELETON,
    PROP_MODEM_TIME_DBUS_SKELETON,
    PROP_MODEM_SIGNAL_DBUS_SKELETON,
    PROP_MODEM_OMA_DBUS_SKELETON,
    PROP_MODEM_FIRMWARE_DBUS_SKELETON,
    PROP_MODEM_SIM,
    PROP_MODEM_BEARER_LIST,
    PROP_MODEM_STATE,
    PROP_MODEM_3GPP_REGISTRATION_STATE,
    PROP_MODEM_3GPP_CS_NETWORK_SUPPORTED,
    PROP_MODEM_3GPP_PS_NETWORK_SUPPORTED,
    PROP_MODEM_3GPP_EPS_NETWORK_SUPPORTED,
    PROP_MODEM_3GPP_IGNORED_FACILITY_LOCKS,
    PROP_MODEM_CDMA_CDMA1X_REGISTRATION_STATE,
    PROP_MODEM_CDMA_EVDO_REGISTRATION_STATE,
    PROP_MODEM_CDMA_CDMA1X_NETWORK_SUPPORTED,
    PROP_MODEM_CDMA_EVDO_NETWORK_SUPPORTED,
    PROP_MODEM_MESSAGING_SMS_LIST,
    PROP_MODEM_MESSAGING_SMS_PDU_MODE,
    PROP_MODEM_MESSAGING_SMS_DEFAULT_STORAGE,
    PROP_MODEM_VOICE_CALL_LIST,
    PROP_MODEM_SIMPLE_STATUS,
    PROP_LAST
};

static GParamSpec *properties[PROP_LAST];

struct _MMBaseModemPrivate {
    /* The connection to the system bus */
    GDBusConnection *connection;

    /* Modem-wide cancellable. If it ever gets cancelled, no further operations
     * should be done by the modem. */
    GCancellable *cancellable;
    gulong invalid_if_cancelled;

    gchar *device;
    gchar **drivers;
    gchar *plugin;

    guint vendor_id;
    guint product_id;

    gboolean hotplugged;
    gboolean valid;

    guint max_timeouts;

    /* The authorization provider */
    MMAuthProvider *authp;
    GCancellable *authp_cancellable;

    GHashTable *ports;
    MMPortSerialAt *primary;
    MMPortSerialAt *secondary;
    MMPortSerialQcdm *qcdm;
    GList *data;

    /* GPS-enabled modems will have an AT port for control, and a raw serial
     * port to receive all GPS traces */
    MMPortSerialAt *gps_control;
    MMPortSerialGps *gps;

#if defined WITH_QMI
    /* QMI ports */
    GList *qmi;
#endif

#if defined WITH_MBIM
    /* MBIM ports */
    GList *mbim;
#endif

    /*<--- Modem interface --->*/
    GObject *modem_dbus_skeleton;
    MMBaseSim *modem_sim;
    MMBearerList *modem_bearer_list;
    MMModemState modem_state;

    /*<--- Modem 3GPP interface --->*/
    GObject *modem_3gpp_dbus_skeleton;
    MMModem3gppRegistrationState modem_3gpp_registration_state;
    gboolean modem_3gpp_cs_network_supported;
    gboolean modem_3gpp_ps_network_supported;
    gboolean modem_3gpp_eps_network_supported;
    MMModem3gppFacility modem_3gpp_ignored_facility_locks;

    /*<--- Modem 3GPP USSD interface --->*/
    GObject *modem_3gpp_ussd_dbus_skeleton;

    /*<--- Modem CDMA interface --->*/
    GObject *modem_cdma_dbus_skeleton;
    MMModemCdmaRegistrationState modem_cdma_cdma1x_registration_state;
    MMModemCdmaRegistrationState modem_cdma_evdo_registration_state;
    gboolean modem_cdma_cdma1x_network_supported;
    gboolean modem_cdma_evdo_network_supported;

    /*<--- Modem Simple interface --->*/
    GObject *modem_simple_dbus_skeleton;
    MMSimpleStatus *modem_simple_status;

    /*<--- Modem Location interface --->*/
    GObject *modem_location_dbus_skeleton;

    /*<--- Modem Messaging interface --->*/
    GObject *modem_messaging_dbus_skeleton;
    MMSmsList *modem_messaging_sms_list;
    gboolean modem_messaging_sms_pdu_mode;
    MMSmsStorage modem_messaging_sms_default_storage;

    /*<--- Modem Voice interface --->*/
    GObject *modem_voice_dbus_skeleton;
    MMCallList *modem_voice_call_list;

    /*<--- Modem Time interface --->*/
    GObject *modem_time_dbus_skeleton;

    /*<--- Modem Signal interface --->*/
    GObject *modem_signal_dbus_skeleton;

    /*<--- Modem OMA interface --->*/
    GObject *modem_oma_dbus_skeleton;

    /*<--- Modem Firmware interface --->*/
    GObject *modem_firmware_dbus_skeleton;
};

static gchar *
get_hash_key (const gchar *subsys,
              const gchar *name)
{
    return g_strdup_printf ("%s%s", subsys, name);
}

MMPort *
mm_base_modem_get_port (MMBaseModem *self,
                        const gchar *subsys,
                        const gchar *name)
{
    MMPort *port;
    gchar *key;

    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);
    g_return_val_if_fail (name != NULL, NULL);
    g_return_val_if_fail (subsys != NULL, NULL);

    /* Only 'net' or 'tty' should be given */
    g_return_val_if_fail (g_str_equal (subsys, "net") ||
                          g_str_equal (subsys, "tty"),
                          NULL);

    key = get_hash_key (subsys, name);
    port = g_hash_table_lookup (self->priv->ports, key);
    g_free (key);

    return port;
}

gboolean
mm_base_modem_owns_port (MMBaseModem *self,
                         const gchar *subsys,
                         const gchar *name)
{
    return !!mm_base_modem_get_port (self, subsys, name);
}

static void
serial_port_timed_out_cb (MMPortSerial *port,
                          guint n_consecutive_timeouts,
                          gpointer user_data)
{
    MMBaseModem *self = (MM_BASE_MODEM (user_data));

    if (self->priv->max_timeouts > 0 &&
        n_consecutive_timeouts >= self->priv->max_timeouts) {
        mm_warn ("(%s/%s) port timed out %u times, marking modem '%s' as disabled",
                 mm_port_type_get_string (mm_port_get_port_type (MM_PORT (port))),
                 mm_port_get_device (MM_PORT (port)),
                 n_consecutive_timeouts,
                 g_dbus_object_get_object_path (G_DBUS_OBJECT (self)));

        /* Only set action to invalidate modem if not already done */
        g_cancellable_cancel (self->priv->cancellable);
    }
}

gboolean
mm_base_modem_grab_port (MMBaseModem *self,
                         const gchar *subsys,
                         const gchar *name,
                         const gchar *parent_path,
                         MMPortType ptype,
                         MMPortSerialAtFlag at_pflags,
                         GError **error)
{
    MMPort *port;
    gchar *key;

    g_return_val_if_fail (MM_IS_BASE_MODEM (self), FALSE);
    g_return_val_if_fail (subsys != NULL, FALSE);
    g_return_val_if_fail (name != NULL, FALSE);

    /* Only allow 'tty', 'net' and 'cdc-wdm' ports */
    if (!g_str_equal (subsys, "net") &&
        !g_str_equal (subsys, "tty") &&
        !(g_str_has_prefix (subsys, "usb") && g_str_has_prefix (name, "cdc-wdm")) &&
        !g_str_equal (subsys, "virtual")) {
        g_set_error (error,
                     MM_CORE_ERROR,
                     MM_CORE_ERROR_UNSUPPORTED,
                     "Cannot add port '%s/%s', unhandled subsystem",
                     subsys,
                     name);
        return FALSE;
    }

    /* Check whether we already have it stored */
    key = get_hash_key (subsys, name);
    port = g_hash_table_lookup (self->priv->ports, key);
    if (port) {
        g_set_error (error,
                     MM_CORE_ERROR,
                     MM_CORE_ERROR_UNSUPPORTED,
                     "Cannot add port '%s/%s', already exists",
                     subsys,
                     name);
        g_free (key);
        return FALSE;
    }

    /* Explicitly ignored ports, grab them but explicitly flag them as ignored
     * right away, all the same way (i.e. regardless of subsystem). */
    if (ptype == MM_PORT_TYPE_IGNORED) {
        port = MM_PORT (g_object_new (MM_TYPE_PORT,
                                      MM_PORT_DEVICE, name,
                                      MM_PORT_TYPE, MM_PORT_TYPE_IGNORED,
                                      NULL));
    }
    /* Serial ports... */
    else if (g_str_equal (subsys, "tty")) {
        if (ptype == MM_PORT_TYPE_QCDM)
            /* QCDM port */
            port = MM_PORT (mm_port_serial_qcdm_new (name));
        else if (ptype == MM_PORT_TYPE_AT) {
            /* AT port */
            port = MM_PORT (mm_port_serial_at_new (name, MM_PORT_SUBSYS_TTY));

            /* Set common response parser */
            mm_port_serial_at_set_response_parser (MM_PORT_SERIAL_AT (port),
                                                   mm_serial_parser_v1_parse,
                                                   mm_serial_parser_v1_new (),
                                                   mm_serial_parser_v1_destroy);
            /* Store flags already */
            mm_port_serial_at_set_flags (MM_PORT_SERIAL_AT (port), at_pflags);
        } else if (ptype == MM_PORT_TYPE_GPS) {
            /* Raw GPS port */
            port = MM_PORT (mm_port_serial_gps_new (name));
        } else {
            g_set_error (error,
                         MM_CORE_ERROR,
                         MM_CORE_ERROR_UNSUPPORTED,
                         "Cannot add port '%s/%s', unhandled serial type",
                         subsys,
                         name);
            g_free (key);
            return FALSE;
        }

        /* For serial ports, enable port timeout checks */
        g_signal_connect (port,
                          "timed-out",
                          G_CALLBACK (serial_port_timed_out_cb),
                          self);
    }
    /* Net ports... */
    else if (g_str_equal (subsys, "net")) {
        port = MM_PORT (g_object_new (MM_TYPE_PORT,
                                      MM_PORT_DEVICE, name,
                                      MM_PORT_SUBSYS, MM_PORT_SUBSYS_NET,
                                      MM_PORT_TYPE, MM_PORT_TYPE_NET,
                                      NULL));
    }
    /* cdc-wdm ports... */
    else if (g_str_has_prefix (subsys, "usb") &&
             g_str_has_prefix (name, "cdc-wdm")) {
#if defined WITH_QMI
        if (ptype == MM_PORT_TYPE_QMI)
            port = MM_PORT (mm_port_qmi_new (name));
#endif
#if defined WITH_MBIM
        if (!port && ptype == MM_PORT_TYPE_MBIM)
            port = MM_PORT (mm_port_mbim_new (name));
#endif

        /* Non-serial AT port */
        if (!port && ptype == MM_PORT_TYPE_AT) {
            port = MM_PORT (mm_port_serial_at_new (name, MM_PORT_SUBSYS_USB));

            /* Set common response parser */
            mm_port_serial_at_set_response_parser (MM_PORT_SERIAL_AT (port),
                                                   mm_serial_parser_v1_parse,
                                                   mm_serial_parser_v1_new (),
                                                   mm_serial_parser_v1_destroy);
            /* Store flags already */
            mm_port_serial_at_set_flags (MM_PORT_SERIAL_AT (port), at_pflags);
        }

        if (!port) {
            g_set_error (error,
                         MM_CORE_ERROR,
                         MM_CORE_ERROR_UNSUPPORTED,
                         "Cannot add port '%s/%s', unsupported",
                         subsys,
                         name);
            g_free (key);
            return FALSE;
        }
    }
    /* Virtual ports... */
    else if (g_str_equal (subsys, "virtual")) {
        port = MM_PORT (mm_port_serial_at_new (name, MM_PORT_SUBSYS_UNIX));

        /* Set common response parser */
        mm_port_serial_at_set_response_parser (MM_PORT_SERIAL_AT (port),
                                               mm_serial_parser_v1_parse,
                                               mm_serial_parser_v1_new (),
                                               mm_serial_parser_v1_destroy);
        /* Store flags already */
        mm_port_serial_at_set_flags (MM_PORT_SERIAL_AT (port), at_pflags);
    }
    else
        /* We already filter out before all non-tty, non-net, non-cdc-wdm ports */
        g_assert_not_reached();

    mm_dbg ("(%s) type '%s' claimed by %s",
            name,
            mm_port_type_get_string (ptype),
            mm_base_modem_get_device (self));

    /* Add it to the tracking HT.
     * Note: 'key' and 'port' now owned by the HT. */
    g_hash_table_insert (self->priv->ports, key, port);

    /* Store parent path */
    g_object_set (port,
                  MM_PORT_PARENT_PATH, parent_path,
                  NULL);

    return TRUE;
}

void
mm_base_modem_release_port (MMBaseModem *self,
                            const gchar *subsys,
                            const gchar *name)
{
    gchar *key;
    MMPort *port;
    GList *l;

    g_return_if_fail (MM_IS_BASE_MODEM (self));
    g_return_if_fail (name != NULL);
    g_return_if_fail (subsys != NULL);

    if (!g_str_equal (subsys, "tty") &&
        !g_str_equal (subsys, "net") &&
        !(g_str_has_prefix (subsys, "usb") && g_str_has_prefix (name, "cdc-wdm")) &&
        !g_str_equal (subsys, "virtual"))
        return;

    key = get_hash_key (subsys, name);

    /* Find the port */
    port = g_hash_table_lookup (self->priv->ports, key);
    if (!port) {
        mm_warn ("(%s/%s): cannot release port, not found",
                 subsys, name);
        g_free (key);
        return;
    }

    if (port == (MMPort *)self->priv->primary) {
        /* Cancel modem-wide cancellable; no further actions can be done
         * without a primary port. */
        g_cancellable_cancel (self->priv->cancellable);

        g_clear_object (&self->priv->primary);
    }

    l = g_list_find (self->priv->data, port);
    if (l) {
        g_object_unref (l->data);
        self->priv->data = g_list_delete_link (self->priv->data, l);
    }

    if (port == (MMPort *)self->priv->secondary)
        g_clear_object (&self->priv->secondary);

    if (port == (MMPort *)self->priv->qcdm)
        g_clear_object (&self->priv->qcdm);

    if (port == (MMPort *)self->priv->gps_control)
        g_clear_object (&self->priv->gps_control);

    if (port == (MMPort *)self->priv->gps)
        g_clear_object (&self->priv->gps);

#if defined WITH_QMI
    l = g_list_find (self->priv->qmi, port);
    if (l) {
        g_object_unref (l->data);
        self->priv->qmi = g_list_delete_link (self->priv->qmi, l);
    }
#endif

#if defined WITH_MBIM
    l = g_list_find (self->priv->mbim, port);
    if (l) {
        g_object_unref (l->data);
        self->priv->mbim = g_list_delete_link (self->priv->mbim, l);
    }
#endif

    /* Remove it from the tracking HT */
    mm_dbg ("(%s/%s) type %s released from %s",
            subsys,
            name,
            mm_port_type_get_string (mm_port_get_port_type (port)),
            mm_port_get_device (port));
    g_hash_table_remove (self->priv->ports, key);
    g_free (key);
}

/*****************************************************************************/
/* First registration checks */

static void
modem_3gpp_run_registration_checks_ready (MMIfaceModem3gpp *self,
                                          GAsyncResult *res)
{
    GError *error = NULL;

    if (!mm_iface_modem_3gpp_run_registration_checks_finish (self, res, &error)) {
        mm_warn ("Initial 3GPP registration check failed: %s", error->message);
        g_error_free (error);
        return;
    }
    mm_dbg ("Initial 3GPP registration checks finished");
}

static void
modem_cdma_run_registration_checks_ready (MMIfaceModemCdma *self,
                                          GAsyncResult *res)
{
    GError *error = NULL;

    if (!mm_iface_modem_cdma_run_registration_checks_finish (self, res, &error)) {
        mm_warn ("Initial CDMA registration check failed: %s", error->message);
        g_error_free (error);
        return;
    }
    mm_dbg ("Initial CDMA registration checks finished");
}

static gboolean
schedule_initial_registration_checks_cb (MMBaseModem *self)
{
    if (mm_iface_modem_is_3gpp (MM_IFACE_MODEM (self)))
        mm_iface_modem_3gpp_run_registration_checks (MM_IFACE_MODEM_3GPP (self),
                                                     (GAsyncReadyCallback) modem_3gpp_run_registration_checks_ready,
                                                     NULL);
    if (mm_iface_modem_is_cdma (MM_IFACE_MODEM (self)))
        mm_iface_modem_cdma_run_registration_checks (MM_IFACE_MODEM_CDMA (self),
                                                     (GAsyncReadyCallback) modem_cdma_run_registration_checks_ready,
                                                     NULL);
    /* We got a full reference, so balance it out here */
    g_object_unref (self);
    return G_SOURCE_REMOVE;
}

static void
schedule_initial_registration_checks (MMBaseModem *self)
{
    g_idle_add ((GSourceFunc) schedule_initial_registration_checks_cb, g_object_ref (self));
}

/*****************************************************************************/

typedef enum {
    DISABLING_STEP_FIRST,
    DISABLING_STEP_WAIT_FOR_FINAL_STATE,
    DISABLING_STEP_DISCONNECT_BEARERS,
    DISABLING_STEP_IFACE_SIMPLE,
    DISABLING_STEP_IFACE_FIRMWARE,
    DISABLING_STEP_IFACE_SIGNAL,
    DISABLING_STEP_IFACE_OMA,
    DISABLING_STEP_IFACE_TIME,
    DISABLING_STEP_IFACE_MESSAGING,
    DISABLING_STEP_IFACE_VOICE,
    DISABLING_STEP_IFACE_LOCATION,
    DISABLING_STEP_IFACE_CONTACTS,
    DISABLING_STEP_IFACE_CDMA,
    DISABLING_STEP_IFACE_3GPP_USSD,
    DISABLING_STEP_IFACE_3GPP,
    DISABLING_STEP_IFACE_MODEM,
    DISABLING_STEP_LAST,
} DisablingStep;

typedef struct {
    MMBaseModem *self;
    GCancellable *cancellable;
    GSimpleAsyncResult *result;
    DisablingStep step;
    MMModemState previous_state;
    gboolean disabled;
} DisablingContext;

static void disabling_step (DisablingContext *ctx);

static void
disabling_context_complete_and_free (DisablingContext *ctx)
{
    GError *error = NULL;

    g_simple_async_result_complete_in_idle (ctx->result);

    if (MM_BASE_MODEM_GET_CLASS (ctx->self)->disabling_stopped &&
        !MM_BASE_MODEM_GET_CLASS (ctx->self)->disabling_stopped (ctx->self, &error)) {
        mm_warn ("Error when stopping the disabling sequence: %s", error->message);
        g_error_free (error);
    }

    if (ctx->disabled)
        mm_iface_modem_update_state (MM_IFACE_MODEM (ctx->self),
                                     MM_MODEM_STATE_DISABLED,
                                     MM_MODEM_STATE_CHANGE_REASON_USER_REQUESTED);
    else if (ctx->previous_state != MM_MODEM_STATE_DISABLED) {
        /* Fallback to previous state */
        mm_iface_modem_update_state (MM_IFACE_MODEM (ctx->self),
                                     ctx->previous_state,
                                     MM_MODEM_STATE_CHANGE_REASON_UNKNOWN);
    }

    g_object_unref (ctx->result);
    if (ctx->cancellable)
        g_object_unref (ctx->cancellable);
    g_object_unref (ctx->self);
    g_free (ctx);
}

static gboolean
disabling_context_complete_and_free_if_cancelled (DisablingContext *ctx)
{
    if (!g_cancellable_is_cancelled (ctx->cancellable))
        return FALSE;

    g_simple_async_result_set_error (ctx->result,
                                     MM_CORE_ERROR,
                                     MM_CORE_ERROR_CANCELLED,
                                     "Disabling cancelled");
    disabling_context_complete_and_free (ctx);
    return TRUE;
}

gboolean
mm_base_modem_disable_finish (MMBaseModem *self,
                              GAsyncResult *res,
                              GError **error)
{
    return !g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error);
}

#undef INTERFACE_DISABLE_READY_FN
#define INTERFACE_DISABLE_READY_FN(NAME,TYPE,FATAL_ERRORS)              \
    static void                                                         \
    NAME##_disable_ready (MMBaseModem *self,                            \
                          GAsyncResult *result,                         \
                          DisablingContext *ctx)                        \
    {                                                                   \
        GError *error = NULL;                                           \
                                                                        \
        if (!mm_##NAME##_disable_finish (TYPE (self),                   \
                                         result,                        \
                                         &error)) {                     \
            if (FATAL_ERRORS) {                                         \
                g_simple_async_result_take_error (ctx->result, error);  \
                disabling_context_complete_and_free (ctx);              \
                return;                                                 \
            }                                                           \
                                                                        \
            mm_dbg ("Couldn't disable interface: '%s'",                 \
                    error->message);                                    \
            g_error_free (error);                                       \
            return;                                                     \
        }                                                               \
                                                                        \
        /* Go on to next step */                                        \
        ctx->step++;                                                    \
        disabling_step (ctx);                                           \
    }

INTERFACE_DISABLE_READY_FN (iface_modem,           MM_IFACE_MODEM,           TRUE)
INTERFACE_DISABLE_READY_FN (iface_modem_3gpp,      MM_IFACE_MODEM_3GPP,      TRUE)
INTERFACE_DISABLE_READY_FN (iface_modem_3gpp_ussd, MM_IFACE_MODEM_3GPP_USSD, TRUE)
INTERFACE_DISABLE_READY_FN (iface_modem_cdma,      MM_IFACE_MODEM_CDMA,      TRUE)
INTERFACE_DISABLE_READY_FN (iface_modem_location,  MM_IFACE_MODEM_LOCATION,  FALSE)
INTERFACE_DISABLE_READY_FN (iface_modem_messaging, MM_IFACE_MODEM_MESSAGING, FALSE)
INTERFACE_DISABLE_READY_FN (iface_modem_voice,     MM_IFACE_MODEM_VOICE,     FALSE)
INTERFACE_DISABLE_READY_FN (iface_modem_signal,    MM_IFACE_MODEM_SIGNAL,    FALSE)
INTERFACE_DISABLE_READY_FN (iface_modem_time,      MM_IFACE_MODEM_TIME,      FALSE)
INTERFACE_DISABLE_READY_FN (iface_modem_oma,       MM_IFACE_MODEM_OMA,       FALSE)

static void
bearer_list_disconnect_all_bearers_ready (MMBearerList *list,
                                          GAsyncResult *res,
                                          DisablingContext *ctx)
{
    GError *error = NULL;

    if (!mm_bearer_list_disconnect_all_bearers_finish (list, res, &error)) {
        g_simple_async_result_take_error (ctx->result, error);
        disabling_context_complete_and_free (ctx);
        return;
    }

    /* Go on to next step */
    ctx->step++;
    disabling_step (ctx);
}

static void
disabling_wait_for_final_state_ready (MMIfaceModem *self,
                                      GAsyncResult *res,
                                      DisablingContext *ctx)
{
    GError *error = NULL;

    ctx->previous_state = mm_iface_modem_wait_for_final_state_finish (self, res, &error);
    if (error) {
        g_simple_async_result_take_error (ctx->result, error);
        disabling_context_complete_and_free (ctx);
        return;
    }

    switch (ctx->previous_state) {
    case MM_MODEM_STATE_UNKNOWN:
    case MM_MODEM_STATE_FAILED:
    case MM_MODEM_STATE_LOCKED:
    case MM_MODEM_STATE_DISABLED:
        /* Just return success, don't relaunch disabling.
         * Note that we do consider here UNKNOWN and FAILED status on purpose,
         * as the MMManager will try to disable every modem before removing
         * it. */
        g_simple_async_result_set_op_res_gboolean (ctx->result, TRUE);
        disabling_context_complete_and_free (ctx);
        return;
    default:
        break;
    }

    /* We're in a final state now, go on */

    mm_iface_modem_update_state (MM_IFACE_MODEM (ctx->self),
                                 MM_MODEM_STATE_DISABLING,
                                 MM_MODEM_STATE_CHANGE_REASON_USER_REQUESTED);

    ctx->step++;
    disabling_step (ctx);
}

static void
disabling_step (DisablingContext *ctx)
{
    /* Don't run new steps if we're cancelled */
    if (disabling_context_complete_and_free_if_cancelled (ctx))
        return;

    switch (ctx->step) {
    case DISABLING_STEP_FIRST:
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_WAIT_FOR_FINAL_STATE:
        mm_iface_modem_wait_for_final_state (MM_IFACE_MODEM (ctx->self),
                                             MM_MODEM_STATE_UNKNOWN, /* just any */
                                             (GAsyncReadyCallback)disabling_wait_for_final_state_ready,
                                             ctx);
        return;

    case DISABLING_STEP_DISCONNECT_BEARERS:
        if (ctx->self->priv->modem_bearer_list) {
            mm_bearer_list_disconnect_all_bearers (
                ctx->self->priv->modem_bearer_list,
                (GAsyncReadyCallback)bearer_list_disconnect_all_bearers_ready,
                ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_IFACE_SIMPLE:
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_IFACE_FIRMWARE:
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_IFACE_SIGNAL:
        if (ctx->self->priv->modem_signal_dbus_skeleton) {
            mm_dbg ("Modem has extended signal reporting capabilities, disabling the Signal interface...");
            /* Disabling the Modem Signal interface */
            mm_iface_modem_signal_disable (MM_IFACE_MODEM_SIGNAL (ctx->self),
                                           (GAsyncReadyCallback)iface_modem_signal_disable_ready,
                                           ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_IFACE_OMA:
        if (ctx->self->priv->modem_oma_dbus_skeleton) {
            mm_dbg ("Modem has OMA capabilities, disabling the OMA interface...");
            /* Disabling the Modem Oma interface */
            mm_iface_modem_oma_disable (MM_IFACE_MODEM_OMA (ctx->self),
                                        (GAsyncReadyCallback)iface_modem_oma_disable_ready,
                                        ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_IFACE_TIME:
        if (ctx->self->priv->modem_time_dbus_skeleton) {
            mm_dbg ("Modem has time capabilities, disabling the Time interface...");
            /* Disabling the Modem Time interface */
            mm_iface_modem_time_disable (MM_IFACE_MODEM_TIME (ctx->self),
                                         (GAsyncReadyCallback)iface_modem_time_disable_ready,
                                         ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_IFACE_MESSAGING:
        if (ctx->self->priv->modem_messaging_dbus_skeleton) {
            mm_dbg ("Modem has messaging capabilities, disabling the Messaging interface...");
            /* Disabling the Modem Messaging interface */
            mm_iface_modem_messaging_disable (MM_IFACE_MODEM_MESSAGING (ctx->self),
                                              (GAsyncReadyCallback)iface_modem_messaging_disable_ready,
                                              ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_IFACE_VOICE:
        if (ctx->self->priv->modem_voice_dbus_skeleton) {
            mm_dbg ("Modem has voice capabilities, disabling the Voice interface...");
            /* Disabling the Modem Voice interface */
            mm_iface_modem_voice_disable (MM_IFACE_MODEM_VOICE (ctx->self),
                                          (GAsyncReadyCallback)iface_modem_voice_disable_ready,
                                          ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_IFACE_LOCATION:
        if (ctx->self->priv->modem_location_dbus_skeleton) {
            mm_dbg ("Modem has location capabilities, disabling the Location interface...");
            /* Disabling the Modem Location interface */
            mm_iface_modem_location_disable (MM_IFACE_MODEM_LOCATION (ctx->self),
                                             (GAsyncReadyCallback)iface_modem_location_disable_ready,
                                             ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_IFACE_CONTACTS:
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_IFACE_CDMA:
        if (ctx->self->priv->modem_cdma_dbus_skeleton) {
            mm_dbg ("Modem has CDMA capabilities, disabling the Modem CDMA interface...");
            /* Disabling the Modem CDMA interface */
            mm_iface_modem_cdma_disable (MM_IFACE_MODEM_CDMA (ctx->self),
                                        (GAsyncReadyCallback)iface_modem_cdma_disable_ready,
                                        ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_IFACE_3GPP_USSD:
        if (ctx->self->priv->modem_3gpp_ussd_dbus_skeleton) {
            mm_dbg ("Modem has 3GPP/USSD capabilities, disabling the Modem 3GPP/USSD interface...");
            /* Disabling the Modem 3GPP USSD interface */
            mm_iface_modem_3gpp_ussd_disable (MM_IFACE_MODEM_3GPP_USSD (ctx->self),
                                              (GAsyncReadyCallback)iface_modem_3gpp_ussd_disable_ready,
                                              ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_IFACE_3GPP:
        if (ctx->self->priv->modem_3gpp_dbus_skeleton) {
            mm_dbg ("Modem has 3GPP capabilities, disabling the Modem 3GPP interface...");
            /* Disabling the Modem 3GPP interface */
            mm_iface_modem_3gpp_disable (MM_IFACE_MODEM_3GPP (ctx->self),
                                        (GAsyncReadyCallback)iface_modem_3gpp_disable_ready,
                                        ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_IFACE_MODEM:
        /* This skeleton may be NULL when mm_base_modem_disable() gets called at
         * the same time as modem object disposal. */
        if (ctx->self->priv->modem_dbus_skeleton) {
            /* Disabling the Modem interface */
            mm_iface_modem_disable (MM_IFACE_MODEM (ctx->self),
                                    (GAsyncReadyCallback)iface_modem_disable_ready,
                                    ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case DISABLING_STEP_LAST:
        ctx->disabled = TRUE;
        /* All disabled without errors! */
        g_simple_async_result_set_op_res_gboolean (ctx->result, TRUE);
        disabling_context_complete_and_free (ctx);
        return;
    }

    g_assert_not_reached ();
}

void
mm_base_modem_disable (MMBaseModem *self,
                       GAsyncReadyCallback callback,
                       gpointer user_data)
{
    DisablingContext *ctx;

    ctx = g_new0 (DisablingContext, 1);
    ctx->self = g_object_ref (self);
    ctx->result = g_simple_async_result_new (G_OBJECT (self), callback, user_data, mm_base_modem_disable);
    ctx->cancellable = (self->priv->cancellable ? g_object_ref (self->priv->cancellable) : NULL);
    ctx->step = DISABLING_STEP_FIRST;

    disabling_step (ctx);
}

/*****************************************************************************/

typedef enum {
    ENABLING_STEP_FIRST,
    ENABLING_STEP_WAIT_FOR_FINAL_STATE,
    ENABLING_STEP_STARTED,
    ENABLING_STEP_IFACE_MODEM,
    ENABLING_STEP_IFACE_3GPP,
    ENABLING_STEP_IFACE_3GPP_USSD,
    ENABLING_STEP_IFACE_CDMA,
    ENABLING_STEP_IFACE_CONTACTS,
    ENABLING_STEP_IFACE_LOCATION,
    ENABLING_STEP_IFACE_MESSAGING,
    ENABLING_STEP_IFACE_VOICE,
    ENABLING_STEP_IFACE_TIME,
    ENABLING_STEP_IFACE_SIGNAL,
    ENABLING_STEP_IFACE_OMA,
    ENABLING_STEP_IFACE_FIRMWARE,
    ENABLING_STEP_IFACE_SIMPLE,
    ENABLING_STEP_LAST,
} EnablingStep;

typedef struct {
    MMBaseModem *self;
    GCancellable *cancellable;
    GSimpleAsyncResult *result;
    EnablingStep step;
    MMModemState previous_state;
    gboolean enabled;
} EnablingContext;

static void enabling_step (EnablingContext *ctx);

static void
enabling_context_complete_and_free (EnablingContext *ctx)
{
    g_simple_async_result_complete_in_idle (ctx->result);
    g_object_unref (ctx->result);

    if (ctx->enabled)
        mm_iface_modem_update_state (MM_IFACE_MODEM (ctx->self),
                                     MM_MODEM_STATE_ENABLED,
                                     MM_MODEM_STATE_CHANGE_REASON_USER_REQUESTED);
    else if (ctx->previous_state != MM_MODEM_STATE_ENABLED) {
        /* Fallback to previous state */
        mm_iface_modem_update_state (MM_IFACE_MODEM (ctx->self),
                                     ctx->previous_state,
                                     MM_MODEM_STATE_CHANGE_REASON_UNKNOWN);
    }

    g_object_unref (ctx->cancellable);
    g_object_unref (ctx->self);
    g_free (ctx);
}

static gboolean
enabling_context_complete_and_free_if_cancelled (EnablingContext *ctx)
{
    if (!g_cancellable_is_cancelled (ctx->cancellable))
        return FALSE;

    g_simple_async_result_set_error (ctx->result,
                                     MM_CORE_ERROR,
                                     MM_CORE_ERROR_CANCELLED,
                                     "Enabling cancelled");
    enabling_context_complete_and_free (ctx);
    return TRUE;
}

gboolean
mm_base_modem_enable_finish (MMBaseModem *self,
                             GAsyncResult *res,
                             GError **error)
{
    if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
        return FALSE;

    return TRUE;
}

#undef INTERFACE_ENABLE_READY_FN
#define INTERFACE_ENABLE_READY_FN(NAME,TYPE,FATAL_ERRORS)               \
    static void                                                         \
    NAME##_enable_ready (MMBaseModem *self,                             \
                         GAsyncResult *result,                          \
                         EnablingContext *ctx)                          \
    {                                                                   \
        GError *error = NULL;                                           \
                                                                        \
        if (!mm_##NAME##_enable_finish (TYPE (self),                    \
                                        result,                         \
                                        &error)) {                      \
            if (FATAL_ERRORS) {                                         \
                g_simple_async_result_take_error (ctx->result, error);  \
                enabling_context_complete_and_free (ctx);               \
                return;                                                 \
            }                                                           \
                                                                        \
            mm_dbg ("Couldn't enable interface: '%s'",                  \
                    error->message);                                    \
            g_error_free (error);                                       \
        }                                                               \
                                                                        \
        /* Go on to next step */                                        \
        ctx->step++;                                                    \
        enabling_step (ctx);                                            \
    }

INTERFACE_ENABLE_READY_FN (iface_modem,           MM_IFACE_MODEM,           TRUE)
INTERFACE_ENABLE_READY_FN (iface_modem_3gpp,      MM_IFACE_MODEM_3GPP,      TRUE)
INTERFACE_ENABLE_READY_FN (iface_modem_3gpp_ussd, MM_IFACE_MODEM_3GPP_USSD, TRUE)
INTERFACE_ENABLE_READY_FN (iface_modem_cdma,      MM_IFACE_MODEM_CDMA,      TRUE)
INTERFACE_ENABLE_READY_FN (iface_modem_location,  MM_IFACE_MODEM_LOCATION,  FALSE)
INTERFACE_ENABLE_READY_FN (iface_modem_messaging, MM_IFACE_MODEM_MESSAGING, FALSE)
INTERFACE_ENABLE_READY_FN (iface_modem_voice,     MM_IFACE_MODEM_VOICE,     FALSE)
INTERFACE_ENABLE_READY_FN (iface_modem_signal,    MM_IFACE_MODEM_SIGNAL,    FALSE)
INTERFACE_ENABLE_READY_FN (iface_modem_time,      MM_IFACE_MODEM_TIME,      FALSE)
INTERFACE_ENABLE_READY_FN (iface_modem_oma,       MM_IFACE_MODEM_OMA,       FALSE)

static void
enabling_started_ready (MMBaseModem *self,
                        GAsyncResult *result,
                        EnablingContext *ctx)
{
    GError *error = NULL;

    if (!MM_BASE_MODEM_GET_CLASS (self)->enabling_started_finish (self, result, &error)) {
        g_simple_async_result_take_error (ctx->result, error);
        enabling_context_complete_and_free (ctx);
        return;
    }

    /* Go on to next step */
    ctx->step++;
    enabling_step (ctx);
}

static void
enabling_wait_for_final_state_ready (MMIfaceModem *self,
                                     GAsyncResult *res,
                                     EnablingContext *ctx)
{
    GError *error = NULL;

    ctx->previous_state = mm_iface_modem_wait_for_final_state_finish (self, res, &error);
    if (error) {
        g_simple_async_result_take_error (ctx->result, error);
        enabling_context_complete_and_free (ctx);
        return;
    }

    if (ctx->previous_state >= MM_MODEM_STATE_ENABLED) {
        /* Just return success, don't relaunch enabling */
        g_simple_async_result_set_op_res_gboolean (ctx->result, TRUE);
        enabling_context_complete_and_free (ctx);
        return;
    }

    /* We're in a final state now, go on */

    mm_iface_modem_update_state (MM_IFACE_MODEM (ctx->self),
                                 MM_MODEM_STATE_ENABLING,
                                 MM_MODEM_STATE_CHANGE_REASON_USER_REQUESTED);

    ctx->step++;
    enabling_step (ctx);
}

static void
enabling_step (EnablingContext *ctx)
{
    /* Don't run new steps if we're cancelled */
    if (enabling_context_complete_and_free_if_cancelled (ctx))
        return;

    switch (ctx->step) {
    case ENABLING_STEP_FIRST:
        /* Fall down to next step */
        ctx->step++;

    case ENABLING_STEP_WAIT_FOR_FINAL_STATE:
        mm_iface_modem_wait_for_final_state (MM_IFACE_MODEM (ctx->self),
                                             MM_MODEM_STATE_UNKNOWN, /* just any */
                                             (GAsyncReadyCallback)enabling_wait_for_final_state_ready,
                                             ctx);
        return;

    case ENABLING_STEP_STARTED:
        if (MM_BASE_MODEM_GET_CLASS (ctx->self)->enabling_started &&
            MM_BASE_MODEM_GET_CLASS (ctx->self)->enabling_started_finish) {
            MM_BASE_MODEM_GET_CLASS (ctx->self)->enabling_started (ctx->self,
                                                                        (GAsyncReadyCallback)enabling_started_ready,
                                                                        ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case ENABLING_STEP_IFACE_MODEM:
        g_assert (ctx->self->priv->modem_dbus_skeleton != NULL);
        /* Enabling the Modem interface */
        mm_iface_modem_enable (MM_IFACE_MODEM (ctx->self),
                               ctx->cancellable,
                               (GAsyncReadyCallback)iface_modem_enable_ready,
                               ctx);
        return;

    case ENABLING_STEP_IFACE_3GPP:
        if (ctx->self->priv->modem_3gpp_dbus_skeleton) {
            mm_dbg ("Modem has 3GPP capabilities, enabling the Modem 3GPP interface...");
            /* Enabling the Modem 3GPP interface */
            mm_iface_modem_3gpp_enable (MM_IFACE_MODEM_3GPP (ctx->self),
                                        ctx->cancellable,
                                        (GAsyncReadyCallback)iface_modem_3gpp_enable_ready,
                                        ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case ENABLING_STEP_IFACE_3GPP_USSD:
        if (ctx->self->priv->modem_3gpp_ussd_dbus_skeleton) {
            mm_dbg ("Modem has 3GPP/USSD capabilities, enabling the Modem 3GPP/USSD interface...");
            mm_iface_modem_3gpp_ussd_enable (MM_IFACE_MODEM_3GPP_USSD (ctx->self),
                                             (GAsyncReadyCallback)iface_modem_3gpp_ussd_enable_ready,
                                             ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case ENABLING_STEP_IFACE_CDMA:
        if (ctx->self->priv->modem_cdma_dbus_skeleton) {
            mm_dbg ("Modem has CDMA capabilities, enabling the Modem CDMA interface...");
            /* Enabling the Modem CDMA interface */
            mm_iface_modem_cdma_enable (MM_IFACE_MODEM_CDMA (ctx->self),
                                        ctx->cancellable,
                                        (GAsyncReadyCallback)iface_modem_cdma_enable_ready,
                                        ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case ENABLING_STEP_IFACE_CONTACTS:
        /* Fall down to next step */
        ctx->step++;

    case ENABLING_STEP_IFACE_LOCATION:
        if (ctx->self->priv->modem_location_dbus_skeleton) {
            mm_dbg ("Modem has location capabilities, enabling the Location interface...");
            /* Enabling the Modem Location interface */
            mm_iface_modem_location_enable (MM_IFACE_MODEM_LOCATION (ctx->self),
                                            ctx->cancellable,
                                            (GAsyncReadyCallback)iface_modem_location_enable_ready,
                                            ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case ENABLING_STEP_IFACE_MESSAGING:
        if (ctx->self->priv->modem_messaging_dbus_skeleton) {
            mm_dbg ("Modem has messaging capabilities, enabling the Messaging interface...");
            /* Enabling the Modem Messaging interface */
            mm_iface_modem_messaging_enable (MM_IFACE_MODEM_MESSAGING (ctx->self),
                                             ctx->cancellable,
                                             (GAsyncReadyCallback)iface_modem_messaging_enable_ready,
                                             ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case ENABLING_STEP_IFACE_VOICE:
        if (ctx->self->priv->modem_voice_dbus_skeleton) {
            mm_dbg ("Modem has voice capabilities, enabling the Voice interface...");
            /* Enabling the Modem Voice interface */
            mm_iface_modem_voice_enable (MM_IFACE_MODEM_VOICE (ctx->self),
                                             ctx->cancellable,
                                             (GAsyncReadyCallback)iface_modem_voice_enable_ready,
                                             ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case ENABLING_STEP_IFACE_TIME:
        if (ctx->self->priv->modem_time_dbus_skeleton) {
            mm_dbg ("Modem has time capabilities, enabling the Time interface...");
            /* Enabling the Modem Time interface */
            mm_iface_modem_time_enable (MM_IFACE_MODEM_TIME (ctx->self),
                                        ctx->cancellable,
                                        (GAsyncReadyCallback)iface_modem_time_enable_ready,
                                        ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case ENABLING_STEP_IFACE_SIGNAL:
        if (ctx->self->priv->modem_signal_dbus_skeleton) {
            mm_dbg ("Modem has extended signal reporting capabilities, enabling the Signal interface...");
            /* Enabling the Modem Signal interface */
            mm_iface_modem_signal_enable (MM_IFACE_MODEM_SIGNAL (ctx->self),
                                          ctx->cancellable,
                                          (GAsyncReadyCallback)iface_modem_signal_enable_ready,
                                          ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case ENABLING_STEP_IFACE_OMA:
        if (ctx->self->priv->modem_oma_dbus_skeleton) {
            mm_dbg ("Modem has OMA capabilities, enabling the OMA interface...");
            /* Enabling the Modem Oma interface */
            mm_iface_modem_oma_enable (MM_IFACE_MODEM_OMA (ctx->self),
                                       ctx->cancellable,
                                       (GAsyncReadyCallback)iface_modem_oma_enable_ready,
                                       ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case ENABLING_STEP_IFACE_FIRMWARE:
        /* Fall down to next step */
        ctx->step++;

    case ENABLING_STEP_IFACE_SIMPLE:
        /* Fall down to next step */
        ctx->step++;

    case ENABLING_STEP_LAST:
        ctx->enabled = TRUE;

        /* Once all interfaces have been enabled, trigger registration checks in
         * 3GPP and CDMA modems. We have to do this at this point so that e.g.
         * location interface gets proper registration related info reported.
         *
         * We do this in an idle so that we don't mess up the logs at this point
         * with the new requests being triggered.
         */
        schedule_initial_registration_checks (ctx->self);

        /* All enabled without errors! */
        g_simple_async_result_set_op_res_gboolean (ctx->result, TRUE);
        enabling_context_complete_and_free (ctx);
        return;
    }

    g_assert_not_reached ();
}

void
mm_base_modem_enable (MMBaseModem *self,
                      GAsyncReadyCallback callback,
                      gpointer user_data)
{
    GSimpleAsyncResult *result;

    result = g_simple_async_result_new (G_OBJECT (self), callback, user_data, mm_base_modem_enable);

    /* Check state before launching modem enabling */
    switch (MM_BASE_MODEM (self)->priv->modem_state) {
    case MM_MODEM_STATE_UNKNOWN:
        /* We should never have a UNKNOWN->ENABLED transition */
        g_assert_not_reached ();
        break;

    case MM_MODEM_STATE_FAILED:
    case MM_MODEM_STATE_INITIALIZING:
        g_simple_async_result_set_error (result,
                                         MM_CORE_ERROR,
                                         MM_CORE_ERROR_WRONG_STATE,
                                         "Cannot enable modem: "
                                         "device not fully initialized yet");
        break;

    case MM_MODEM_STATE_LOCKED:
        g_simple_async_result_set_error (result,
                                         MM_CORE_ERROR,
                                         MM_CORE_ERROR_WRONG_STATE,
                                         "Cannot enable modem: device locked");
        break;

    case MM_MODEM_STATE_DISABLED: {
        EnablingContext *ctx;

        ctx = g_new0 (EnablingContext, 1);
        ctx->self = g_object_ref (self);
        ctx->result = result;
        ctx->cancellable = g_object_ref (self->priv->cancellable);
        ctx->step = ENABLING_STEP_FIRST;
        enabling_step (ctx);
        return;
    }

    case MM_MODEM_STATE_DISABLING:
        g_simple_async_result_set_error (result,
                                         MM_CORE_ERROR,
                                         MM_CORE_ERROR_WRONG_STATE,
                                         "Cannot enable modem: "
                                         "currently being disabled");
        break;

    case MM_MODEM_STATE_ENABLING:
        g_simple_async_result_set_error (result,
                                         MM_CORE_ERROR,
                                         MM_CORE_ERROR_IN_PROGRESS,
                                         "Cannot enable modem: "
                                         "already being enabled");
        break;

    case MM_MODEM_STATE_ENABLED:
    case MM_MODEM_STATE_SEARCHING:
    case MM_MODEM_STATE_REGISTERED:
    case MM_MODEM_STATE_DISCONNECTING:
    case MM_MODEM_STATE_CONNECTING:
    case MM_MODEM_STATE_CONNECTED:
        /* Just return success, don't relaunch enabling */
        g_simple_async_result_set_op_res_gboolean (result, TRUE);
        break;
    }

    g_simple_async_result_complete_in_idle (result);
    g_object_unref (result);
}

/*****************************************************************************/
/* Initialize */

typedef enum {
    INITIALIZE_STEP_FIRST,
    INITIALIZE_STEP_SETUP_PORTS,
    INITIALIZE_STEP_STARTED,
    INITIALIZE_STEP_SETUP_SIMPLE_STATUS,
    INITIALIZE_STEP_IFACE_MODEM,
    INITIALIZE_STEP_IFACE_3GPP,
    INITIALIZE_STEP_IFACE_3GPP_USSD,
    INITIALIZE_STEP_IFACE_CDMA,
    INITIALIZE_STEP_IFACE_CONTACTS,
    INITIALIZE_STEP_IFACE_LOCATION,
    INITIALIZE_STEP_IFACE_MESSAGING,
    INITIALIZE_STEP_IFACE_VOICE,
    INITIALIZE_STEP_IFACE_TIME,
    INITIALIZE_STEP_IFACE_SIGNAL,
    INITIALIZE_STEP_IFACE_OMA,
    INITIALIZE_STEP_IFACE_FIRMWARE,
    INITIALIZE_STEP_IFACE_SIMPLE,
    INITIALIZE_STEP_LAST,
} InitializeStep;

typedef struct {
    MMBaseModem *self;
    GCancellable *cancellable;
    GSimpleAsyncResult *result;
    InitializeStep step;
    gpointer ports_ctx;
} InitializeContext;

static void initialize_step (InitializeContext *ctx);

static void
initialize_context_complete_and_free (InitializeContext *ctx)
{
    GError *error = NULL;

    g_simple_async_result_complete_in_idle (ctx->result);

    if (ctx->ports_ctx &&
        MM_BASE_MODEM_GET_CLASS (ctx->self)->initialization_stopped &&
        !MM_BASE_MODEM_GET_CLASS (ctx->self)->initialization_stopped (ctx->self, ctx->ports_ctx, &error)) {
        mm_warn ("Error when stopping the initialization sequence: %s", error->message);
        g_error_free (error);
    }

    g_object_unref (ctx->result);
    g_object_unref (ctx->cancellable);
    g_object_unref (ctx->self);
    g_free (ctx);
}

static gboolean
initialize_context_complete_and_free_if_cancelled (InitializeContext *ctx)
{
    if (!g_cancellable_is_cancelled (ctx->cancellable))
        return FALSE;

    g_simple_async_result_set_error (ctx->result,
                                     MM_CORE_ERROR,
                                     MM_CORE_ERROR_CANCELLED,
                                     "Initialization cancelled");
    initialize_context_complete_and_free (ctx);
    return TRUE;
}

gboolean
mm_base_modem_initialize_finish (MMBaseModem *self,
                                 GAsyncResult *res,
                                 GError **error)
{
    if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
        return FALSE;

    return TRUE;
}

static void
initialization_started_ready (MMBaseModem *self,
                              GAsyncResult *result,
                              InitializeContext *ctx)
{
    GError *error = NULL;
    gpointer ports_ctx;

    /* May return NULL without error */
    ports_ctx = MM_BASE_MODEM_GET_CLASS (self)->initialization_started_finish (self, result, &error);
    if (error) {
        mm_warn ("Couldn't start initialization: %s", error->message);
        g_error_free (error);

        /* There is no Modem interface yet, so just update the variable directly */
        ctx->self->priv->modem_state = MM_MODEM_STATE_FAILED;

        /* Just jump to the last step */
        ctx->step = INITIALIZE_STEP_LAST;
        initialize_step (ctx);
        return;
    }

    /* Keep the ctx for later use when stopping initialization */
    ctx->ports_ctx = ports_ctx;

    /* Go on to next step */
    ctx->step++;
    initialize_step (ctx);
}

static void
iface_modem_initialize_ready (MMBaseModem *self,
                              GAsyncResult *result,
                              InitializeContext *ctx)
{
    GError *error = NULL;

    /* If the modem interface fails to get initialized, we will move the modem
     * to a FAILED state. Note that in this case we still export the interface. */
    if (!mm_iface_modem_initialize_finish (MM_IFACE_MODEM (self), result, &error)) {
        MMModemStateFailedReason failed_reason = MM_MODEM_STATE_FAILED_REASON_UNKNOWN;

        /* Report the new FAILED state */
        mm_warn ("Modem couldn't be initialized: %s", error->message);

        if (g_error_matches (error,
                             MM_MOBILE_EQUIPMENT_ERROR,
                             MM_MOBILE_EQUIPMENT_ERROR_SIM_NOT_INSERTED))
            failed_reason = MM_MODEM_STATE_FAILED_REASON_SIM_MISSING;
        else if (g_error_matches (error,
                                  MM_MOBILE_EQUIPMENT_ERROR,
                                  MM_MOBILE_EQUIPMENT_ERROR_SIM_FAILURE) ||
                 g_error_matches (error,
                                  MM_MOBILE_EQUIPMENT_ERROR,
                                  MM_MOBILE_EQUIPMENT_ERROR_SIM_WRONG))
            failed_reason = MM_MODEM_STATE_FAILED_REASON_SIM_MISSING;

        g_error_free (error);

        mm_iface_modem_update_failed_state (MM_IFACE_MODEM (self), failed_reason);

        /* Jump to the firmware step. We allow firmware switching even in failed
         * state */
        ctx->step = INITIALIZE_STEP_IFACE_FIRMWARE;
        initialize_step (ctx);
        return;
    }

    /* bind simple properties */
    mm_iface_modem_bind_simple_status (MM_IFACE_MODEM (self),
                                       self->priv->modem_simple_status);

    /* If we find ourselves in a LOCKED state, we shouldn't keep on
     * the initialization sequence. Instead, we will re-initialize once
     * we are unlocked. */
    if (ctx->self->priv->modem_state == MM_MODEM_STATE_LOCKED) {
        /* Jump to the Firmware interface. We do allow modems to export
         * both the Firmware and Simple interfaces when locked. */
        ctx->step = INITIALIZE_STEP_IFACE_FIRMWARE;
        initialize_step (ctx);
        return;
    }

    /* Go on to next step */
    ctx->step++;
    initialize_step (ctx);
}

#undef INTERFACE_INIT_READY_FN
#define INTERFACE_INIT_READY_FN(NAME,TYPE,FATAL_ERRORS)                 \
    static void                                                         \
    NAME##_initialize_ready (MMBaseModem *self,                         \
                             GAsyncResult *result,                      \
                             InitializeContext *ctx)                    \
    {                                                                   \
        GError *error = NULL;                                           \
                                                                        \
        if (!mm_##NAME##_initialize_finish (TYPE (self), result, &error)) { \
            if (FATAL_ERRORS) {                                         \
                mm_warn ("Couldn't initialize interface: '%s'",         \
                         error->message);                               \
                g_error_free (error);                                   \
                                                                        \
                /* Report the new FAILED state */                       \
                mm_iface_modem_update_failed_state (MM_IFACE_MODEM (self), \
                                                    MM_MODEM_STATE_FAILED_REASON_UNKNOWN); \
                                                                        \
                /* Just jump to the last step */                        \
                ctx->step = INITIALIZE_STEP_LAST;                       \
                initialize_step (ctx);                                  \
                return;                                                 \
            }                                                           \
                                                                        \
            mm_dbg ("Couldn't initialize interface: '%s'",              \
                    error->message);                                    \
            /* Just shutdown this interface */                          \
            mm_##NAME##_shutdown (TYPE (self));                         \
            g_error_free (error);                                       \
        } else {                                                        \
            /* bind simple properties */                                \
            mm_##NAME##_bind_simple_status (TYPE (self), self->priv->modem_simple_status); \
        }                                                               \
                                                                        \
        /* Go on to next step */                                        \
        ctx->step++;                                                    \
        initialize_step (ctx);                                          \
    }

INTERFACE_INIT_READY_FN (iface_modem_3gpp,      MM_IFACE_MODEM_3GPP,      TRUE)
INTERFACE_INIT_READY_FN (iface_modem_3gpp_ussd, MM_IFACE_MODEM_3GPP_USSD, FALSE)
INTERFACE_INIT_READY_FN (iface_modem_cdma,      MM_IFACE_MODEM_CDMA,      TRUE)
INTERFACE_INIT_READY_FN (iface_modem_location,  MM_IFACE_MODEM_LOCATION,  FALSE)
INTERFACE_INIT_READY_FN (iface_modem_messaging, MM_IFACE_MODEM_MESSAGING, FALSE)
INTERFACE_INIT_READY_FN (iface_modem_voice,     MM_IFACE_MODEM_VOICE,     FALSE)
INTERFACE_INIT_READY_FN (iface_modem_time,      MM_IFACE_MODEM_TIME,      FALSE)
INTERFACE_INIT_READY_FN (iface_modem_signal,    MM_IFACE_MODEM_SIGNAL,    FALSE)
INTERFACE_INIT_READY_FN (iface_modem_oma,       MM_IFACE_MODEM_OMA,       FALSE)
INTERFACE_INIT_READY_FN (iface_modem_firmware,  MM_IFACE_MODEM_FIRMWARE,  FALSE)

static void
initialize_step (InitializeContext *ctx)
{
    /* Don't run new steps if we're cancelled */
    if (initialize_context_complete_and_free_if_cancelled (ctx))
        return;

    switch (ctx->step) {
    case INITIALIZE_STEP_FIRST:
        /* Fall down to next step */
        ctx->step++;

    case INITIALIZE_STEP_SETUP_PORTS:
        if (MM_BASE_MODEM_GET_CLASS (ctx->self)->setup_ports)
            MM_BASE_MODEM_GET_CLASS (ctx->self)->setup_ports (ctx->self);
        /* Fall down to next step */
        ctx->step++;

    case INITIALIZE_STEP_STARTED:
        if (MM_BASE_MODEM_GET_CLASS (ctx->self)->initialization_started &&
            MM_BASE_MODEM_GET_CLASS (ctx->self)->initialization_started_finish) {
            MM_BASE_MODEM_GET_CLASS (ctx->self)->initialization_started (ctx->self,
                                                                              (GAsyncReadyCallback)initialization_started_ready,
                                                                              ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case INITIALIZE_STEP_SETUP_SIMPLE_STATUS:
        /* Simple status must be created before any interface initialization,
         * so that interfaces add and bind the properties they want to export.
         */
        if (!ctx->self->priv->modem_simple_status)
            ctx->self->priv->modem_simple_status = mm_simple_status_new ();
        /* Fall down to next step */
        ctx->step++;

    case INITIALIZE_STEP_IFACE_MODEM:
        /* Initialize the Modem interface */
        mm_iface_modem_initialize (MM_IFACE_MODEM (ctx->self),
                                   ctx->cancellable,
                                   (GAsyncReadyCallback)iface_modem_initialize_ready,
                                   ctx);
        return;

    case INITIALIZE_STEP_IFACE_3GPP:
        if (mm_iface_modem_is_3gpp (MM_IFACE_MODEM (ctx->self))) {
            /* Initialize the 3GPP interface */
            mm_iface_modem_3gpp_initialize (MM_IFACE_MODEM_3GPP (ctx->self),
                                            ctx->cancellable,
                                            (GAsyncReadyCallback)iface_modem_3gpp_initialize_ready,
                                            ctx);
            return;
        }

        /* Fall down to next step */
        ctx->step++;

    case INITIALIZE_STEP_IFACE_3GPP_USSD:
        if (mm_iface_modem_is_3gpp (MM_IFACE_MODEM (ctx->self))) {
            /* Initialize the 3GPP/USSD interface */
            mm_iface_modem_3gpp_ussd_initialize (MM_IFACE_MODEM_3GPP_USSD (ctx->self),
                                                 (GAsyncReadyCallback)iface_modem_3gpp_ussd_initialize_ready,
                                                 ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case INITIALIZE_STEP_IFACE_CDMA:
        if (mm_iface_modem_is_cdma (MM_IFACE_MODEM (ctx->self))) {
            /* Initialize the CDMA interface */
            mm_iface_modem_cdma_initialize (MM_IFACE_MODEM_CDMA (ctx->self),
                                            ctx->cancellable,
                                            (GAsyncReadyCallback)iface_modem_cdma_initialize_ready,
                                            ctx);
            return;
        }
        /* Fall down to next step */
        ctx->step++;

    case INITIALIZE_STEP_IFACE_CONTACTS:
        /* Fall down to next step */
        ctx->step++;

    case INITIALIZE_STEP_IFACE_LOCATION:
        /* Initialize the Location interface */
        mm_iface_modem_location_initialize (MM_IFACE_MODEM_LOCATION (ctx->self),
                                            ctx->cancellable,
                                            (GAsyncReadyCallback)iface_modem_location_initialize_ready,
                                            ctx);
        return;

    case INITIALIZE_STEP_IFACE_MESSAGING:
        /* Initialize the Messaging interface */
        mm_iface_modem_messaging_initialize (MM_IFACE_MODEM_MESSAGING (ctx->self),
                                             ctx->cancellable,
                                             (GAsyncReadyCallback)iface_modem_messaging_initialize_ready,
                                             ctx);
        return;

    case INITIALIZE_STEP_IFACE_VOICE:
        /* Initialize the Voice interface */
        mm_iface_modem_voice_initialize (MM_IFACE_MODEM_VOICE (ctx->self),
                                         ctx->cancellable,
                                         (GAsyncReadyCallback)iface_modem_voice_initialize_ready,
                                         ctx);
        return;

    case INITIALIZE_STEP_IFACE_TIME:
        /* Initialize the Time interface */
        mm_iface_modem_time_initialize (MM_IFACE_MODEM_TIME (ctx->self),
                                        ctx->cancellable,
                                        (GAsyncReadyCallback)iface_modem_time_initialize_ready,
                                        ctx);
        return;

    case INITIALIZE_STEP_IFACE_SIGNAL:
        /* Initialize the Signal interface */
        mm_iface_modem_signal_initialize (MM_IFACE_MODEM_SIGNAL (ctx->self),
                                          ctx->cancellable,
                                          (GAsyncReadyCallback)iface_modem_signal_initialize_ready,
                                          ctx);
        return;

    case INITIALIZE_STEP_IFACE_OMA:
        /* Initialize the Oma interface */
        mm_iface_modem_oma_initialize (MM_IFACE_MODEM_OMA (ctx->self),
                                       ctx->cancellable,
                                       (GAsyncReadyCallback)iface_modem_oma_initialize_ready,
                                       ctx);
        return;

    case INITIALIZE_STEP_IFACE_FIRMWARE:
        /* Initialize the Firmware interface */
        mm_iface_modem_firmware_initialize (MM_IFACE_MODEM_FIRMWARE (ctx->self),
                                            ctx->cancellable,
                                            (GAsyncReadyCallback)iface_modem_firmware_initialize_ready,
                                            ctx);
        return;

    case INITIALIZE_STEP_IFACE_SIMPLE:
        if (ctx->self->priv->modem_state != MM_MODEM_STATE_FAILED)
            mm_iface_modem_simple_initialize (MM_IFACE_MODEM_SIMPLE (ctx->self));
        /* Fall down to next step */
        ctx->step++;

    case INITIALIZE_STEP_LAST:
        if (ctx->self->priv->modem_state == MM_MODEM_STATE_FAILED) {


            if (!ctx->self->priv->modem_dbus_skeleton) {
                /* Error setting up ports. Abort without even exporting the
                 * Modem interface */
                g_simple_async_result_set_error (ctx->result,
                                                 MM_CORE_ERROR,
                                                 MM_CORE_ERROR_ABORTED,
                                                 "Modem is unusable, "
                                                 "cannot fully initialize");
            } else {
                /* Fatal SIM failure :-( */
                g_simple_async_result_set_error (ctx->result,
                                                 MM_CORE_ERROR,
                                                 MM_CORE_ERROR_WRONG_STATE,
                                                 "Modem is unusable, "
                                                 "cannot fully initialize");
                /* Ensure we only leave the Modem interface around */
                mm_iface_modem_3gpp_shutdown (MM_IFACE_MODEM_3GPP (ctx->self));
                mm_iface_modem_3gpp_ussd_shutdown (MM_IFACE_MODEM_3GPP_USSD (ctx->self));
                mm_iface_modem_cdma_shutdown (MM_IFACE_MODEM_CDMA (ctx->self));
                mm_iface_modem_location_shutdown (MM_IFACE_MODEM_LOCATION (ctx->self));
                mm_iface_modem_messaging_shutdown (MM_IFACE_MODEM_MESSAGING (ctx->self));
                mm_iface_modem_voice_shutdown (MM_IFACE_MODEM_VOICE (ctx->self));
                mm_iface_modem_time_shutdown (MM_IFACE_MODEM_TIME (ctx->self));
                mm_iface_modem_simple_shutdown (MM_IFACE_MODEM_SIMPLE (ctx->self));
            }
            initialize_context_complete_and_free (ctx);
            return;
        }

        if (ctx->self->priv->modem_state == MM_MODEM_STATE_LOCKED) {
            /* We're locked :-/ */
            g_simple_async_result_set_error (ctx->result,
                                             MM_CORE_ERROR,
                                             MM_CORE_ERROR_WRONG_STATE,
                                             "Modem is currently locked, "
                                             "cannot fully initialize");
            initialize_context_complete_and_free (ctx);
            return;
        }

        /* All initialized without errors!
         * Set as disabled (a.k.a. initialized) */
        mm_iface_modem_update_state (MM_IFACE_MODEM (ctx->self),
                                     MM_MODEM_STATE_DISABLED,
                                     MM_MODEM_STATE_CHANGE_REASON_UNKNOWN);

        g_simple_async_result_set_op_res_gboolean (ctx->result, TRUE);
        initialize_context_complete_and_free (ctx);
        return;
    }

    g_assert_not_reached ();
}

void
mm_base_modem_initialize (MMBaseModem *self,
                          GAsyncReadyCallback callback,
                          gpointer user_data)
{
    GSimpleAsyncResult *result;

    result = g_simple_async_result_new (G_OBJECT (self), callback, user_data, mm_base_modem_initialize);

    /* Check state before launching modem initialization */
    switch (MM_BASE_MODEM (self)->priv->modem_state) {
    case MM_MODEM_STATE_FAILED:
        /* NOTE: this will only happen if we ever support hot-plugging SIMs */
        g_simple_async_result_set_error (result,
                                         MM_CORE_ERROR,
                                         MM_CORE_ERROR_WRONG_STATE,
                                         "Cannot initialize modem: "
                                         "device is unusable");
        break;

    case MM_MODEM_STATE_UNKNOWN:
    case MM_MODEM_STATE_LOCKED: {
        InitializeContext *ctx;

        ctx = g_new0 (InitializeContext, 1);
        ctx->self = g_object_ref (self);
        ctx->cancellable = g_object_ref (self->priv->cancellable);
        ctx->result = result;
        ctx->step = INITIALIZE_STEP_FIRST;

        /* Set as being initialized, even if we were locked before */
        mm_iface_modem_update_state (MM_IFACE_MODEM (self),
                                     MM_MODEM_STATE_INITIALIZING,
                                     MM_MODEM_STATE_CHANGE_REASON_UNKNOWN);

        initialize_step (ctx);
        return;
    }

    case MM_MODEM_STATE_INITIALIZING:
        g_simple_async_result_set_error (result,
                                         MM_CORE_ERROR,
                                         MM_CORE_ERROR_IN_PROGRESS,
                                         "Cannot initialize modem: "
                                         "already being initialized");
        break;

    case MM_MODEM_STATE_DISABLED:
    case MM_MODEM_STATE_DISABLING:
    case MM_MODEM_STATE_ENABLING:
    case MM_MODEM_STATE_ENABLED:
    case MM_MODEM_STATE_SEARCHING:
    case MM_MODEM_STATE_REGISTERED:
    case MM_MODEM_STATE_DISCONNECTING:
    case MM_MODEM_STATE_CONNECTING:
    case MM_MODEM_STATE_CONNECTED:
        /* Just return success, don't relaunch initialization */
        g_simple_async_result_set_op_res_gboolean (result, TRUE);
        break;
    }

    g_simple_async_result_complete_in_idle (result);
    g_object_unref (result);
}

/*****************************************************************************/

void
mm_base_modem_set_hotplugged (MMBaseModem *self,
                              gboolean hotplugged)
{
    g_return_if_fail (MM_IS_BASE_MODEM (self));

    self->priv->hotplugged = hotplugged;
}

gboolean
mm_base_modem_get_hotplugged (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), FALSE);

    return self->priv->hotplugged;
}

void
mm_base_modem_set_valid (MMBaseModem *self,
                         gboolean new_valid)
{
    g_return_if_fail (MM_IS_BASE_MODEM (self));

    /* If validity changed OR if both old and new were invalid, notify. This
     * last case is to cover failures during initialization. */
    if (self->priv->valid != new_valid ||
        !new_valid) {
        self->priv->valid = new_valid;
        g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_VALID]);
    }
}

gboolean
mm_base_modem_get_valid (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), FALSE);

    return self->priv->valid;
}

GCancellable *
mm_base_modem_peek_cancellable (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return self->priv->cancellable;
}

GCancellable *
mm_base_modem_get_cancellable  (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return g_object_ref (self->priv->cancellable);
}

MMPortSerialAt *
mm_base_modem_get_port_primary (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return (self->priv->primary ? g_object_ref (self->priv->primary) : NULL);
}

MMPortSerialAt *
mm_base_modem_peek_port_primary (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return self->priv->primary;
}

MMPortSerialAt *
mm_base_modem_get_port_secondary (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return (self->priv->secondary ? g_object_ref (self->priv->secondary) : NULL);
}

MMPortSerialAt *
mm_base_modem_peek_port_secondary (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return self->priv->secondary;
}

MMPortSerialQcdm *
mm_base_modem_get_port_qcdm (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return (self->priv->qcdm ? g_object_ref (self->priv->qcdm) : NULL);
}

MMPortSerialQcdm *
mm_base_modem_peek_port_qcdm (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return self->priv->qcdm;
}

MMPortSerialAt *
mm_base_modem_get_port_gps_control (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return (self->priv->gps_control ? g_object_ref (self->priv->gps_control) : NULL);
}

MMPortSerialAt *
mm_base_modem_peek_port_gps_control (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return self->priv->gps_control;
}

MMPortSerialGps *
mm_base_modem_get_port_gps (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return (self->priv->gps ? g_object_ref (self->priv->gps) : NULL);
}

MMPortSerialGps *
mm_base_modem_peek_port_gps (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return self->priv->gps;
}

#if defined WITH_QMI

MMPortQmi *
mm_base_modem_get_port_qmi (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    /* First QMI port in the list is the primary one always */
    return (self->priv->qmi ? ((MMPortQmi *)g_object_ref (self->priv->qmi->data)) : NULL);
}

MMPortQmi *
mm_base_modem_peek_port_qmi (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    /* First QMI port in the list is the primary one always */
    return (self->priv->qmi ? (MMPortQmi *)self->priv->qmi->data : NULL);
}

MMPortQmi *
mm_base_modem_get_port_qmi_for_data (MMBaseModem *self,
                                     MMPort *data,
                                     GError **error)
{
    MMPortQmi *qmi;

    qmi = mm_base_modem_peek_port_qmi_for_data (self, data, error);
    return (qmi ? (MMPortQmi *)g_object_ref (qmi) : NULL);
}

MMPortQmi *
mm_base_modem_peek_port_qmi_for_data (MMBaseModem *self,
                                      MMPort *data,
                                      GError **error)
{
    GList *cdc_wdm_qmi_ports, *l;
    const gchar *net_port_parent_path;

    g_warn_if_fail (mm_port_get_subsys (data) == MM_PORT_SUBSYS_NET);
    net_port_parent_path = mm_port_get_parent_path (data);
    if (!net_port_parent_path) {
        g_set_error (error,
                     MM_CORE_ERROR,
                     MM_CORE_ERROR_FAILED,
                     "No parent path for 'net/%s'",
                     mm_port_get_device (data));
        return NULL;
    }

    /* Find the CDC-WDM port on the same USB interface as the given net port */
    cdc_wdm_qmi_ports = mm_base_modem_find_ports (MM_BASE_MODEM (self),
                                                  MM_PORT_SUBSYS_USB,
                                                  MM_PORT_TYPE_QMI,
                                                  NULL);
    for (l = cdc_wdm_qmi_ports; l; l = g_list_next (l)) {
        const gchar *wdm_port_parent_path;

        g_assert (MM_IS_PORT_QMI (l->data));
        wdm_port_parent_path = mm_port_get_parent_path (MM_PORT (l->data));
        if (wdm_port_parent_path && g_str_equal (wdm_port_parent_path, net_port_parent_path))
            return MM_PORT_QMI (l->data);
    }

    g_set_error (error,
                 MM_CORE_ERROR,
                 MM_CORE_ERROR_NOT_FOUND,
                 "Couldn't find associated QMI port for 'net/%s'",
                 mm_port_get_device (data));
    return NULL;
}

#endif /* WITH_QMI */

#if defined WITH_MBIM

MMPortMbim *
mm_base_modem_get_port_mbim (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    /* First MBIM port in the list is the primary one always */
    return (self->priv->mbim ? ((MMPortMbim *)g_object_ref (self->priv->mbim->data)) : NULL);
}

MMPortMbim *
mm_base_modem_peek_port_mbim (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    /* First MBIM port in the list is the primary one always */
    return (self->priv->mbim ? (MMPortMbim *)self->priv->mbim->data : NULL);
}

MMPortMbim *
mm_base_modem_get_port_mbim_for_data (MMBaseModem *self,
                                      MMPort *data,
                                      GError **error)
{
    MMPortMbim *mbim;

    mbim = mm_base_modem_peek_port_mbim_for_data (self, data, error);
    return (mbim ? (MMPortMbim *)g_object_ref (mbim) : NULL);
}

MMPortMbim *
mm_base_modem_peek_port_mbim_for_data (MMBaseModem *self,
                                       MMPort *data,
                                       GError **error)
{
    GList *cdc_wdm_mbim_ports, *l;
    const gchar *net_port_parent_path;

    g_warn_if_fail (mm_port_get_subsys (data) == MM_PORT_SUBSYS_NET);
    net_port_parent_path = mm_port_get_parent_path (data);
    if (!net_port_parent_path) {
        g_set_error (error,
                     MM_CORE_ERROR,
                     MM_CORE_ERROR_FAILED,
                     "No parent path for 'net/%s'",
                     mm_port_get_device (data));
        return NULL;
    }

    /* Find the CDC-WDM port on the same USB interface as the given net port */
    cdc_wdm_mbim_ports = mm_base_modem_find_ports (MM_BASE_MODEM (self),
                                                  MM_PORT_SUBSYS_USB,
                                                  MM_PORT_TYPE_MBIM,
                                                  NULL);
    for (l = cdc_wdm_mbim_ports; l; l = g_list_next (l)) {
        const gchar *wdm_port_parent_path;

        g_assert (MM_IS_PORT_MBIM (l->data));
        wdm_port_parent_path = mm_port_get_parent_path (MM_PORT (l->data));
        if (wdm_port_parent_path && g_str_equal (wdm_port_parent_path, net_port_parent_path))
            return MM_PORT_MBIM (l->data);
    }

    g_set_error (error,
                 MM_CORE_ERROR,
                 MM_CORE_ERROR_NOT_FOUND,
                 "Couldn't find associated MBIM port for 'net/%s'",
                 mm_port_get_device (data));
    return NULL;
}

#endif /* WITH_MBIM */

MMPort *
mm_base_modem_get_best_data_port (MMBaseModem *self,
                                  MMPortType type)
{
    MMPort *port;

    port = mm_base_modem_peek_best_data_port (self, type);
    return (port ? g_object_ref (port) : NULL);
}

MMPort *
mm_base_modem_peek_best_data_port (MMBaseModem *self,
                                   MMPortType type)
{
    GList *l;

    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    /* Return first not-connected data port */
    for (l = self->priv->data; l; l = g_list_next (l)) {
        if (!mm_port_get_connected ((MMPort *)l->data) &&
            (mm_port_get_port_type ((MMPort *)l->data) == type ||
             type == MM_PORT_TYPE_UNKNOWN)) {
            return (MMPort *)l->data;
        }
    }

    return NULL;
}

GList *
mm_base_modem_get_data_ports (MMBaseModem *self)
{
    GList *copy;

    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    copy = g_list_copy (self->priv->data);
    g_list_foreach (copy, (GFunc)g_object_ref, NULL);
    return copy;
}

GList *
mm_base_modem_peek_data_ports (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return self->priv->data;
}

MMPortSerialAt *
mm_base_modem_get_best_at_port (MMBaseModem *self,
                                GError **error)
{
    MMPortSerialAt *best;

    best = mm_base_modem_peek_best_at_port (self, error);
    return (best ? g_object_ref (best) : NULL);
}

MMPortSerialAt *
mm_base_modem_peek_best_at_port (MMBaseModem *self,
                                 GError **error)
{
    /* Decide which port to use */
    if (self->priv->primary &&
        !mm_port_get_connected (MM_PORT (self->priv->primary)))
        return self->priv->primary;

    /* If primary port is connected, check if we can get the secondary
     * port */
    if (self->priv->secondary &&
        !mm_port_get_connected (MM_PORT (self->priv->secondary)))
        return self->priv->secondary;

    /* Otherwise, we cannot get any port */
    g_set_error (error,
                 MM_CORE_ERROR,
                 MM_CORE_ERROR_CONNECTED,
                 "No AT port available to run command");
    return NULL;
}

gboolean
mm_base_modem_has_at_port (MMBaseModem *self)
{
    GHashTableIter iter;
    gpointer value;
    gpointer key;

    /* We'll iterate the ht of ports, looking for any port which is AT */
    g_hash_table_iter_init (&iter, self->priv->ports);
    while (g_hash_table_iter_next (&iter, &key, &value)) {
        if (MM_IS_PORT_SERIAL_AT (value))
            return TRUE;
    }

    return FALSE;
}

MMModemPortInfo *
mm_base_modem_get_port_infos (MMBaseModem *self,
                              guint *n_port_infos)
{
    GHashTableIter iter;
    MMModemPortInfo *port_infos;
    MMPort *port;
    guint i;

    *n_port_infos = g_hash_table_size (self->priv->ports);
    port_infos = g_new (MMModemPortInfo, *n_port_infos);
    g_hash_table_iter_init (&iter, self->priv->ports);
    i = 0;
    while (g_hash_table_iter_next (&iter, NULL, (gpointer)&port)) {
        port_infos[i].name = g_strdup (mm_port_get_device (port));
        switch (mm_port_get_port_type (port)) {
        case MM_PORT_TYPE_NET:
            port_infos[i].type = MM_MODEM_PORT_TYPE_NET;
            break;
        case MM_PORT_TYPE_AT:
            port_infos[i].type = MM_MODEM_PORT_TYPE_AT;
            break;
        case MM_PORT_TYPE_QCDM:
            port_infos[i].type = MM_MODEM_PORT_TYPE_QCDM;
            break;
        case MM_PORT_TYPE_GPS:
            port_infos[i].type = MM_MODEM_PORT_TYPE_GPS;
            break;
        case MM_PORT_TYPE_QMI:
            port_infos[i].type = MM_MODEM_PORT_TYPE_QMI;
            break;
        case MM_PORT_TYPE_MBIM:
            port_infos[i].type = MM_MODEM_PORT_TYPE_MBIM;
            break;
        case MM_PORT_TYPE_UNKNOWN:
        case MM_PORT_TYPE_IGNORED:
        default:
            port_infos[i].type = MM_MODEM_PORT_TYPE_UNKNOWN;
            break;
        }

        i++;
    }

    return port_infos;
}

GList *
mm_base_modem_find_ports (MMBaseModem *self,
                          MMPortSubsys subsys,
                          MMPortType type,
                          const gchar *name)
{
    GList *out = NULL;
    GHashTableIter iter;
    gpointer value;
    gpointer key;

    /* We'll iterate the ht of ports, looking for any port which is matches
     * the compare function */
    g_hash_table_iter_init (&iter, self->priv->ports);
    while (g_hash_table_iter_next (&iter, &key, &value)) {
        MMPort *port = MM_PORT (value);

        if (subsys != MM_PORT_SUBSYS_UNKNOWN && mm_port_get_subsys (port) != subsys)
            continue;

        if (type != MM_PORT_TYPE_UNKNOWN && mm_port_get_port_type (port) != type)
            continue;

        if (name != NULL && !g_str_equal (mm_port_get_device (port), name))
            continue;

        out = g_list_append (out, g_object_ref (port));
    }

    return out;
}

static void
initialize_ready (MMBaseModem *self,
                  GAsyncResult *res)
{
    GError *error = NULL;

    if (mm_base_modem_initialize_finish (self, res, &error)) {
        mm_dbg ("modem properly initialized");
        mm_base_modem_set_valid (self, TRUE);
        return;
    }

    /* Wrong state is returned when modem is found locked */
    if (g_error_matches (error, MM_CORE_ERROR, MM_CORE_ERROR_WRONG_STATE)) {
        /* Even with initialization errors, we do set the state to valid, so
         * that the modem gets exported and the failure notified to the user.
         */
        mm_dbg ("Couldn't finish initialization in the current state: '%s'",
                error->message);
        g_error_free (error);
        mm_base_modem_set_valid (self, TRUE);
        return;
    }

    /* Really fatal, we cannot even export the failed modem (e.g. error before
     * even trying to enable the Modem interface */
    mm_warn ("couldn't initialize the modem: '%s'", error->message);
    g_error_free (error);
}

static inline void
log_port (MMBaseModem *self, MMPort *port, const char *desc)
{
    if (port) {
        mm_dbg ("(%s) %s/%s %s",
                self->priv->device,
                mm_port_subsys_get_string (mm_port_get_subsys (port)),
                mm_port_get_device (port),
                desc);
    }
}

gboolean
mm_base_modem_organize_ports (MMBaseModem *self,
                              GError **error)
{
    GHashTableIter iter;
    MMPort *candidate;
    MMPortSerialAtFlag flags;
    MMPortSerialAt *backup_primary = NULL;
    MMPortSerialAt *primary = NULL;
    MMPortSerialAt *secondary = NULL;
    MMPortSerialAt *backup_secondary = NULL;
    MMPortSerialQcdm *qcdm = NULL;
    MMPortSerialAt *gps_control = NULL;
    MMPortSerialGps *gps = NULL;
    MMPort *data_primary = NULL;
    GList *data = NULL;
#if defined WITH_QMI
    MMPort *qmi_primary = NULL;
    GList *qmi = NULL;
#endif
#if defined WITH_MBIM
    MMPort *mbim_primary = NULL;
    GList *mbim = NULL;
#endif
    GList *l;

    g_return_val_if_fail (MM_IS_BASE_MODEM (self), FALSE);

    /* If ports have already been organized, just return success */
    if (self->priv->primary)
        return TRUE;

    g_hash_table_iter_init (&iter, self->priv->ports);
    while (g_hash_table_iter_next (&iter, NULL, (gpointer *) &candidate)) {
        switch (mm_port_get_port_type (candidate)) {

        case MM_PORT_TYPE_AT:
            g_assert (MM_IS_PORT_SERIAL_AT (candidate));
            flags = mm_port_serial_at_get_flags (MM_PORT_SERIAL_AT (candidate));

            if (flags & MM_PORT_SERIAL_AT_FLAG_PRIMARY) {
                if (!primary)
                    primary = MM_PORT_SERIAL_AT (candidate);
                else if (!backup_primary) {
                    /* Just in case the plugin gave us more than one primary
                     * and no secondaries, treat additional primary ports as
                     * secondary.
                     */
                    backup_primary = MM_PORT_SERIAL_AT (candidate);
                }
            }

            if (flags & MM_PORT_SERIAL_AT_FLAG_PPP) {
                if (!data_primary)
                    data_primary = candidate;
                else
                    data = g_list_append (data, candidate);
            }

            /* Explicitly flagged secondary ports trump NONE ports for secondary */
            if (flags & MM_PORT_SERIAL_AT_FLAG_SECONDARY) {
                if (!secondary || !(mm_port_serial_at_get_flags (secondary) & MM_PORT_SERIAL_AT_FLAG_SECONDARY))
                    secondary = MM_PORT_SERIAL_AT (candidate);
            }

            if (flags & MM_PORT_SERIAL_AT_FLAG_GPS_CONTROL) {
                if (!gps_control)
                    gps_control = MM_PORT_SERIAL_AT (candidate);
            }

            /* Fallback secondary */
            if (flags == MM_PORT_SERIAL_AT_FLAG_NONE) {
                if (!secondary)
                    secondary = MM_PORT_SERIAL_AT (candidate);
                else if (!backup_secondary)
                    backup_secondary = MM_PORT_SERIAL_AT (candidate);
            }
            break;

        case MM_PORT_TYPE_QCDM:
            g_assert (MM_IS_PORT_SERIAL_QCDM (candidate));
            if (!qcdm)
                qcdm = MM_PORT_SERIAL_QCDM (candidate);
            break;

        case MM_PORT_TYPE_NET:
            if (!data_primary)
                data_primary = candidate;
            else if (MM_IS_PORT_SERIAL_AT (data_primary)) {
                /* Net device (if any) is the preferred data port */
                data = g_list_append (data, data_primary);
                data_primary = candidate;
            }
            else
                /* All non-primary net ports get added to the list of data ports */
                data = g_list_append (data, candidate);
            break;

        case MM_PORT_TYPE_GPS:
            g_assert (MM_IS_PORT_SERIAL_GPS (candidate));
            if (!gps)
                gps = MM_PORT_SERIAL_GPS (candidate);
            break;

#if defined WITH_QMI
        case MM_PORT_TYPE_QMI:
            if (!qmi_primary)
                qmi_primary = candidate;
            else
                /* All non-primary QMI ports get added to the list of QMI ports */
                qmi = g_list_append (qmi, candidate);
            break;
#endif

#if defined WITH_MBIM
        case MM_PORT_TYPE_MBIM:
            if (!mbim_primary)
                mbim_primary = candidate;
            else
                /* All non-primary MBIM ports get added to the list of MBIM ports */
                mbim = g_list_append (mbim, candidate);
            break;
#endif

        default:
            /* Ignore port */
            break;
        }
    }

    if (!primary) {
        /* Fall back to a secondary port if we didn't find a primary port */
        if (secondary) {
            primary = secondary;
            secondary = NULL;
        }
        /* Fallback to a data port if no primary or secondary */
        else if (data_primary && MM_IS_PORT_SERIAL_AT (data_primary)) {
            primary = MM_PORT_SERIAL_AT (data_primary);
            data_primary = NULL;
        }
        else {
            gboolean allow_modem_without_at_port = FALSE;

#if defined WITH_QMI
            if (qmi_primary)
                allow_modem_without_at_port = TRUE;
#endif

#if defined WITH_MBIM
            if (mbim_primary)
                allow_modem_without_at_port = TRUE;
#endif

            if (!allow_modem_without_at_port) {
                g_set_error_literal (error,
                                     MM_CORE_ERROR,
                                     MM_CORE_ERROR_FAILED,
                                     "Failed to find primary AT port");
                return FALSE;
            }
        }
    }

    /* If the plugin didn't give us any secondary ports, use any additional
     * primary ports or backup secondary ports as secondary.
     */
    if (!secondary)
        secondary = backup_primary ? backup_primary : backup_secondary;

#if defined WITH_QMI
    /* On QMI-based modems, we need to have at least a net port */
    if (qmi_primary && !data_primary) {
        g_set_error_literal (error,
                             MM_CORE_ERROR,
                             MM_CORE_ERROR_FAILED,
                             "Failed to find a net port in the QMI modem");
        return FALSE;
    }
#endif

#if defined WITH_MBIM
    /* On MBIM-based modems, we need to have at least a net port */
    if (mbim_primary && !data_primary) {
        g_set_error_literal (error,
                             MM_CORE_ERROR,
                             MM_CORE_ERROR_FAILED,
                             "Failed to find a net port in the MBIM modem");
        return FALSE;
    }
#endif

    /* Data port defaults to primary AT port */
    if (!data_primary)
        data_primary = MM_PORT (primary);
    g_assert (data_primary);

    /* Reset flags on all ports; clear data port first since it might also
     * be the primary or secondary port.
     */
    if (MM_IS_PORT_SERIAL_AT (data_primary))
        mm_port_serial_at_set_flags (MM_PORT_SERIAL_AT (data_primary), MM_PORT_SERIAL_AT_FLAG_NONE);

    if (primary)
        mm_port_serial_at_set_flags (primary, MM_PORT_SERIAL_AT_FLAG_PRIMARY);
    if (secondary)
        mm_port_serial_at_set_flags (secondary, MM_PORT_SERIAL_AT_FLAG_SECONDARY);

    if (MM_IS_PORT_SERIAL_AT (data_primary)) {
        flags = mm_port_serial_at_get_flags (MM_PORT_SERIAL_AT (data_primary));
        mm_port_serial_at_set_flags (MM_PORT_SERIAL_AT (data_primary), flags | MM_PORT_SERIAL_AT_FLAG_PPP);
    }

    log_port (self, MM_PORT (primary),      "at (primary)");
    log_port (self, MM_PORT (secondary),    "at (secondary)");
    log_port (self, MM_PORT (data_primary), "data (primary)");
    for (l = data; l; l = g_list_next (l))
        log_port (self, MM_PORT (l->data),  "data (secondary)");
    log_port (self, MM_PORT (qcdm),         "qcdm");
    log_port (self, MM_PORT (gps_control),  "gps (control)");
    log_port (self, MM_PORT (gps),          "gps (nmea)");
#if defined WITH_QMI
    log_port (self, MM_PORT (qmi_primary),  "qmi (primary)");
    for (l = qmi; l; l = g_list_next (l))
        log_port (self, MM_PORT (l->data),  "qmi (secondary)");
#endif
#if defined WITH_MBIM
    log_port (self, MM_PORT (mbim_primary),  "mbim (primary)");
    for (l = mbim; l; l = g_list_next (l))
        log_port (self, MM_PORT (l->data),   "mbim (secondary)");
#endif

    /* We keep new refs to the objects here */
    self->priv->primary = (primary ? g_object_ref (primary) : NULL);
    self->priv->secondary = (secondary ? g_object_ref (secondary) : NULL);
    self->priv->qcdm = (qcdm ? g_object_ref (qcdm) : NULL);
    self->priv->gps_control = (gps_control ? g_object_ref (gps_control) : NULL);
    self->priv->gps = (gps ? g_object_ref (gps) : NULL);

    /* Build the final list of data ports, primary port first */
    self->priv->data = g_list_append (self->priv->data, g_object_ref (data_primary));
    g_list_foreach (data, (GFunc)g_object_ref, NULL);
    self->priv->data = g_list_concat (self->priv->data, data);

#if defined WITH_QMI
    /* Build the final list of QMI ports, primary port first */
    if (qmi_primary) {
        self->priv->qmi = g_list_append (self->priv->qmi, g_object_ref (qmi_primary));
        g_list_foreach (qmi, (GFunc)g_object_ref, NULL);
        self->priv->qmi = g_list_concat (self->priv->qmi, qmi);
    }
#endif

#if defined WITH_MBIM
    /* Build the final list of MBIM ports, primary port first */
    if (mbim_primary) {
        self->priv->mbim = g_list_append (self->priv->mbim, g_object_ref (mbim_primary));
        g_list_foreach (mbim, (GFunc)g_object_ref, NULL);
        self->priv->mbim = g_list_concat (self->priv->mbim, mbim);
    }
#endif

    /* As soon as we get the ports organized, we initialize the modem */
    mm_base_modem_initialize (self,
                              (GAsyncReadyCallback)initialize_ready,
                              NULL);

    return TRUE;
}

/*****************************************************************************/
/* Authorization */

gboolean
mm_base_modem_authorize_finish (MMBaseModem *self,
                                GAsyncResult *res,
                                GError **error)
{
    return !g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error);
}

static void
authorize_ready (MMAuthProvider *authp,
                 GAsyncResult *res,
                 GSimpleAsyncResult *simple)
{
    GError *error = NULL;

    if (!mm_auth_provider_authorize_finish (authp, res, &error))
        g_simple_async_result_take_error (simple, error);
    else
        g_simple_async_result_set_op_res_gboolean (simple, TRUE);

    g_simple_async_result_complete (simple);
    g_object_unref (simple);
}

void
mm_base_modem_authorize (MMBaseModem *self,
                         GDBusMethodInvocation *invocation,
                         const gchar *authorization,
                         GAsyncReadyCallback callback,
                         gpointer user_data)
{
    GSimpleAsyncResult *result;

    result = g_simple_async_result_new (G_OBJECT (self),
                                        callback,
                                        user_data,
                                        mm_base_modem_authorize);

    /* When running in the session bus for tests, default to always allow */
    if (mm_context_get_test_session ()) {
        g_simple_async_result_set_op_res_gboolean (result, TRUE);
        g_simple_async_result_complete_in_idle (result);
        g_object_unref (result);
        return;
    }

    mm_auth_provider_authorize (self->priv->authp,
                                invocation,
                                authorization,
                                self->priv->authp_cancellable,
                                (GAsyncReadyCallback)authorize_ready,
                                result);
}

/*****************************************************************************/

const gchar *
mm_base_modem_get_device (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return self->priv->device;
}

const gchar **
mm_base_modem_get_drivers (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return (const gchar **)self->priv->drivers;
}

const gchar *
mm_base_modem_get_plugin (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), NULL);

    return self->priv->plugin;
}

guint
mm_base_modem_get_vendor_id  (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), 0);

    return self->priv->vendor_id;
}

guint
mm_base_modem_get_product_id (MMBaseModem *self)
{
    g_return_val_if_fail (MM_IS_BASE_MODEM (self), 0);

    return self->priv->product_id;
}

/*****************************************************************************/

static gboolean
base_modem_invalid_idle (MMBaseModem *self)
{
    /* Ensure the modem is set invalid if we get the modem-wide cancellable
     * cancelled */
    mm_base_modem_set_valid (self, FALSE);
    g_object_unref (self);
    return G_SOURCE_REMOVE;
}

static void
base_modem_cancelled (GCancellable *cancellable,
                      MMBaseModem *self)
{
    /* NOTE: Don't call set_valid() directly here, do it in an idle, and ensure
     * that we pass a valid reference of the modem object as context. */
    g_idle_add ((GSourceFunc)base_modem_invalid_idle, g_object_ref (self));
}

/*****************************************************************************/

static void
mm_base_modem_init (MMBaseModem *self)
{
    /* Initialize private data */
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                              MM_TYPE_BASE_MODEM,
                                              MMBaseModemPrivate);

    /* Setup authorization provider */
    self->priv->authp = mm_auth_get_provider ();
    self->priv->authp_cancellable = g_cancellable_new ();

    /* Setup modem-wide cancellable */
    self->priv->cancellable = g_cancellable_new ();
    self->priv->invalid_if_cancelled =
        g_cancellable_connect (self->priv->cancellable,
                               G_CALLBACK (base_modem_cancelled),
                               self,
                               NULL);

    self->priv->ports = g_hash_table_new_full (g_str_hash,
                                               g_str_equal,
                                               g_free,
                                               g_object_unref);

    /* Initialize properties */
    self->priv->modem_state = MM_MODEM_STATE_UNKNOWN;
    self->priv->modem_3gpp_registration_state = MM_MODEM_3GPP_REGISTRATION_STATE_UNKNOWN;
    self->priv->modem_3gpp_cs_network_supported = TRUE;
    self->priv->modem_3gpp_ps_network_supported = TRUE;
    self->priv->modem_3gpp_eps_network_supported = FALSE;
    self->priv->modem_3gpp_ignored_facility_locks = MM_MODEM_3GPP_FACILITY_NONE;
    self->priv->modem_cdma_cdma1x_registration_state = MM_MODEM_CDMA_REGISTRATION_STATE_UNKNOWN;
    self->priv->modem_cdma_evdo_registration_state = MM_MODEM_CDMA_REGISTRATION_STATE_UNKNOWN;
    self->priv->modem_cdma_cdma1x_network_supported = TRUE;
    self->priv->modem_cdma_evdo_network_supported = TRUE;
    self->priv->modem_messaging_sms_default_storage = MM_SMS_STORAGE_UNKNOWN;
}

static void
set_property (GObject *object,
              guint prop_id,
              const GValue *value,
              GParamSpec *pspec)
{
    MMBaseModem *self = MM_BASE_MODEM (object);

    switch (prop_id) {
    case PROP_VALID:
        mm_base_modem_set_valid (self, g_value_get_boolean (value));
        break;
    case PROP_MAX_TIMEOUTS:
        self->priv->max_timeouts = g_value_get_uint (value);
        break;
    case PROP_DEVICE:
        g_free (self->priv->device);
        self->priv->device = g_value_dup_string (value);
        break;
    case PROP_DRIVERS:
        g_strfreev (self->priv->drivers);
        self->priv->drivers = g_value_dup_boxed (value);
        break;
    case PROP_PLUGIN:
        g_free (self->priv->plugin);
        self->priv->plugin = g_value_dup_string (value);
        break;
    case PROP_VENDOR_ID:
        self->priv->vendor_id = g_value_get_uint (value);
        break;
    case PROP_PRODUCT_ID:
        self->priv->product_id = g_value_get_uint (value);
        break;
    case PROP_CONNECTION:
        g_clear_object (&self->priv->connection);
        self->priv->connection = g_value_dup_object (value);
        break;
    case PROP_MODEM_DBUS_SKELETON:
        g_clear_object (&self->priv->modem_dbus_skeleton);
        self->priv->modem_dbus_skeleton = g_value_dup_object (value);
        break;
    case PROP_MODEM_3GPP_DBUS_SKELETON:
        g_clear_object (&self->priv->modem_3gpp_dbus_skeleton);
        self->priv->modem_3gpp_dbus_skeleton = g_value_dup_object (value);
        break;
    case PROP_MODEM_3GPP_USSD_DBUS_SKELETON:
        g_clear_object (&self->priv->modem_3gpp_ussd_dbus_skeleton);
        self->priv->modem_3gpp_ussd_dbus_skeleton = g_value_dup_object (value);
        break;
    case PROP_MODEM_CDMA_DBUS_SKELETON:
        g_clear_object (&self->priv->modem_cdma_dbus_skeleton);
        self->priv->modem_cdma_dbus_skeleton = g_value_dup_object (value);
        break;
    case PROP_MODEM_SIMPLE_DBUS_SKELETON:
        g_clear_object (&self->priv->modem_simple_dbus_skeleton);
        self->priv->modem_simple_dbus_skeleton = g_value_dup_object (value);
        break;
    case PROP_MODEM_LOCATION_DBUS_SKELETON:
        g_clear_object (&self->priv->modem_location_dbus_skeleton);
        self->priv->modem_location_dbus_skeleton = g_value_dup_object (value);
        break;
    case PROP_MODEM_MESSAGING_DBUS_SKELETON:
        g_clear_object (&self->priv->modem_messaging_dbus_skeleton);
        self->priv->modem_messaging_dbus_skeleton = g_value_dup_object (value);
        break;
    case PROP_MODEM_VOICE_DBUS_SKELETON:
        g_clear_object (&self->priv->modem_voice_dbus_skeleton);
        self->priv->modem_voice_dbus_skeleton = g_value_dup_object (value);
        break;
    case PROP_MODEM_TIME_DBUS_SKELETON:
        g_clear_object (&self->priv->modem_time_dbus_skeleton);
        self->priv->modem_time_dbus_skeleton = g_value_dup_object (value);
        break;
    case PROP_MODEM_SIGNAL_DBUS_SKELETON:
        g_clear_object (&self->priv->modem_signal_dbus_skeleton);
        self->priv->modem_signal_dbus_skeleton = g_value_dup_object (value);
        break;
    case PROP_MODEM_OMA_DBUS_SKELETON:
        g_clear_object (&self->priv->modem_oma_dbus_skeleton);
        self->priv->modem_oma_dbus_skeleton = g_value_dup_object (value);
        break;
    case PROP_MODEM_FIRMWARE_DBUS_SKELETON:
        g_clear_object (&self->priv->modem_firmware_dbus_skeleton);
        self->priv->modem_firmware_dbus_skeleton = g_value_dup_object (value);
        break;
    case PROP_MODEM_SIM:
        g_clear_object (&self->priv->modem_sim);
        self->priv->modem_sim = g_value_dup_object (value);
        break;
    case PROP_MODEM_BEARER_LIST:
        g_clear_object (&self->priv->modem_bearer_list);
        self->priv->modem_bearer_list = g_value_dup_object (value);
        break;
    case PROP_MODEM_STATE:
        self->priv->modem_state = g_value_get_enum (value);
        break;
    case PROP_MODEM_3GPP_REGISTRATION_STATE:
        self->priv->modem_3gpp_registration_state = g_value_get_enum (value);
        break;
    case PROP_MODEM_3GPP_CS_NETWORK_SUPPORTED:
        self->priv->modem_3gpp_cs_network_supported = g_value_get_boolean (value);
        break;
    case PROP_MODEM_3GPP_PS_NETWORK_SUPPORTED:
        self->priv->modem_3gpp_ps_network_supported = g_value_get_boolean (value);
        break;
    case PROP_MODEM_3GPP_EPS_NETWORK_SUPPORTED:
        self->priv->modem_3gpp_eps_network_supported = g_value_get_boolean (value);
        break;
    case PROP_MODEM_3GPP_IGNORED_FACILITY_LOCKS:
        self->priv->modem_3gpp_ignored_facility_locks = g_value_get_flags (value);
        break;
    case PROP_MODEM_CDMA_CDMA1X_REGISTRATION_STATE:
        self->priv->modem_cdma_cdma1x_registration_state = g_value_get_enum (value);
        break;
    case PROP_MODEM_CDMA_EVDO_REGISTRATION_STATE:
        self->priv->modem_cdma_evdo_registration_state = g_value_get_enum (value);
        break;
    case PROP_MODEM_CDMA_CDMA1X_NETWORK_SUPPORTED:
        self->priv->modem_cdma_cdma1x_network_supported = g_value_get_boolean (value);
        break;
    case PROP_MODEM_CDMA_EVDO_NETWORK_SUPPORTED:
        self->priv->modem_cdma_evdo_network_supported = g_value_get_boolean (value);
        break;
    case PROP_MODEM_MESSAGING_SMS_LIST:
        g_clear_object (&self->priv->modem_messaging_sms_list);
        self->priv->modem_messaging_sms_list = g_value_dup_object (value);
        break;
    case PROP_MODEM_VOICE_CALL_LIST:
        g_clear_object (&self->priv->modem_voice_call_list);
        self->priv->modem_voice_call_list = g_value_dup_object (value);
        break;
    case PROP_MODEM_MESSAGING_SMS_PDU_MODE:
        self->priv->modem_messaging_sms_pdu_mode = g_value_get_boolean (value);
        break;
    case PROP_MODEM_MESSAGING_SMS_DEFAULT_STORAGE:
        self->priv->modem_messaging_sms_default_storage = g_value_get_enum (value);
        break;
    case PROP_MODEM_SIMPLE_STATUS:
        g_clear_object (&self->priv->modem_simple_status);
        self->priv->modem_simple_status = g_value_dup_object (value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject *object,
              guint prop_id,
              GValue *value,
              GParamSpec *pspec)
{
    MMBaseModem *self = MM_BASE_MODEM (object);

    switch (prop_id) {
    case PROP_VALID:
        g_value_set_boolean (value, self->priv->valid);
        break;
    case PROP_MAX_TIMEOUTS:
        g_value_set_uint (value, self->priv->max_timeouts);
        break;
    case PROP_DEVICE:
        g_value_set_string (value, self->priv->device);
        break;
    case PROP_DRIVERS:
        g_value_set_boxed (value, self->priv->drivers);
        break;
    case PROP_PLUGIN:
        g_value_set_string (value, self->priv->plugin);
        break;
    case PROP_VENDOR_ID:
        g_value_set_uint (value, self->priv->vendor_id);
        break;
    case PROP_PRODUCT_ID:
        g_value_set_uint (value, self->priv->product_id);
        break;
    case PROP_CONNECTION:
        g_value_set_object (value, self->priv->connection);
        break;
    case PROP_MODEM_DBUS_SKELETON:
        g_value_set_object (value, self->priv->modem_dbus_skeleton);
        break;
    case PROP_MODEM_3GPP_DBUS_SKELETON:
        g_value_set_object (value, self->priv->modem_3gpp_dbus_skeleton);
        break;
    case PROP_MODEM_3GPP_USSD_DBUS_SKELETON:
        g_value_set_object (value, self->priv->modem_3gpp_ussd_dbus_skeleton);
        break;
    case PROP_MODEM_CDMA_DBUS_SKELETON:
        g_value_set_object (value, self->priv->modem_cdma_dbus_skeleton);
        break;
    case PROP_MODEM_SIMPLE_DBUS_SKELETON:
        g_value_set_object (value, self->priv->modem_simple_dbus_skeleton);
        break;
    case PROP_MODEM_LOCATION_DBUS_SKELETON:
        g_value_set_object (value, self->priv->modem_location_dbus_skeleton);
        break;
    case PROP_MODEM_MESSAGING_DBUS_SKELETON:
        g_value_set_object (value, self->priv->modem_messaging_dbus_skeleton);
        break;
    case PROP_MODEM_VOICE_DBUS_SKELETON:
        g_value_set_object (value, self->priv->modem_voice_dbus_skeleton);
        break;
    case PROP_MODEM_TIME_DBUS_SKELETON:
        g_value_set_object (value, self->priv->modem_time_dbus_skeleton);
        break;
    case PROP_MODEM_SIGNAL_DBUS_SKELETON:
        g_value_set_object (value, self->priv->modem_signal_dbus_skeleton);
        break;
    case PROP_MODEM_OMA_DBUS_SKELETON:
        g_value_set_object (value, self->priv->modem_oma_dbus_skeleton);
        break;
    case PROP_MODEM_FIRMWARE_DBUS_SKELETON:
        g_value_set_object (value, self->priv->modem_firmware_dbus_skeleton);
        break;
    case PROP_MODEM_SIM:
        g_value_set_object (value, self->priv->modem_sim);
        break;
    case PROP_MODEM_BEARER_LIST:
        g_value_set_object (value, self->priv->modem_bearer_list);
        break;
    case PROP_MODEM_STATE:
        g_value_set_enum (value, self->priv->modem_state);
        break;
    case PROP_MODEM_3GPP_REGISTRATION_STATE:
        g_value_set_enum (value, self->priv->modem_3gpp_registration_state);
        break;
    case PROP_MODEM_3GPP_CS_NETWORK_SUPPORTED:
        g_value_set_boolean (value, self->priv->modem_3gpp_cs_network_supported);
        break;
    case PROP_MODEM_3GPP_PS_NETWORK_SUPPORTED:
        g_value_set_boolean (value, self->priv->modem_3gpp_ps_network_supported);
        break;
    case PROP_MODEM_3GPP_EPS_NETWORK_SUPPORTED:
        g_value_set_boolean (value, self->priv->modem_3gpp_eps_network_supported);
        break;
    case PROP_MODEM_3GPP_IGNORED_FACILITY_LOCKS:
        g_value_set_flags (value, self->priv->modem_3gpp_ignored_facility_locks);
        break;
    case PROP_MODEM_CDMA_CDMA1X_REGISTRATION_STATE:
        g_value_set_enum (value, self->priv->modem_cdma_cdma1x_registration_state);
        break;
    case PROP_MODEM_CDMA_EVDO_REGISTRATION_STATE:
        g_value_set_enum (value, self->priv->modem_cdma_evdo_registration_state);
        break;
    case PROP_MODEM_CDMA_CDMA1X_NETWORK_SUPPORTED:
        g_value_set_boolean (value, self->priv->modem_cdma_cdma1x_network_supported);
        break;
    case PROP_MODEM_CDMA_EVDO_NETWORK_SUPPORTED:
        g_value_set_boolean (value, self->priv->modem_cdma_evdo_network_supported);
        break;
    case PROP_MODEM_MESSAGING_SMS_LIST:
        g_value_set_object (value, self->priv->modem_messaging_sms_list);
        break;
    case PROP_MODEM_VOICE_CALL_LIST:
        g_value_set_object (value, self->priv->modem_voice_call_list);
        break;
    case PROP_MODEM_MESSAGING_SMS_PDU_MODE:
        g_value_set_boolean (value, self->priv->modem_messaging_sms_pdu_mode);
        break;
    case PROP_MODEM_MESSAGING_SMS_DEFAULT_STORAGE:
        g_value_set_enum (value, self->priv->modem_messaging_sms_default_storage);
        break;
    case PROP_MODEM_SIMPLE_STATUS:
        g_value_set_object (value, self->priv->modem_simple_status);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
finalize (GObject *object)
{
    MMBaseModem *self = MM_BASE_MODEM (object);

    /* TODO
     * mm_auth_provider_cancel_for_owner (self->priv->authp, object);
    */

    mm_dbg ("Modem (%s) '%s' completely disposed",
            self->priv->plugin,
            self->priv->device);

    g_free (self->priv->device);
    g_strfreev (self->priv->drivers);
    g_free (self->priv->plugin);

    G_OBJECT_CLASS (mm_base_modem_parent_class)->finalize (object);
}

static void
dispose (GObject *object)
{
    MMBaseModem *self = MM_BASE_MODEM (object);

    /* Cancel all ongoing auth requests */
    g_cancellable_cancel (self->priv->authp_cancellable);
    g_clear_object (&self->priv->authp_cancellable);
    g_clear_object (&self->priv->authp);

    /* Ensure we cancel any ongoing operation, but before
     * disconnect our own signal handler, or we'll end up with
     * another reference of the modem object around. */
    g_cancellable_disconnect (self->priv->cancellable,
                              self->priv->invalid_if_cancelled);
    g_cancellable_cancel (self->priv->cancellable);
    g_clear_object (&self->priv->cancellable);

    if (self->priv->modem_dbus_skeleton) {
        mm_iface_modem_shutdown (MM_IFACE_MODEM (object));
        g_clear_object (&self->priv->modem_dbus_skeleton);
    }

    if (self->priv->modem_3gpp_dbus_skeleton) {
        mm_iface_modem_3gpp_shutdown (MM_IFACE_MODEM_3GPP (object));
        g_clear_object (&self->priv->modem_3gpp_dbus_skeleton);
    }

    if (self->priv->modem_3gpp_ussd_dbus_skeleton) {
        mm_iface_modem_3gpp_ussd_shutdown (MM_IFACE_MODEM_3GPP_USSD (object));
        g_clear_object (&self->priv->modem_3gpp_ussd_dbus_skeleton);
    }

    if (self->priv->modem_cdma_dbus_skeleton) {
        mm_iface_modem_cdma_shutdown (MM_IFACE_MODEM_CDMA (object));
        g_clear_object (&self->priv->modem_cdma_dbus_skeleton);
    }

    if (self->priv->modem_location_dbus_skeleton) {
        mm_iface_modem_location_shutdown (MM_IFACE_MODEM_LOCATION (object));
        g_clear_object (&self->priv->modem_location_dbus_skeleton);
    }

    if (self->priv->modem_messaging_dbus_skeleton) {
        mm_iface_modem_messaging_shutdown (MM_IFACE_MODEM_MESSAGING (object));
        g_clear_object (&self->priv->modem_messaging_dbus_skeleton);
    }

    if (self->priv->modem_voice_dbus_skeleton) {
        mm_iface_modem_voice_shutdown (MM_IFACE_MODEM_VOICE (object));
        g_clear_object (&self->priv->modem_voice_dbus_skeleton);
    }

    if (self->priv->modem_time_dbus_skeleton) {
        mm_iface_modem_time_shutdown (MM_IFACE_MODEM_TIME (object));
        g_clear_object (&self->priv->modem_time_dbus_skeleton);
    }

    if (self->priv->modem_simple_dbus_skeleton) {
        mm_iface_modem_simple_shutdown (MM_IFACE_MODEM_SIMPLE (object));
        g_clear_object (&self->priv->modem_simple_dbus_skeleton);
    }

    g_clear_object (&self->priv->modem_sim);
    g_clear_object (&self->priv->modem_bearer_list);
    g_clear_object (&self->priv->modem_messaging_sms_list);
    g_clear_object (&self->priv->modem_voice_call_list);
    g_clear_object (&self->priv->modem_simple_status);

    g_clear_object (&self->priv->primary);
    g_clear_object (&self->priv->secondary);
    g_list_free_full (self->priv->data, g_object_unref);
    self->priv->data = NULL;
    g_clear_object (&self->priv->qcdm);
    g_clear_object (&self->priv->gps_control);
    g_clear_object (&self->priv->gps);
#if defined WITH_QMI
    /* We need to close the QMI port cleanly when disposing the modem object,
     * otherwise the allocated CIDs will be kept allocated, and if we end up
     * allocating too many newer allocations will fail with client-ids-exhausted
     * errors. */
    g_list_foreach (self->priv->qmi, (GFunc)mm_port_qmi_close, NULL);
    g_list_free_full (self->priv->qmi, g_object_unref);
    self->priv->qmi = NULL;
#endif
#if defined WITH_MBIM
    /* We need to close the MBIM port cleanly when disposing the modem object */
    g_list_foreach (self->priv->mbim, (GFunc)mm_port_mbim_close, NULL);
    g_list_free_full (self->priv->mbim, g_object_unref);
    self->priv->mbim = NULL;
#endif

    if (self->priv->ports) {
        g_hash_table_destroy (self->priv->ports);
        self->priv->ports = NULL;
    }

    g_clear_object (&self->priv->connection);

    G_OBJECT_CLASS (mm_base_modem_parent_class)->dispose (object);
}

static void
mm_base_modem_class_init (MMBaseModemClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (object_class, sizeof (MMBaseModemPrivate));

    /* Virtual methods */
    object_class->get_property = get_property;
    object_class->set_property = set_property;
    object_class->finalize = finalize;
    object_class->dispose = dispose;

    properties[PROP_MAX_TIMEOUTS] =
        g_param_spec_uint (MM_BASE_MODEM_MAX_TIMEOUTS,
                           "Max timeouts",
                           "Maximum number of consecutive timed out commands sent to "
                           "the modem before disabling it. If 0, this feature is disabled.",
                           0, G_MAXUINT, 0,
                           G_PARAM_READWRITE);
    g_object_class_install_property (object_class, PROP_MAX_TIMEOUTS, properties[PROP_MAX_TIMEOUTS]);

    properties[PROP_VALID] =
        g_param_spec_boolean (MM_BASE_MODEM_VALID,
                              "Valid",
                              "Whether the modem is to be considered valid or not.",
                              FALSE,
                              G_PARAM_READWRITE);
    g_object_class_install_property (object_class, PROP_VALID, properties[PROP_VALID]);

    properties[PROP_DEVICE] =
        g_param_spec_string (MM_BASE_MODEM_DEVICE,
                             "Device",
                             "Master modem parent device of all the modem's ports",
                             NULL,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_DEVICE, properties[PROP_DEVICE]);

    properties[PROP_DRIVERS] =
        g_param_spec_boxed (MM_BASE_MODEM_DRIVERS,
                            "Drivers",
                            "Kernel drivers",
                            G_TYPE_STRV,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_DRIVERS, properties[PROP_DRIVERS]);

    properties[PROP_PLUGIN] =
        g_param_spec_string (MM_BASE_MODEM_PLUGIN,
                             "Plugin",
                             "Name of the plugin managing this modem",
                             NULL,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_PLUGIN, properties[PROP_PLUGIN]);

    properties[PROP_VENDOR_ID] =
        g_param_spec_uint (MM_BASE_MODEM_VENDOR_ID,
                           "Hardware vendor ID",
                           "Hardware vendor ID. May be unknown for serial devices.",
                           0, G_MAXUINT, 0,
                           G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_VENDOR_ID, properties[PROP_VENDOR_ID]);

    properties[PROP_PRODUCT_ID] =
        g_param_spec_uint (MM_BASE_MODEM_PRODUCT_ID,
                           "Hardware product ID",
                           "Hardware product ID. May be unknown for serial devices.",
                           0, G_MAXUINT, 0,
                           G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_PRODUCT_ID, properties[PROP_PRODUCT_ID]);

    properties[PROP_CONNECTION] =
        g_param_spec_object (MM_BASE_MODEM_CONNECTION,
                             "Connection",
                             "GDBus connection to the system bus.",
                             G_TYPE_DBUS_CONNECTION,
                             G_PARAM_READWRITE);
    g_object_class_install_property (object_class, PROP_CONNECTION, properties[PROP_CONNECTION]);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_DBUS_SKELETON,
                                      MM_IFACE_MODEM_DBUS_SKELETON);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_3GPP_DBUS_SKELETON,
                                      MM_IFACE_MODEM_3GPP_DBUS_SKELETON);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_3GPP_USSD_DBUS_SKELETON,
                                      MM_IFACE_MODEM_3GPP_USSD_DBUS_SKELETON);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_CDMA_DBUS_SKELETON,
                                      MM_IFACE_MODEM_CDMA_DBUS_SKELETON);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_SIMPLE_DBUS_SKELETON,
                                      MM_IFACE_MODEM_SIMPLE_DBUS_SKELETON);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_LOCATION_DBUS_SKELETON,
                                      MM_IFACE_MODEM_LOCATION_DBUS_SKELETON);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_MESSAGING_DBUS_SKELETON,
                                      MM_IFACE_MODEM_MESSAGING_DBUS_SKELETON);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_VOICE_DBUS_SKELETON,
                                      MM_IFACE_MODEM_VOICE_DBUS_SKELETON);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_TIME_DBUS_SKELETON,
                                      MM_IFACE_MODEM_TIME_DBUS_SKELETON);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_SIGNAL_DBUS_SKELETON,
                                      MM_IFACE_MODEM_SIGNAL_DBUS_SKELETON);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_OMA_DBUS_SKELETON,
                                      MM_IFACE_MODEM_OMA_DBUS_SKELETON);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_FIRMWARE_DBUS_SKELETON,
                                      MM_IFACE_MODEM_FIRMWARE_DBUS_SKELETON);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_SIM,
                                      MM_IFACE_MODEM_SIM);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_BEARER_LIST,
                                      MM_IFACE_MODEM_BEARER_LIST);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_STATE,
                                      MM_IFACE_MODEM_STATE);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_3GPP_REGISTRATION_STATE,
                                      MM_IFACE_MODEM_3GPP_REGISTRATION_STATE);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_3GPP_CS_NETWORK_SUPPORTED,
                                      MM_IFACE_MODEM_3GPP_CS_NETWORK_SUPPORTED);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_3GPP_PS_NETWORK_SUPPORTED,
                                      MM_IFACE_MODEM_3GPP_PS_NETWORK_SUPPORTED);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_3GPP_EPS_NETWORK_SUPPORTED,
                                      MM_IFACE_MODEM_3GPP_EPS_NETWORK_SUPPORTED);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_3GPP_IGNORED_FACILITY_LOCKS,
                                      MM_IFACE_MODEM_3GPP_IGNORED_FACILITY_LOCKS);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_CDMA_CDMA1X_REGISTRATION_STATE,
                                      MM_IFACE_MODEM_CDMA_CDMA1X_REGISTRATION_STATE);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_CDMA_EVDO_REGISTRATION_STATE,
                                      MM_IFACE_MODEM_CDMA_EVDO_REGISTRATION_STATE);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_CDMA_CDMA1X_NETWORK_SUPPORTED,
                                      MM_IFACE_MODEM_CDMA_CDMA1X_NETWORK_SUPPORTED);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_CDMA_EVDO_NETWORK_SUPPORTED,
                                      MM_IFACE_MODEM_CDMA_EVDO_NETWORK_SUPPORTED);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_MESSAGING_SMS_LIST,
                                      MM_IFACE_MODEM_MESSAGING_SMS_LIST);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_VOICE_CALL_LIST,
                                      MM_IFACE_MODEM_VOICE_CALL_LIST);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_MESSAGING_SMS_PDU_MODE,
                                      MM_IFACE_MODEM_MESSAGING_SMS_PDU_MODE);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_MESSAGING_SMS_DEFAULT_STORAGE,
                                      MM_IFACE_MODEM_MESSAGING_SMS_DEFAULT_STORAGE);

    g_object_class_override_property (object_class,
                                      PROP_MODEM_SIMPLE_STATUS,
                                      MM_IFACE_MODEM_SIMPLE_STATUS);
}

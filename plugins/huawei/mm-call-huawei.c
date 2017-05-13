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
 * Copyright (C) 2015 Riccardo Vangelisti <riccardo.vangelisti@sadel.it>
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include <ModemManager.h>
#define _LIBMM_INSIDE_MM
#include <libmm-glib.h>

#include "mm-log.h"
#include "mm-base-modem-at.h"
#include "mm-call-huawei.h"

G_DEFINE_TYPE (MMCallHuawei, mm_call_huawei, MM_TYPE_BASE_CALL)

enum {
    PROP_0,
    PROP_AUDIO_HZ,
    PROP_AUDIO_BITS,
    PROP_LAST
};

static GParamSpec *properties[PROP_LAST];

struct _MMCallHuaweiPrivate {
    guint audio_hz;
    guint audio_bits;
};

/*****************************************************************************/

static void
call_set_audio_properties (MMCallHuawei *self,
                           MMPort *port,
                           gboolean connected)
{
    MMBaseCall *base_call = MM_BASE_CALL (self);
    MMCallAudioFormat *format = NULL;

    mm_port_set_connected (port, connected);
    mm_base_call_set_audio_port (base_call, port ? mm_port_get_device (port) : NULL);

    if (connected && self->priv->audio_hz && self->priv->audio_bits) {
        gchar *resolution_str = g_strdup_printf ("s%ule", self->priv->audio_bits);

        format = mm_call_audio_format_new ();
        mm_call_audio_format_set_encoding (format, "pcm");
        mm_call_audio_format_set_resolution (format, resolution_str);
        mm_call_audio_format_set_rate (format, self->priv->audio_hz);

        g_free (resolution_str);
    }
    mm_base_call_set_audio_format (base_call, format);
    g_clear_object (&format);
}

/*****************************************************************************/
/* Start the CALL */

static gboolean
call_start_finish (MMBaseCall *self,
                   GAsyncResult *res,
                   GError **error)
{
    return g_task_propagate_boolean (G_TASK (res), error);
}

static void
call_start_audio_ready (MMBaseModem *modem,
                        GAsyncResult *res,
                        GTask *task)
{
    GError *error = NULL;
    const gchar *response = NULL;
    MMPortSerialQcdm *qcdm;

    response = mm_base_modem_at_command_finish (modem, res, &error);
    if (error) {
        mm_dbg ("Couldn't start call audio: '%s'", error->message);
        g_task_return_error (task, error);
        g_object_unref (task);
        return;
    }

    qcdm = mm_base_modem_peek_port_qcdm (modem);
    if (qcdm) {
        /* The QCDM port, if present, switches from QCDM to voice while
         * a voice call is active.
         */
        call_set_audio_properties (
            MM_CALL_HUAWEI (g_task_get_source_object (task)),
            MM_PORT (qcdm),
            TRUE);
    }

    g_task_return_boolean (task, TRUE);
    g_object_unref (task);
}

static void
parent_call_start_ready (MMBaseCall *self,
                         GAsyncResult *res,
                         GTask *task)
{
    GError *error = NULL;

    if (!MM_BASE_CALL_CLASS (mm_call_huawei_parent_class)->start_finish (self, res, &error)) {
        g_task_return_error (task, error);
        g_object_unref (task);
        return;
    }

    /* Enable audio streaming on the audio port */
    mm_base_modem_at_command (MM_BASE_MODEM (g_task_get_task_data (task)),
                              "AT^DDSETEX=2",
                              5,
                              FALSE,
                              (GAsyncReadyCallback)call_start_audio_ready,
                              task);
}

static void
call_start (MMBaseCall *self,
            GAsyncReadyCallback callback,
            gpointer user_data)
{
    GTask *task;
    MMBaseModem *modem = NULL;

    task = g_task_new (self, NULL, callback, user_data);

    g_object_get (self,
                  MM_BASE_CALL_MODEM, &modem,
                  NULL);
    g_task_set_task_data (task, modem, g_object_unref);

    /* Chain up parent's dial sequence */
    MM_BASE_CALL_CLASS (mm_call_huawei_parent_class)->start (
        MM_BASE_CALL (self),
        (GAsyncReadyCallback)parent_call_start_ready,
        task);
}

/*****************************************************************************/
/* Accept the CALL */

static gboolean
call_accept_finish (MMBaseCall *self,
                    GAsyncResult *res,
                    GError **error)
{
    return g_task_propagate_boolean (G_TASK (res), error);
}

static void
call_accept_audio_ready (MMBaseModem *modem,
                         GAsyncResult *res,
                         GTask *task)
{
    GError *error = NULL;
    const gchar *response = NULL;
    MMPortSerialQcdm *qcdm;

    response = mm_base_modem_at_command_finish (modem, res, &error);
    if (error) {
        mm_dbg ("Couldn't start call audio: '%s'", error->message);
        g_task_return_error (task, error);
        g_object_unref (task);
        return;
    }

    qcdm = mm_base_modem_peek_port_qcdm (modem);
    if (qcdm) {
        /* The QCDM port, if present, switches from QCDM to voice while
         * a voice call is active.
         */
        call_set_audio_properties (
            MM_CALL_HUAWEI (g_task_get_source_object (task)),
            MM_PORT (qcdm),
            TRUE);
    }

    g_task_return_boolean (task, TRUE);
    g_object_unref (task);
}

static void
parent_call_accept_ready (MMBaseCall *self,
                          GAsyncResult *res,
                          GTask *task)
{
    GError *error = NULL;

    if (!MM_BASE_CALL_CLASS (mm_call_huawei_parent_class)->accept_finish (self, res, &error)) {
        g_task_return_error (task, error);
        g_object_unref (task);
        return;
    }

    /* Enable audio streaming on the audio port */
    mm_base_modem_at_command (MM_BASE_MODEM (g_task_get_task_data (task)),
                              "AT^DDSETEX=2",
                              5,
                              FALSE,
                              (GAsyncReadyCallback)call_accept_audio_ready,
                              task);
}

static void
call_accept (MMBaseCall *self,
             GAsyncReadyCallback callback,
             gpointer user_data)
{
    GTask *task;
    MMBaseModem *modem = NULL;

    task = g_task_new (self, NULL, callback, user_data);

    g_object_get (self,
                  MM_BASE_CALL_MODEM, &modem,
                  NULL);
    g_task_set_task_data (task, modem, g_object_unref);

    /* Chain up parent's dial sequence */
    MM_BASE_CALL_CLASS (mm_call_huawei_parent_class)->accept (
        MM_BASE_CALL (self),
        (GAsyncReadyCallback)parent_call_accept_ready,
        task);
}

/*****************************************************************************/
/* Hang up the CALL */

static gboolean
call_hangup_finish (MMBaseCall *self,
                    GAsyncResult *res,
                    GError **error)
{
    return g_task_propagate_boolean (G_TASK (res), error);
}

static void
parent_call_hangup_ready (MMBaseCall *self,
                          GAsyncResult *res,
                          GTask *task)
{
    GError *error = NULL;
    MMPortSerialQcdm *qcdm;

    if (!MM_BASE_CALL_CLASS (mm_call_huawei_parent_class)->hangup_finish (self, res, &error)) {
        g_task_return_error (task, error);
        g_object_unref (task);
        return;
    }

    qcdm = mm_base_modem_peek_port_qcdm (MM_BASE_MODEM (g_task_get_task_data (task)));
    if (qcdm) {
        /* Unblock the QCDM port so it can be used for QCDM again */
        call_set_audio_properties (
            MM_CALL_HUAWEI (g_task_get_source_object (task)),
            MM_PORT (qcdm),
            FALSE);
    }

    g_task_return_boolean (task, TRUE);
    g_object_unref (task);
}

static void
call_hangup (MMBaseCall *self,
             GAsyncReadyCallback callback,
             gpointer user_data)
{
    GTask *task;
    MMBaseModem *modem = NULL;

    task = g_task_new (self, NULL, callback, user_data);

    g_object_get (self,
                  MM_BASE_CALL_MODEM, &modem,
                  NULL);
    g_task_set_task_data (task, modem, g_object_unref);

    /* Chain up parent's dial sequence */
    MM_BASE_CALL_CLASS (mm_call_huawei_parent_class)->hangup (
        MM_BASE_CALL (self),
        (GAsyncReadyCallback)parent_call_hangup_ready,
        task);
}

/*****************************************************************************/

MMBaseCall *
mm_call_huawei_new (MMBaseModem *modem,
                    guint audio_hz,
                    guint audio_bits)
{
    return MM_BASE_CALL (g_object_new (MM_TYPE_CALL_HUAWEI,
                                       MM_BASE_CALL_MODEM, modem,
                                       MM_CALL_HUAWEI_AUDIO_HZ, audio_hz,
                                       MM_CALL_HUAWEI_AUDIO_BITS, audio_bits,
                                       NULL));
}

static void
mm_call_huawei_init (MMCallHuawei *self)
{
    /* Initialize private data */
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                              MM_TYPE_CALL_HUAWEI,
                                              MMCallHuaweiPrivate);
}

static void
set_property (GObject *object,
              guint prop_id,
              const GValue *value,
              GParamSpec *pspec)
{
    MMCallHuawei *self = MM_CALL_HUAWEI (object);

    switch (prop_id) {
    case PROP_AUDIO_HZ:
        self->priv->audio_hz = g_value_get_uint (value);
        break;
    case PROP_AUDIO_BITS:
        self->priv->audio_bits = g_value_get_uint (value);
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
    MMCallHuawei *self = MM_CALL_HUAWEI (object);

    switch (prop_id) {
    case PROP_AUDIO_HZ:
        g_value_set_uint (value, self->priv->audio_hz);
        break;
    case PROP_AUDIO_BITS:
        g_value_set_uint (value, self->priv->audio_bits);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
mm_call_huawei_class_init (MMCallHuaweiClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    MMBaseCallClass *base_call_class = MM_BASE_CALL_CLASS (klass);

    g_type_class_add_private (object_class, sizeof (MMCallHuaweiPrivate));

    object_class->get_property = get_property;
    object_class->set_property = set_property;

    base_call_class->start = call_start;
    base_call_class->start_finish = call_start_finish;
    base_call_class->accept = call_accept;
    base_call_class->accept_finish = call_accept_finish;
    base_call_class->hangup = call_hangup;
    base_call_class->hangup_finish = call_hangup_finish;

    properties[PROP_AUDIO_HZ] =
        g_param_spec_uint (MM_CALL_HUAWEI_AUDIO_HZ,
                           "Audio Hz",
                           "Voice call audio hz if call audio is routed via the host",
                           0, 24000, 0,
                           G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_AUDIO_HZ, properties[PROP_AUDIO_HZ]);

    properties[PROP_AUDIO_BITS] =
        g_param_spec_uint (MM_CALL_HUAWEI_AUDIO_BITS,
                           "Audio Bits",
                           "Voice call audio bits if call audio is routed via the host",
                           0, 24, 0,
                           G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_AUDIO_BITS, properties[PROP_AUDIO_BITS]);
}

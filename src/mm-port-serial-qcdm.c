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
 * Copyright (C) 2009 - 2010 Red Hat, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <ModemManager.h>
#include <mm-errors-types.h>

#include "mm-port-serial-qcdm.h"
#include "libqcdm/src/com.h"
#include "libqcdm/src/utils.h"
#include "libqcdm/src/errors.h"
#include "libqcdm/src/commands.h"
#include "libqcdm/src/dm-commands.h"
#include "mm-log.h"
#include "mm-hdlc.h"

G_DEFINE_TYPE (MMPortSerialQcdm, mm_port_serial_qcdm, MM_TYPE_PORT_SERIAL)

struct _MMPortSerialQcdmPrivate {
    GSList *unsolicited_msg_handlers;
};

/*****************************************************************************/

static MMPortSerialResponseType
parse_response (MMPortSerial *port,
                GByteArray *response,
                GByteArray **parsed_response,
                GError **error)
{
    int err_code;

    err_code = qcdm_cmd_check ((const char *) response->data, response->len);
    if (err_code < 0) {
        g_set_error (error,
                     MM_SERIAL_ERROR,
                     MM_SERIAL_ERROR_PARSE_FAILED,
                     "QCDM command error %d",
                     err_code);
        return MM_PORT_SERIAL_RESPONSE_ERROR;
    }

    /* Already decapsulated, just copy command data into parsed buffer */
    *parsed_response = g_byte_array_sized_new (response->len);
    g_byte_array_append (*parsed_response, response->data, response->len);
    return MM_PORT_SERIAL_RESPONSE_BUFFER;
}

/*****************************************************************************/

GByteArray *
mm_port_serial_qcdm_command_finish (MMPortSerialQcdm *self,
                                    GAsyncResult *res,
                                    GError **error)
{
    return g_task_propagate_pointer (G_TASK (res), error);
}

static void
serial_command_ready (MMPortSerial *port,
                      GAsyncResult *res,
                      GTask *task)
{
    GByteArray *response;
    GError *error = NULL;

    response = mm_port_serial_hdlc_command_finish (port, res, &error);
    if (!response)
        g_task_return_error (task, error);
    else
        g_task_return_pointer (task, response, (GDestroyNotify)g_byte_array_unref);

    g_object_unref (task);
}

void
mm_port_serial_qcdm_command (MMPortSerialQcdm *self,
                             GByteArray *command,
                             guint32 timeout_seconds,
                             GCancellable *cancellable,
                             GAsyncReadyCallback callback,
                             gpointer user_data)
{
    GTask *task;

    g_return_if_fail (MM_IS_PORT_SERIAL_QCDM (self));
    g_return_if_fail (command != NULL);

    task = g_task_new (self, cancellable, callback, user_data);

    /* 'command' is expected to be already CRC-ed and escaped */
    mm_port_serial_hdlc_command (MM_PORT_SERIAL (self),
                                 command,
                                 timeout_seconds,
                                 cancellable,
                                 (GAsyncReadyCallback)serial_command_ready,
                                 task);
}

static void
debug_log (MMPortSerial *port,
           gboolean binary,
           const char *prefix,
           const char *buf,
           gsize len)
{
    mm_port_serial_debug_log (port,
                              TRUE, /* QCDM is always binary */
                              prefix,
                              buf,
                              len);
}

/*****************************************************************************/

typedef struct {
    guint log_code;
    MMPortSerialQcdmUnsolicitedMsgFn callback;
    gboolean enable;
    gpointer user_data;
    GDestroyNotify notify;
} MMQcdmUnsolicitedMsgHandler;

static gint
unsolicited_msg_handler_cmp (MMQcdmUnsolicitedMsgHandler *handler,
                             gpointer log_code)
{
    return handler->log_code - GPOINTER_TO_UINT (log_code);
}

void
mm_port_serial_qcdm_add_unsolicited_msg_handler (MMPortSerialQcdm *self,
                                                 guint log_code,
                                                 MMPortSerialQcdmUnsolicitedMsgFn callback,
                                                 gpointer user_data,
                                                 GDestroyNotify notify)
{
    GSList *existing;
    MMQcdmUnsolicitedMsgHandler *handler;

    g_return_if_fail (MM_IS_PORT_SERIAL_QCDM (self));
    g_return_if_fail (log_code > 0 && log_code <= G_MAXUINT16);

    existing = g_slist_find_custom (self->priv->unsolicited_msg_handlers,
                                    GUINT_TO_POINTER (log_code),
                                    (GCompareFunc)unsolicited_msg_handler_cmp);
    if (existing) {
        handler = existing->data;
        /* We OVERWRITE any existing one, so if any context data existing, free it */
        if (handler->notify)
            handler->notify (handler->user_data);
    } else {
        handler = g_slice_new (MMQcdmUnsolicitedMsgHandler);
        self->priv->unsolicited_msg_handlers = g_slist_append (self->priv->unsolicited_msg_handlers, handler);
        handler->log_code = log_code;
    }

    handler->callback = callback;
    handler->enable = TRUE;
    handler->user_data = user_data;
    handler->notify = notify;
}

void
mm_port_serial_qcdm_enable_unsolicited_msg_handler (MMPortSerialQcdm *self,
                                                    guint log_code,
                                                    gboolean enable)
{
    GSList *existing;
    MMQcdmUnsolicitedMsgHandler *handler;

    g_return_if_fail (MM_IS_PORT_SERIAL_QCDM (self));
    g_return_if_fail (log_code > 0 && log_code <= G_MAXUINT16);

    existing = g_slist_find_custom (self->priv->unsolicited_msg_handlers,
                                    GUINT_TO_POINTER (log_code),
                                    (GCompareFunc)unsolicited_msg_handler_cmp);
    if (existing) {
        handler = existing->data;
        handler->enable = enable;
    }
}

static void
parse_unsolicited (MMPortSerial *port, GByteArray *response)
{
    MMPortSerialQcdm *self = MM_PORT_SERIAL_QCDM (port);
    GSList *iter;

    if (response->len < sizeof (DMCmdLog) || response->data[0] != DIAG_CMD_LOG)
        return;

    for (iter = self->priv->unsolicited_msg_handlers; iter; iter = iter->next) {
        MMQcdmUnsolicitedMsgHandler *handler = (MMQcdmUnsolicitedMsgHandler *) iter->data;
        DMCmdLog *log_cmd = (DMCmdLog *) response->data;

        if (!handler->enable)
            continue;
        if (handler->log_code != le16toh (log_cmd->log_code))
            continue;
        if (handler->callback)
            handler->callback (self, response, handler->user_data);
    }
}

/*****************************************************************************/

static gboolean
config_fd (MMPortSerial *port, int fd, GError **error)
{
    int err;

    err = qcdm_port_setup (fd);
    if (err != QCDM_SUCCESS) {
        g_set_error (error, MM_SERIAL_ERROR, MM_SERIAL_ERROR_OPEN_FAILED,
                     "Failed to open QCDM port: %d", err);
        return FALSE;
    }
    return TRUE;
}

/*****************************************************************************/

MMPortSerialQcdm *
mm_port_serial_qcdm_new (const char *name)
{
    return MM_PORT_SERIAL_QCDM (g_object_new (MM_TYPE_PORT_SERIAL_QCDM,
                                              MM_PORT_DEVICE, name,
                                              MM_PORT_SUBSYS, MM_PORT_SUBSYS_TTY,
                                              MM_PORT_TYPE, MM_PORT_TYPE_QCDM,
                                              MM_PORT_SERIAL_SEND_DELAY, (guint64) 0,
                                              MM_PORT_SERIAL_HDLC, TRUE,
                                              NULL));
}

MMPortSerialQcdm *
mm_port_serial_qcdm_new_fd (int fd)
{
    MMPortSerialQcdm *port;
    char *name;

    name = g_strdup_printf ("port%d", fd);
    port = MM_PORT_SERIAL_QCDM (g_object_new (MM_TYPE_PORT_SERIAL_QCDM,
                                              MM_PORT_DEVICE, name,
                                              MM_PORT_SUBSYS, MM_PORT_SUBSYS_TTY,
                                              MM_PORT_TYPE, MM_PORT_TYPE_QCDM,
                                              MM_PORT_SERIAL_FD, fd,
                                              MM_PORT_SERIAL_SEND_DELAY, (guint64) 0,
                                              MM_PORT_SERIAL_HDLC, TRUE,
                                              NULL));
    g_free (name);
    return port;
}

static void
mm_port_serial_qcdm_init (MMPortSerialQcdm *self)
{
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, MM_TYPE_PORT_SERIAL_QCDM, MMPortSerialQcdmPrivate);
}

static void
finalize (GObject *object)
{
    MMPortSerialQcdm *self = MM_PORT_SERIAL_QCDM (object);

    while (self->priv->unsolicited_msg_handlers) {
        MMQcdmUnsolicitedMsgHandler *handler = (MMQcdmUnsolicitedMsgHandler *) self->priv->unsolicited_msg_handlers->data;

        if (handler->notify)
            handler->notify (handler->user_data);

        g_slice_free (MMQcdmUnsolicitedMsgHandler, handler);
        self->priv->unsolicited_msg_handlers = g_slist_delete_link (self->priv->unsolicited_msg_handlers,
                                                                    self->priv->unsolicited_msg_handlers);
    }

    G_OBJECT_CLASS (mm_port_serial_qcdm_parent_class)->finalize (object);
}

static void
mm_port_serial_qcdm_class_init (MMPortSerialQcdmClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    MMPortSerialClass *port_class = MM_PORT_SERIAL_CLASS (klass);

    g_type_class_add_private (object_class, sizeof (MMPortSerialQcdmPrivate));

    /* Virtual methods */
    object_class->finalize = finalize;
    port_class->parse_unsolicited = parse_unsolicited;
    port_class->parse_response = parse_response;
    port_class->config_fd = config_fd;
    port_class->debug_log = debug_log;
}

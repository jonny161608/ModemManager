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
 * Copyright (C) 2009 - 2012 Red Hat, Inc.
 * Copyright (C) 2012 Google, Inc.
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

#include "mm-sms.h"
#include "mm-broadband-modem.h"
#include "mm-iface-modem.h"
#include "mm-iface-modem-messaging.h"
#include "mm-sms-part-3gpp.h"
#include "mm-base-modem-at.h"
#include "mm-base-modem.h"
#include "mm-log.h"
#include "mm-modem-helpers.h"

G_DEFINE_TYPE (MMSms, mm_sms, MM_TYPE_BASE_SMS)

/*****************************************************************************/

static gboolean
sms_get_store_or_send_command (MMSmsPart *part,
                               gboolean text_or_pdu,   /* TRUE for PDU */
                               gboolean store_or_send, /* TRUE for send */
                               gchar **out_cmd,
                               gchar **out_msg_data,
                               GError **error)
{
    g_assert (out_cmd != NULL);
    g_assert (out_msg_data != NULL);

    if (!text_or_pdu) {
        /* Text mode */
        *out_cmd = g_strdup_printf ("+CMG%c=\"%s\"",
                                    store_or_send ? 'S' : 'W',
                                    mm_sms_part_get_number (part));
        *out_msg_data = g_strdup_printf ("%s\x1a", mm_sms_part_get_text (part));
    } else {
        guint8 *pdu;
        guint pdulen = 0;
        guint msgstart = 0;
        gchar *hex;

        /* AT+CMGW=<length>[, <stat>]<CR> PDU can be entered. <CTRL-Z>/<ESC> */

        pdu = mm_sms_part_3gpp_get_submit_pdu (part, &pdulen, &msgstart, error);
        if (!pdu)
            /* 'error' should already be set */
            return FALSE;

        /* Convert PDU to hex */
        hex = mm_utils_bin2hexstr (pdu, pdulen);
        g_free (pdu);

        if (!hex) {
            g_set_error (error,
                         MM_CORE_ERROR,
                         MM_CORE_ERROR_FAILED,
                         "Not enough memory to send SMS PDU");
            return FALSE;
        }

        /* CMGW/S length is the size of the PDU without SMSC information */
        *out_cmd = g_strdup_printf ("+CMG%c=%d",
                                    store_or_send ? 'S' : 'W',
                                    pdulen - msgstart);
        *out_msg_data = g_strdup_printf ("%s\x1a", hex);
        g_free (hex);
    }

    return TRUE;
}

/*****************************************************************************/
/* Store the SMS */

typedef struct {
    MMSms *self;
    MMBaseModem *modem;
    GSimpleAsyncResult *result;
    MMSmsStorage storage;
    gboolean need_unlock;
    gboolean use_pdu_mode;
    GList *current;
    gchar *msg_data;
} SmsStoreContext;

static void
sms_store_context_complete_and_free (SmsStoreContext *ctx)
{
    g_simple_async_result_complete (ctx->result);
    g_object_unref (ctx->result);
    /* Unlock mem2 storage if we had the lock */
    if (ctx->need_unlock)
        mm_broadband_modem_unlock_sms_storages (MM_BROADBAND_MODEM (ctx->modem), FALSE, TRUE);
    g_object_unref (ctx->modem);
    g_object_unref (ctx->self);
    g_free (ctx->msg_data);
    g_free (ctx);
}

static gboolean
sms_store_finish (MMBaseSms *self,
                  GAsyncResult *res,
                  GError **error)
{
    return !g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error);
}

static void sms_store_next_part (SmsStoreContext *ctx);

static void
store_msg_data_ready (MMBaseModem *modem,
                      GAsyncResult *res,
                      SmsStoreContext *ctx)
{
    const gchar *response;
    GError *error = NULL;
    gint rv;
    gint idx;

    response = mm_base_modem_at_command_finish (modem, res, &error);
    if (error) {
        g_simple_async_result_take_error (ctx->result, error);
        sms_store_context_complete_and_free (ctx);
        return;
    }

    /* Read the new part index from the reply */
    rv = sscanf (response, "+CMGW: %d", &idx);
    if (rv != 1 || idx < 0) {
        g_simple_async_result_set_error (ctx->result,
                                         MM_CORE_ERROR,
                                         MM_CORE_ERROR_FAILED,
                                         "Couldn't read index of already stored part: "
                                         "%d fields parsed",
                                         rv);
        sms_store_context_complete_and_free (ctx);
        return;
    }

    /* Set the index in the part we hold */
    mm_sms_part_set_index ((MMSmsPart *)ctx->current->data, (guint)idx);

    ctx->current = g_list_next (ctx->current);
    sms_store_next_part (ctx);
}

static void
store_ready (MMBaseModem *modem,
             GAsyncResult *res,
             SmsStoreContext *ctx)
{
    const gchar *response;
    GError *error = NULL;

    response = mm_base_modem_at_command_finish (modem, res, &error);
    if (error) {
        g_simple_async_result_take_error (ctx->result, error);
        sms_store_context_complete_and_free (ctx);
        return;
    }

    /* Send the actual message data */
    mm_base_modem_at_command_raw (ctx->modem,
                                  ctx->msg_data,
                                  10,
                                  FALSE,
                                  (GAsyncReadyCallback)store_msg_data_ready,
                                  ctx);
}

static void
sms_store_next_part (SmsStoreContext *ctx)
{
    gchar *cmd;
    GError *error = NULL;

    if (!ctx->current) {
        /* Done we are */
        g_simple_async_result_set_op_res_gboolean (ctx->result, TRUE);
        sms_store_context_complete_and_free (ctx);
        return;
    }

    if (ctx->msg_data) {
        g_free (ctx->msg_data);
        ctx->msg_data = NULL;
    }

    if (!sms_get_store_or_send_command ((MMSmsPart *)ctx->current->data,
                                        ctx->use_pdu_mode,
                                        FALSE,
                                        &cmd,
                                        &ctx->msg_data,
                                        &error)) {
        g_simple_async_result_take_error (ctx->result, error);
        sms_store_context_complete_and_free (ctx);
        return;
    }

    g_assert (cmd != NULL);
    g_assert (ctx->msg_data != NULL);

    mm_base_modem_at_command (ctx->modem,
                              cmd,
                              10,
                              FALSE,
                              (GAsyncReadyCallback)store_ready,
                              ctx);
    g_free (cmd);
}

static void
store_lock_sms_storages_ready (MMBroadbandModem *modem,
                               GAsyncResult *res,
                               SmsStoreContext *ctx)
{
    GError *error = NULL;

    if (!mm_broadband_modem_lock_sms_storages_finish (modem, res, &error)) {
        g_simple_async_result_take_error (ctx->result, error);
        sms_store_context_complete_and_free (ctx);
        return;
    }

    /* We are now locked. Whatever result we have here, we need to make sure
     * we unlock the storages before finishing. */
    ctx->need_unlock = TRUE;

    /* Go on to store the parts */
    ctx->current = mm_base_sms_peek_parts (MM_BASE_SMS (ctx->self));
    sms_store_next_part (ctx);
}

static void
sms_store (MMBaseSms *self,
           MMSmsStorage storage,
           GAsyncReadyCallback callback,
           gpointer user_data)
{
    SmsStoreContext *ctx;

    /* Setup the context */
    ctx = g_new0 (SmsStoreContext, 1);
    ctx->result = g_simple_async_result_new (G_OBJECT (self),
                                             callback,
                                             user_data,
                                             sms_store);
    ctx->self = g_object_ref (self);
    ctx->modem = mm_base_sms_peek_modem (self);
    ctx->storage = storage;

    /* Different ways to do it if on PDU or text mode */
    g_object_get (mm_base_sms_peek_modem (self),
                  MM_IFACE_MODEM_MESSAGING_SMS_PDU_MODE, &ctx->use_pdu_mode,
                  NULL);

    /* First, lock storage to use */
    mm_broadband_modem_lock_sms_storages (
        MM_BROADBAND_MODEM (mm_base_sms_peek_modem (self)),
        MM_SMS_STORAGE_UNKNOWN, /* none required for mem1 */
        ctx->storage,
        (GAsyncReadyCallback)store_lock_sms_storages_ready,
        ctx);
}

/*****************************************************************************/
/* Send the SMS */

typedef struct {
    MMSms *self;
    MMBaseModem *modem;
    GSimpleAsyncResult *result;
    gboolean need_unlock;
    gboolean from_storage;
    gboolean use_pdu_mode;
    GList *current;
    gchar *msg_data;
} SmsSendContext;

static void
sms_send_context_complete_and_free (SmsSendContext *ctx)
{
    g_simple_async_result_complete_in_idle (ctx->result);
    g_object_unref (ctx->result);
    /* Unlock mem2 storage if we had the lock */
    if (ctx->need_unlock)
        mm_broadband_modem_unlock_sms_storages (MM_BROADBAND_MODEM (ctx->modem), FALSE, TRUE);
    g_object_unref (ctx->modem);
    g_object_unref (ctx->self);
    g_free (ctx->msg_data);
    g_free (ctx);
}

static gboolean
sms_send_finish (MMBaseSms *self,
                 GAsyncResult *res,
                 GError **error)
{
    return !g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error);
}

static void sms_send_next_part (SmsSendContext *ctx);

static gint
read_message_reference_from_reply (const gchar *response,
                                   GError **error)
{
    gint rv = 0;
    gint idx = -1;

    if (strstr (response, "+CMGS"))
        rv = sscanf (strstr (response, "+CMGS"), "+CMGS: %d", &idx);
    else if (strstr (response, "+CMSS"))
        rv = sscanf (strstr (response, "+CMSS"), "+CMSS: %d", &idx);

    if (rv != 1 || idx < 0) {
        g_set_error (error,
                     MM_CORE_ERROR,
                     MM_CORE_ERROR_FAILED,
                     "Couldn't read message reference: "
                     "%d fields parsed from response '%s'",
                     rv, response);
        return -1;
    }

    return idx;
}

static void
send_generic_msg_data_ready (MMBaseModem *modem,
                             GAsyncResult *res,
                             SmsSendContext *ctx)
{
    GError *error = NULL;
    const gchar *response;
    gint message_reference;

    response = mm_base_modem_at_command_finish (modem, res, &error);
    if (error) {
        g_simple_async_result_take_error (ctx->result, error);
        sms_send_context_complete_and_free (ctx);
        return;
    }

    message_reference = read_message_reference_from_reply (response, &error);
    if (error) {
        g_simple_async_result_take_error (ctx->result, error);
        sms_send_context_complete_and_free (ctx);
        return;
    }

    mm_sms_part_set_message_reference ((MMSmsPart *)ctx->current->data,
                                       (guint)message_reference);

    ctx->current = g_list_next (ctx->current);
    sms_send_next_part (ctx);
}

static void
send_generic_ready (MMBaseModem *modem,
                    GAsyncResult *res,
                    SmsSendContext *ctx)
{
    GError *error = NULL;

    mm_base_modem_at_command_finish (modem, res, &error);
    if (error) {
        g_simple_async_result_take_error (ctx->result, error);
        sms_send_context_complete_and_free (ctx);
        return;
    }

    /* Send the actual message data */
    mm_base_modem_at_command_raw (ctx->modem,
                                  ctx->msg_data,
                                  10,
                                  FALSE,
                                  (GAsyncReadyCallback)send_generic_msg_data_ready,
                                  ctx);
}

static void
send_from_storage_ready (MMBaseModem *modem,
                         GAsyncResult *res,
                         SmsSendContext *ctx)
{
    GError *error = NULL;
    const gchar *response;
    gint message_reference;

    response = mm_base_modem_at_command_finish (modem, res, &error);
    if (error) {
        if (g_error_matches (error, MM_SERIAL_ERROR, MM_SERIAL_ERROR_RESPONSE_TIMEOUT)) {
            g_simple_async_result_take_error (ctx->result, error);
            sms_send_context_complete_and_free (ctx);
            return;
        }

        mm_dbg ("Couldn't send SMS from storage: '%s'; trying generic send...",
                error->message);
        g_error_free (error);

        ctx->from_storage = FALSE;
        sms_send_next_part (ctx);
        return;
    }

    message_reference = read_message_reference_from_reply (response, &error);
    if (error) {
        g_simple_async_result_take_error (ctx->result, error);
        sms_send_context_complete_and_free (ctx);
        return;
    }

    mm_sms_part_set_message_reference ((MMSmsPart *)ctx->current->data,
                                       (guint)message_reference);

    ctx->current = g_list_next (ctx->current);
    sms_send_next_part (ctx);
}

static void
sms_send_next_part (SmsSendContext *ctx)
{
    GError *error = NULL;
    gchar *cmd;

    if (!ctx->current) {
        /* Done we are */
        g_simple_async_result_set_op_res_gboolean (ctx->result, TRUE);
        sms_send_context_complete_and_free (ctx);
        return;
    }

    /* Send from storage */
    if (ctx->from_storage) {
        cmd = g_strdup_printf ("+CMSS=%d",
                               mm_sms_part_get_index ((MMSmsPart *)ctx->current->data));
        mm_base_modem_at_command (ctx->modem,
                                  cmd,
                                  30,
                                  FALSE,
                                  (GAsyncReadyCallback)send_from_storage_ready,
                                  ctx);
        g_free (cmd);
        return;
    }

    /* Generic send */

    if (ctx->msg_data) {
        g_free (ctx->msg_data);
        ctx->msg_data = NULL;
    }

    if (!sms_get_store_or_send_command ((MMSmsPart *)ctx->current->data,
                                        ctx->use_pdu_mode,
                                        TRUE,
                                        &cmd,
                                        &ctx->msg_data,
                                        &error)) {
        g_simple_async_result_take_error (ctx->result, error);
        sms_send_context_complete_and_free (ctx);
        return;
    }

    g_assert (cmd != NULL);
    g_assert (ctx->msg_data != NULL);
    mm_base_modem_at_command (ctx->modem,
                              cmd,
                              30,
                              FALSE,
                              (GAsyncReadyCallback)send_generic_ready,
                              ctx);
    g_free (cmd);
}

static void
send_lock_sms_storages_ready (MMBroadbandModem *modem,
                              GAsyncResult *res,
                              SmsSendContext *ctx)
{
    GError *error = NULL;

    if (!mm_broadband_modem_lock_sms_storages_finish (modem, res, &error)) {
        g_simple_async_result_take_error (ctx->result, error);
        sms_send_context_complete_and_free (ctx);
        return;
    }

    /* We are now locked. Whatever result we have here, we need to make sure
     * we unlock the storages before finishing. */
    ctx->need_unlock = TRUE;

    /* Go on to send the parts */
    ctx->current = mm_base_sms_peek_parts (MM_BASE_SMS (ctx->self));
    sms_send_next_part (ctx);
}

static void
sms_send (MMBaseSms *self,
          GAsyncReadyCallback callback,
          gpointer user_data)
{
    SmsSendContext *ctx;

    /* Setup the context */
    ctx = g_new0 (SmsSendContext, 1);
    ctx->result = g_simple_async_result_new (G_OBJECT (self),
                                             callback,
                                             user_data,
                                             sms_send);
    ctx->self = g_object_ref (self);
    ctx->modem = g_object_ref (mm_base_sms_peek_modem (self));

    /* If the SMS is STORED, try to send from storage */
    ctx->from_storage = (mm_base_sms_get_storage (self) != MM_SMS_STORAGE_UNKNOWN);
    if (ctx->from_storage) {
        /* When sending from storage, first lock storage to use */
        mm_broadband_modem_lock_sms_storages (
            MM_BROADBAND_MODEM (mm_base_sms_peek_modem (self)),
            MM_SMS_STORAGE_UNKNOWN, /* none required for mem1 */
            mm_base_sms_get_storage (self),
            (GAsyncReadyCallback)send_lock_sms_storages_ready,
            ctx);
        return;
    }

    /* Different ways to do it if on PDU or text mode */
    g_object_get (mm_base_sms_peek_modem (self),
                  MM_IFACE_MODEM_MESSAGING_SMS_PDU_MODE, &ctx->use_pdu_mode,
                  NULL);
    ctx->current = mm_base_sms_peek_parts (self);
    sms_send_next_part (ctx);
}

/*****************************************************************************/

typedef struct {
    MMSms *self;
    MMBaseModem *modem;
    GSimpleAsyncResult *result;
    gboolean need_unlock;
    GList *current;
    guint n_failed;
} SmsDeletePartsContext;

static void
sms_delete_parts_context_complete_and_free (SmsDeletePartsContext *ctx)
{
    g_simple_async_result_complete_in_idle (ctx->result);
    g_object_unref (ctx->result);
    /* Unlock mem1 storage if we had the lock */
    if (ctx->need_unlock)
        mm_broadband_modem_unlock_sms_storages (MM_BROADBAND_MODEM (ctx->modem), TRUE, FALSE);
    g_object_unref (ctx->modem);
    g_object_unref (ctx->self);
    g_free (ctx);
}

static gboolean
sms_delete_finish (MMBaseSms *self,
                   GAsyncResult *res,
                   GError **error)
{
    return !g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error);
}

static void delete_next_part (SmsDeletePartsContext *ctx);

static void
delete_part_ready (MMBaseModem *modem,
                   GAsyncResult *res,
                   SmsDeletePartsContext *ctx)
{
    GError *error = NULL;

    mm_base_modem_at_command_finish (modem, res, &error);
    if (error) {
        ctx->n_failed++;
        mm_dbg ("Couldn't delete SMS part with index %u: '%s'",
                mm_sms_part_get_index ((MMSmsPart *)ctx->current->data),
                error->message);
        g_error_free (error);
    }

    /* We reset the index, as there is no longer that part */
    mm_sms_part_set_index ((MMSmsPart *)ctx->current->data, SMS_PART_INVALID_INDEX);

    ctx->current = g_list_next (ctx->current);
    delete_next_part (ctx);
}

static void
delete_next_part (SmsDeletePartsContext *ctx)
{
    gchar *cmd;

    /* Skip non-stored parts */
    while (ctx->current &&
           mm_sms_part_get_index ((MMSmsPart *)ctx->current->data) == SMS_PART_INVALID_INDEX)
        ctx->current = g_list_next (ctx->current);

    /* If all removed, we're done */
    if (!ctx->current) {
        if (ctx->n_failed > 0)
            g_simple_async_result_set_error (ctx->result,
                                             MM_CORE_ERROR,
                                             MM_CORE_ERROR_FAILED,
                                             "Couldn't delete %u parts from this SMS",
                                             ctx->n_failed);
        else
            g_simple_async_result_set_op_res_gboolean (ctx->result, TRUE);

        sms_delete_parts_context_complete_and_free (ctx);
        return;
    }

    cmd = g_strdup_printf ("+CMGD=%d",
                           mm_sms_part_get_index ((MMSmsPart *)ctx->current->data));
    mm_base_modem_at_command (ctx->modem,
                              cmd,
                              10,
                              FALSE,
                              (GAsyncReadyCallback)delete_part_ready,
                              ctx);
    g_free (cmd);
}

static void
delete_lock_sms_storages_ready (MMBroadbandModem *modem,
                                GAsyncResult *res,
                                SmsDeletePartsContext *ctx)
{
    GError *error = NULL;

    if (!mm_broadband_modem_lock_sms_storages_finish (modem, res, &error)) {
        g_simple_async_result_take_error (ctx->result, error);
        sms_delete_parts_context_complete_and_free (ctx);
        return;
    }

    /* We are now locked. Whatever result we have here, we need to make sure
     * we unlock the storages before finishing. */
    ctx->need_unlock = TRUE;

    /* Go on deleting parts */
    ctx->current = mm_base_sms_peek_parts (MM_BASE_SMS (ctx->self));
    delete_next_part (ctx);
}

static void
sms_delete (MMBaseSms *self,
            GAsyncReadyCallback callback,
            gpointer user_data)
{
    SmsDeletePartsContext *ctx;

    ctx = g_new0 (SmsDeletePartsContext, 1);
    ctx->result = g_simple_async_result_new (G_OBJECT (self),
                                             callback,
                                             user_data,
                                             sms_delete);
    ctx->self = g_object_ref (self);
    ctx->modem = g_object_ref (mm_base_sms_peek_modem (self));

    if (mm_base_sms_get_storage (self) == MM_SMS_STORAGE_UNKNOWN) {
        mm_dbg ("Not removing parts from non-stored SMS");
        g_simple_async_result_set_op_res_gboolean (ctx->result, TRUE);
        sms_delete_parts_context_complete_and_free (ctx);
        return;
    }

    /* Select specific storage to delete from */
    mm_broadband_modem_lock_sms_storages (
        MM_BROADBAND_MODEM (mm_base_sms_peek_modem (self)),
        mm_base_sms_get_storage (self),
        MM_SMS_STORAGE_UNKNOWN, /* none required for mem2 */
        (GAsyncReadyCallback)delete_lock_sms_storages_ready,
        ctx);
}

/*****************************************************************************/

MMBaseSms *
mm_sms_new (MMBaseModem *modem)
{
    return MM_BASE_SMS (g_object_new (MM_TYPE_SMS,
                                      MM_BASE_SMS_MODEM, modem,
                                      NULL));
}

/*****************************************************************************/

static void
mm_sms_init (MMSms *self)
{
}

static void
mm_sms_class_init (MMSmsClass *klass)
{
    MMBaseSmsClass *base_sms_class = MM_BASE_SMS_CLASS (klass);

    base_sms_class->store         = sms_store;
    base_sms_class->store_finish  = sms_store_finish;
    base_sms_class->send          = sms_send;
    base_sms_class->send_finish   = sms_send_finish;
    base_sms_class->delete        = sms_delete;
    base_sms_class->delete_finish = sms_delete_finish;
}

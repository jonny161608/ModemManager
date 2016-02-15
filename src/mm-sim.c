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
 * Copyright (C) 2016 Aleksander Morgado
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

#include "mm-iface-modem.h"
#include "mm-sim.h"
#include "mm-base-modem-at.h"
#include "mm-base-modem.h"
#include "mm-log.h"
#include "mm-modem-helpers.h"

G_DEFINE_TYPE (MMSim, mm_sim, MM_TYPE_BASE_SIM)

/*****************************************************************************/
/* CHANGE PIN (Generic implementation) */

static gboolean
change_pin_finish (MMBaseSim *self,
                   GAsyncResult *res,
                   GError **error)
{
    return !g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error);
}

static void
change_pin_ready (MMBaseModem *modem,
                  GAsyncResult *res,
                  GSimpleAsyncResult *simple)
{
    GError *error = NULL;

    mm_base_modem_at_command_finish (modem, res, &error);
    if (error)
        g_simple_async_result_take_error (simple, error);
    else
        g_simple_async_result_set_op_res_gboolean (simple, TRUE);
    g_simple_async_result_complete (simple);
    g_object_unref (simple);
}

static void
change_pin (MMBaseSim *self,
            const gchar *old_pin,
            const gchar *new_pin,
            GAsyncReadyCallback callback,
            gpointer user_data)
{
    GSimpleAsyncResult *result;
    gchar *command;

    result = g_simple_async_result_new (G_OBJECT (self),
                                        callback,
                                        user_data,
                                        change_pin);

    command = g_strdup_printf ("+CPWD=\"SC\",\"%s\",\"%s\"",
                               old_pin,
                               new_pin);

    mm_base_modem_at_command (mm_base_sim_peek_modem (self),
                              command,
                              3,
                              FALSE,
                              (GAsyncReadyCallback)change_pin_ready,
                              result);

    g_free (command);
}

/*****************************************************************************/
/* ENABLE PIN (Generic implementation) */

static gboolean
enable_pin_finish (MMBaseSim *self,
                   GAsyncResult *res,
                   GError **error)
{
    return !g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error);
}

static void
enable_pin_ready (MMBaseModem *modem,
                  GAsyncResult *res,
                  GSimpleAsyncResult *simple)
{
    GError *error = NULL;

    mm_base_modem_at_command_finish (modem, res, &error);
    if (error)
        g_simple_async_result_take_error (simple, error);
    else
        g_simple_async_result_set_op_res_gboolean (simple, TRUE);
    g_simple_async_result_complete (simple);
    g_object_unref (simple);
}

static void
enable_pin (MMBaseSim *self,
            const gchar *pin,
            gboolean enabled,
            GAsyncReadyCallback callback,
            gpointer user_data)
{
    GSimpleAsyncResult *result;
    gchar *command;

    result = g_simple_async_result_new (G_OBJECT (self),
                                        callback,
                                        user_data,
                                        enable_pin);

    command = g_strdup_printf ("+CLCK=\"SC\",%d,\"%s\"",
                               enabled ? 1 : 0,
                               pin);
    mm_base_modem_at_command (mm_base_sim_peek_modem (self),
                              command,
                              3,
                              FALSE,
                              (GAsyncReadyCallback)enable_pin_ready,
                              result);

    g_free (command);
}

/*****************************************************************************/
/* SEND PIN/PUK (Generic implementation) */

static gboolean
common_send_pin_puk_finish (MMBaseSim *self,
                            GAsyncResult *res,
                            GError **error)
{
    return !g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error);
}

static void
send_pin_puk_ready (MMBaseModem *modem,
                    GAsyncResult *res,
                    GSimpleAsyncResult *simple)
{
    GError *error = NULL;

    mm_base_modem_at_command_finish (modem, res, &error);
    if (error)
        g_simple_async_result_take_error (simple, error);
    else
        g_simple_async_result_set_op_res_gboolean (simple, TRUE);
    g_simple_async_result_complete (simple);
    g_object_unref (simple);
}

static void
common_send_pin_puk (MMBaseSim *self,
                     const gchar *pin,
                     const gchar *puk,
                     GAsyncReadyCallback callback,
                     gpointer user_data)
{
    GSimpleAsyncResult *result;
    gchar *command;

    result = g_simple_async_result_new (G_OBJECT (self),
                                        callback,
                                        user_data,
                                        common_send_pin_puk);

    command = (puk ?
               g_strdup_printf ("+CPIN=\"%s\",\"%s\"", puk, pin) :
               g_strdup_printf ("+CPIN=\"%s\"", pin));

    mm_base_modem_at_command (mm_base_sim_peek_modem (self),
                              command,
                              3,
                              FALSE,
                              (GAsyncReadyCallback)send_pin_puk_ready,
                              result);

    g_free (command);
}

static void
send_puk (MMBaseSim *self,
          const gchar *puk,
          const gchar *new_pin,
          GAsyncReadyCallback callback,
          gpointer user_data)
{
    common_send_pin_puk (self, new_pin, puk, callback, user_data);
}

static void
send_pin (MMBaseSim *self,
          const gchar *pin,
          GAsyncReadyCallback callback,
          gpointer user_data)
{
    common_send_pin_puk (self, pin, NULL, callback, user_data);
}

/*****************************************************************************/

#undef STR_REPLY_READY_FN
#define STR_REPLY_READY_FN(NAME)                                        \
    static void                                                         \
    NAME##_command_ready (MMBaseModem *modem,                           \
                          GAsyncResult *res,                            \
                          GSimpleAsyncResult *operation_result)         \
    {                                                                   \
        GError *error = NULL;                                           \
        const gchar *response;                                          \
                                                                        \
        response = mm_base_modem_at_command_finish (modem, res, &error); \
        if (error)                                                      \
            g_simple_async_result_take_error (operation_result, error); \
        else                                                            \
            g_simple_async_result_set_op_res_gpointer (operation_result, \
                                                       (gpointer)response, \
                                                       NULL);           \
                                                                        \
        g_simple_async_result_complete (operation_result);              \
        g_object_unref (operation_result);                              \
    }

/*****************************************************************************/
/* SIM IDENTIFIER */

static gchar *
parse_iccid (const gchar *response,
             GError **error)
{
    gchar buf[21];
    const gchar *str;
    gint sw1;
    gint sw2;
    gboolean success = FALSE;

    memset (buf, 0, sizeof (buf));
    str = mm_strip_tag (response, "+CRSM:");
    if (sscanf (str, "%d,%d,\"%20c\"", &sw1, &sw2, (char *) &buf) == 3)
        success = TRUE;
    else {
        /* May not include quotes... */
        if (sscanf (str, "%d,%d,%20c", &sw1, &sw2, (char *) &buf) == 3)
            success = TRUE;
    }

    if (!success) {
        g_set_error (error,
                     MM_CORE_ERROR,
                     MM_CORE_ERROR_FAILED,
                     "Could not parse the CRSM response");
        return NULL;
    }

    if ((sw1 == 0x90 && sw2 == 0x00) ||
        (sw1 == 0x91) ||
        (sw1 == 0x92) ||
        (sw1 == 0x9f)) {
        return mm_3gpp_parse_iccid (buf, error);
    } else {
        g_set_error (error,
                     MM_CORE_ERROR,
                     MM_CORE_ERROR_FAILED,
                     "SIM failed to handle CRSM request (sw1 %d sw2 %d)",
                     sw1, sw2);
        return NULL;
    }
}

static gchar *
load_sim_identifier_finish (MMBaseSim *self,
                            GAsyncResult *res,
                            GError **error)
{
    const gchar *result;
    gchar *sim_identifier;

    if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
        return NULL;
    result = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (res));

    sim_identifier = parse_iccid (result, error);
    if (!sim_identifier)
        return NULL;

    mm_dbg ("loaded SIM identifier: %s", sim_identifier);
    return sim_identifier;
}

STR_REPLY_READY_FN (load_sim_identifier)

static void
load_sim_identifier (MMBaseSim *self,
                     GAsyncReadyCallback callback,
                     gpointer user_data)
{
    mm_dbg ("loading SIM identifier...");

    /* READ BINARY of EFiccid (ICC Identification) ETSI TS 102.221 section 13.2 */
    mm_base_modem_at_command (
        mm_base_sim_peek_modem (self),
        "+CRSM=176,12258,0,0,10",
        20,
        FALSE,
        (GAsyncReadyCallback)load_sim_identifier_command_ready,
        g_simple_async_result_new (G_OBJECT (self),
                                   callback,
                                   user_data,
                                   load_sim_identifier));
}

/*****************************************************************************/
/* IMSI */

static gchar *
parse_imsi (const gchar *response,
            GError **error)
{
    const gchar *s;
    gint len;

    g_assert (response != NULL);

    for (s = mm_strip_tag (response, "+CIMI"), len = 0;
         *s;
         ++s, ++len) {
        /* IMSI is a number with 15 or less decimal digits. */
        if (!isdigit (*s) || len > 15) {
            g_set_error (error,
                         MM_CORE_ERROR,
                         MM_CORE_ERROR_FAILED,
                         "Invalid +CIMI response '%s'", response ? response : "<null>");
            return NULL;
        }
    }

    return g_strdup (response);
}

static gchar *
load_imsi_finish (MMBaseSim *self,
                  GAsyncResult *res,
                  GError **error)
{
    const gchar *result;
    gchar *imsi;

    if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
        return NULL;
    result = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (res));

    imsi = parse_imsi (result, error);
    if (!imsi)
        return NULL;

    mm_dbg ("loaded IMSI: %s", imsi);
    return imsi;
}

STR_REPLY_READY_FN (load_imsi)

static void
load_imsi (MMBaseSim *self,
           GAsyncReadyCallback callback,
           gpointer user_data)
{
    mm_dbg ("loading IMSI...");

    mm_base_modem_at_command (
        mm_base_sim_peek_modem (self),
        "+CIMI",
        3,
        FALSE,
        (GAsyncReadyCallback)load_imsi_command_ready,
        g_simple_async_result_new (G_OBJECT (self),
                                   callback,
                                   user_data,
                                   load_imsi));
}

/*****************************************************************************/
/* Operator ID */

static guint
parse_mnc_length (const gchar *response,
                  GError **error)
{
    gint sw1;
    gint sw2;
    gboolean success = FALSE;
    gchar hex[51];

    memset (hex, 0, sizeof (hex));
    if (sscanf (response, "+CRSM:%d,%d,\"%50c\"", &sw1, &sw2, (char *) &hex) == 3)
        success = TRUE;
    else {
        /* May not include quotes... */
        if (sscanf (response, "+CRSM:%d,%d,%50c", &sw1, &sw2, (char *) &hex) == 3)
            success = TRUE;
    }

    if (!success) {
        g_set_error (error,
                     MM_CORE_ERROR,
                     MM_CORE_ERROR_FAILED,
                     "Could not parse the CRSM response");
        return 0;
    }

    if ((sw1 == 0x90 && sw2 == 0x00) ||
        (sw1 == 0x91) ||
        (sw1 == 0x92) ||
        (sw1 == 0x9f)) {
        gsize buflen = 0;
        guint32 mnc_len;
        gchar *bin;

        /* Make sure the buffer is only hex characters */
        while (buflen < sizeof (hex) && hex[buflen]) {
            if (!isxdigit (hex[buflen])) {
                hex[buflen] = 0x0;
                break;
            }
            buflen++;
        }

        /* Convert hex string to binary */
        bin = mm_utils_hexstr2bin (hex, &buflen);
        if (!bin || buflen < 4) {
            g_set_error (error,
                         MM_CORE_ERROR,
                         MM_CORE_ERROR_FAILED,
                         "SIM returned malformed response '%s'",
                         hex);
            g_free (bin);
            return 0;
        }

        /* MNC length is byte 4 of this SIM file */
        mnc_len = bin[3] & 0xFF;
        if (mnc_len == 2 || mnc_len == 3) {
            g_free (bin);
            return mnc_len;
        }

        g_set_error (error,
                     MM_CORE_ERROR,
                     MM_CORE_ERROR_FAILED,
                     "SIM returned invalid MNC length %d (should be either 2 or 3)",
                     mnc_len);
        g_free (bin);
        return 0;
    }

    g_set_error (error,
                 MM_CORE_ERROR,
                 MM_CORE_ERROR_FAILED,
                 "SIM failed to handle CRSM request (sw1 %d sw2 %d)",
                 sw1, sw2);
    return 0;
}

static gchar *
load_operator_identifier_finish (MMBaseSim *self,
                                 GAsyncResult *res,
                                 GError **error)
{
    GError *inner_error = NULL;
    const gchar *imsi;
    const gchar *result;
    guint mnc_length;

    if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
        return NULL;
    result = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (res));

    imsi = mm_gdbus_sim_get_imsi (MM_GDBUS_SIM (self));
    if (!imsi) {
        g_set_error (error,
                     MM_CORE_ERROR,
                     MM_CORE_ERROR_FAILED,
                     "Cannot load Operator ID without IMSI");
        return NULL;
    }

    mnc_length = parse_mnc_length (result, &inner_error);
    if (inner_error) {
        g_propagate_error (error, inner_error);
        return NULL;
    }

    /* Build Operator ID */
    return g_strndup (imsi, 3 + mnc_length);
}

STR_REPLY_READY_FN (load_operator_identifier)

static void
load_operator_identifier (MMBaseSim *self,
                          GAsyncReadyCallback callback,
                          gpointer user_data)
{
    mm_dbg ("loading Operator ID...");

    /* READ BINARY of EFad (Administrative Data) ETSI 51.011 section 10.3.18 */
    mm_base_modem_at_command (
        mm_base_sim_peek_modem (self),
        "+CRSM=176,28589,0,0,4",
        10,
        FALSE,
        (GAsyncReadyCallback)load_operator_identifier_command_ready,
        g_simple_async_result_new (G_OBJECT (self),
                                   callback,
                                   user_data,
                                   load_operator_identifier));
}

/*****************************************************************************/
/* Operator Name (Service Provider Name) */

static gchar *
parse_spn (const gchar *response,
           GError **error)
{
    gint sw1;
    gint sw2;
    gboolean success = FALSE;
    gchar hex[51];

    memset (hex, 0, sizeof (hex));
    if (sscanf (response, "+CRSM:%d,%d,\"%50c\"", &sw1, &sw2, (char *) &hex) == 3)
        success = TRUE;
    else {
        /* May not include quotes... */
        if (sscanf (response, "+CRSM:%d,%d,%50c", &sw1, &sw2, (char *) &hex) == 3)
            success = TRUE;
    }

    if (!success) {
        g_set_error (error,
                     MM_CORE_ERROR,
                     MM_CORE_ERROR_FAILED,
                     "Could not parse the CRSM response");
        return NULL;
    }

    if ((sw1 == 0x90 && sw2 == 0x00) ||
        (sw1 == 0x91) ||
        (sw1 == 0x92) ||
        (sw1 == 0x9f)) {
        gsize buflen = 0;
        gchar *bin;
        gchar *utf8;

        /* Make sure the buffer is only hex characters */
        while (buflen < sizeof (hex) && hex[buflen]) {
            if (!isxdigit (hex[buflen])) {
                hex[buflen] = 0x0;
                break;
            }
            buflen++;
        }

        /* Convert hex string to binary */
        bin = mm_utils_hexstr2bin (hex, &buflen);
        if (!bin) {
            g_set_error (error,
                         MM_CORE_ERROR,
                         MM_CORE_ERROR_FAILED,
                         "SIM returned malformed response '%s'",
                         hex);
            return NULL;
        }

        /* Remove the FF filler at the end */
        while (buflen > 1 && bin[buflen - 1] == (char)0xff)
            buflen--;

        /* First byte is metadata; remainder is GSM-7 unpacked into octets; convert to UTF8 */
        utf8 = (gchar *)mm_charset_gsm_unpacked_to_utf8 ((guint8 *)bin + 1, buflen - 1);
        g_free (bin);
        return utf8;
    }

    g_set_error (error,
                 MM_CORE_ERROR,
                 MM_CORE_ERROR_FAILED,
                 "SIM failed to handle CRSM request (sw1 %d sw2 %d)",
                 sw1, sw2);
    return NULL;
}

static gchar *
load_operator_name_finish (MMBaseSim *self,
                           GAsyncResult *res,
                           GError **error)
{
    const gchar *result;

    if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
        return NULL;
    result = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (res));

    return parse_spn (result, error);
}

STR_REPLY_READY_FN (load_operator_name)

static void
load_operator_name (MMBaseSim *self,
                    GAsyncReadyCallback callback,
                    gpointer user_data)
{
    mm_dbg ("loading Operator Name...");

    /* READ BINARY of EFspn (Service Provider Name) ETSI 51.011 section 10.3.11 */
    mm_base_modem_at_command (
        mm_base_sim_peek_modem (self),
        "+CRSM=176,28486,0,0,17",
        10,
        FALSE,
        (GAsyncReadyCallback)load_operator_name_command_ready,
        g_simple_async_result_new (G_OBJECT (self),
                                   callback,
                                   user_data,
                                   load_operator_name));
}

/*****************************************************************************/

MMBaseSim *
mm_sim_new_finish (GAsyncResult  *res,
                   GError       **error)
{
    GObject *source;
    GObject *sim;

    source = g_async_result_get_source_object (res);
    sim = g_async_initable_new_finish (G_ASYNC_INITABLE (source), res, error);
    g_object_unref (source);

    if (!sim)
        return NULL;

    /* Only export valid SIMs */
    mm_base_sim_export (MM_BASE_SIM (sim));

    return MM_BASE_SIM (sim);
}

void
mm_sim_new (MMBaseModem *modem,
            GCancellable *cancellable,
            GAsyncReadyCallback callback,
            gpointer user_data)
{
    g_async_initable_new_async (MM_TYPE_SIM,
                                G_PRIORITY_DEFAULT,
                                cancellable,
                                callback,
                                user_data,
                                MM_BASE_SIM_MODEM, modem,
                                NULL);
}

static void
mm_sim_init (MMSim *self)
{
}

static void
mm_sim_class_init (MMSimClass *klass)
{
    MMBaseSimClass *base_sim_class = MM_BASE_SIM_CLASS (klass);

    base_sim_class->load_sim_identifier = load_sim_identifier;
    base_sim_class->load_sim_identifier_finish = load_sim_identifier_finish;
    base_sim_class->load_imsi = load_imsi;
    base_sim_class->load_imsi_finish = load_imsi_finish;
    base_sim_class->load_operator_identifier = load_operator_identifier;
    base_sim_class->load_operator_identifier_finish = load_operator_identifier_finish;
    base_sim_class->load_operator_name = load_operator_name;
    base_sim_class->load_operator_name_finish = load_operator_name_finish;
    base_sim_class->send_pin = send_pin;
    base_sim_class->send_pin_finish = common_send_pin_puk_finish;
    base_sim_class->send_puk = send_puk;
    base_sim_class->send_puk_finish = common_send_pin_puk_finish;
    base_sim_class->enable_pin = enable_pin;
    base_sim_class->enable_pin_finish = enable_pin_finish;
    base_sim_class->change_pin = change_pin;
    base_sim_class->change_pin_finish = change_pin_finish;
}

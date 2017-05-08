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
 * Copyright (C) 2017 Red Hat, Inc.
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mm-ppp.h"
#include "mm-log.h"

G_DEFINE_TYPE (MMPpp, mm_ppp, G_TYPE_OBJECT)

enum {
    PROP_0,
    PROP_AUTH,
    PROP_USERNAME,
    PROP_PASSWORD,
    PROP_LAST
};

static GParamSpec *properties[PROP_LAST];

typedef enum {
    MM_PPP_PHASE_DEAD         = 0,
    MM_PPP_PHASE_ESTABLISH    = 1 << 0,
    MM_PPP_PHASE_AUTHENTICATE = 1 << 1,
    MM_PPP_PHASE_NETWORK      = 1 << 2,
    MM_PPP_PHASE_TERMINATE    = 1 << 3,
} MMPppPhase;

struct _MMPppPrivate {
    MMPppPhase phase;
    MMLcp     *lcp;

    MMPppAuth  auth;
    gchar     *username;
    gchar     *password;

    guint      mtu;
    guint      mru;
};

/*****************************************************************************/

GQuark
mm_ppp_error_quark (void)
{
    static GQuark quark = 0;

    return (G_LIKELY (quark != 0) ? quark : (quark = g_quark_from_static_string ("mm-ppp-error-quark")));
}

/*****************************************************************************/

/* Retrieves a guint16 in host byte order, handling
 * alignment issues.
 */
static inline guint16
get_host_u16 (const guint8 *bytes)
{
  union {
    guchar  bytes[2];
    guint16 integer;
  } tmpvalue = { 0 };

  memcpy (&tmpvalue.bytes, bytes, 2);
  return GUINT16_FROM_BE (tmpvalue.integer);
}

/*****************************************************************************/

https://www.rfc-editor.org/rfc/rfc1548.txt

MMPpp *
mm_ppp_new (MMPppAuth    auth,
            const gchar *username,
            const gchar *password)
{
    return g_object_new (MM_TYPE_PPP,
                         "auth", auth,
                         "username", username,
                         "password", password,
                         NULL);
}

#define PPP_PROTO_IP     0x0021
#define PPP_PROTO_IPCP   0x8021
#define PPP_PROTO_IPV6CP 0x8057
#define PPP_PROTO_LCP    0xC021
#define PPP_PROTO_PAP    0xC023
#define PPP_PROTO_CHAP   0xC223

static gboolean
protocol_allowed (guint protocol,
                  MMPppPhase phase,
                  GError **error)
{
    switch (phase) {
    case MM_PPP_PHASE_DEAD:
    case MM_PPP_PHASE_ESTABLISH:
    case MM_PPP_PHASE_TERMINATE:
        if (protocol == PPP_PROTO_LCP)
            return TRUE;
        break;
    case MM_PPP_PHASE_AUTHENTICATE:
        if (protocol == PPP_PROTO_LCP ||
            protocol == PPP_PROTO_PAP ||
            protocol == PPP_PROTO_CHAP)
            return TRUE;
        break;
    case MM_PPP_PHASE_NETWORK:
        if (protocol == PPP_PROTO_LCP ||
            protocol == PPP_PROTO_IPCP ||
            protocol == PPP_PROTO_IPV6CP ||
            protocol == PPP_PROTO_IP)
            return TRUE;
        break;
    }

    g_set_error (error,
                 MM_PPP_ERROR,
                 MM_PPP_ERROR_UNHANDLED_FRAME,
                 "unhandled or disallowed protocol %d frame (phase %d)",
                 protocol,
                 phase);
    return FALSE;
}

gboolean
mm_ppp_process (MMPpp *self,
                GByteArray *data,
                GError **error)
{
    const guint8 *buf = data->data;
    guint8 len = data->len;
    guint16 protocol = 0;

    /* Need at least the Protocol field */
    if (data->len == 0) {
        g_set_error (error,
                     MM_PPP_ERROR,
                     MM_PPP_ERROR_MALFORMED_FRAME,
                     "zero-length PPP frame");
        return FALSE;
    }

    /* Check for Address/Control fields first.  If LCP compression has been
     * negotiated, these fields will be absent.
     */
    if (buf[0] == 0xFF) {
        if (data->len < 3 || buf[1] != 0x03) {
            g_set_error (error,
                         MM_PPP_ERROR,
                         MM_PPP_ERROR_MALFORMED_FRAME,
                         "short uncompressed frame or missing Control field valid");
            return FALSE;
        }
        /* Address/Control fields are valid and first */
        buf += 2;
        len -= 2;
    }

    /* ProtocolID: normally 16 bits, unless Protocol Field Compression has been
     * negotiated, then 8 bits.  The least significant bit of the least
     * significant byte is always 1, which allows us to figure out the field
     * size without knowing whether Protocol Field Compression is enabled.
     */
    if (buf[0] & 0x1) {
        /* single byte */
        protocol = (guint16) buf[0];
        buf++;
        len--;
    } else if (len < 2) {
        g_set_error (error,
                     MM_PPP_ERROR,
                     MM_PPP_ERROR_MALFORMED_FRAME,
                     "expected two-byte protocol field but only got one");
        return FALSE;
    } else {
        /* two bytes */
        protocol = get_host_u16 (buf);
        buf += 2;
        len -= 2;
    }

    if (!protocol_allowed (protocol, self->priv->phase, error))
        return FALSE;

    switch (protocol) {
    case PPP_PROTO_IP:
        break;
    case PPP_PROTO_IPCP:
        break;
    case PPP_PROTO_IPV6CP:
        break;
    case PPP_PROTO_LCP:
        if (!self->priv->lcp)
            self->priv->lcp = mm_lcp_new ();
        mm_lcp_process (self->priv->lcp, buf, len);
        break;
    case PPP_PROTO_PAP:
        break;
    case PPP_PROTO_CHAP:
        break;
    default:
        g_set_error (error,
                     MM_PPP_ERROR,
                     MM_PPP_ERROR_UNHANDLED_FRAME,
                     "unknown protocol %d",
                     protocol);
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************************/

static void
mm_ppp_init (MMPpp *self)
{
    /* Initialize private data */
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, MM_TYPE_PPP, MMPppPrivate);
    self->priv->auth = MM_PPP_AUTH_NONE;
    self->priv->mtu = 1500;
    self->priv->mru = 1500;
    self->priv->phase = MM_PPP_PHASE_DEAD;
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    MMPpp *self = MM_PPP (object);

    switch (prop_id) {
    case PROP_AUTH:
        self->priv->auth = g_value_get_uint (value);
        break;
    case PROP_USERNAME:
        g_assert (!self->priv->username);
        self->priv->username = g_value_dup_string (value);
        break;
    case PROP_PASSWORD:
        g_assert (!self->priv->password);
        self->priv->password = g_value_dup_string (value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    MMPpp *self = MM_PPP (object);

    switch (prop_id) {
    case PROP_AUTH:
        g_value_set_uint (value, self->priv->auth);
        break;
    case PROP_USERNAME:
        g_value_set_string (value, self->priv->username);
        break;
    case PROP_PASSWORD:
        g_value_set_string (value, self->priv->password);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
dispose (GObject *object)
{
    MMPpp *self = MM_PPP (object);

    g_clear_pointer (&self->priv->lcp, mm_lcp_free);

    g_clear_pointer (&self->priv->username, g_free);
    g_clear_pointer (&self->priv->password, g_free);

    G_OBJECT_CLASS (mm_ppp_parent_class)->dispose (object);
}

static void
mm_ppp_class_init (MMPppClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (object_class, sizeof (MMPppPrivate));

    object_class->dispose      = dispose;
    object_class->get_property = get_property;
    object_class->set_property = set_property;

    properties[PROP_AUTH] =
        g_param_spec_uint ("auth",
                           "Authentication",
                           "PPP authentication protocol",
                           MM_PPP_AUTH_NONE, MM_PPP_AUTH_PAP, MM_PPP_AUTH_CHAP,
                           G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_AUTH, properties[PROP_AUTH]);

    properties[PROP_USERNAME] =
        g_param_spec_string ("username",
                             "Username",
                             "PPP username",
                             NULL,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_USERNAME, properties[PROP_USERNAME]);

    properties[PROP_PASSWORD] =
        g_param_spec_string ("password",
                             "Password",
                             "PPP password",
                             NULL,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_PASSWORD, properties[PROP_PASSWORD]);
}

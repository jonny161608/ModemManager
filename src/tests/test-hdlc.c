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
 * Copyright (C) 2016 Red Hat, Inc.
 */

#include <config.h>
#include <string.h>
#include <glib.h>

#include "mm-hdlc.h"

static void
test_hdlc_decapsulate ()
{
    GByteArray *framed, *unframed;
    guint bytes_used = 0;
    gboolean need_more = FALSE;
    GError *error = NULL;

    static const guint8 hdlc[] = {
        0x40, 0x03, 0x00, 0x01, 0x00, 0x19, 0xf0, 0x00, 0x16, 0x00, 0x21, 0x00,
        0x1c, 0x00, 0xd8, 0x00, 0x3f, 0x00, 0x56, 0x01, 0x3f, 0x00, 0x15, 0x00,
        0x1a, 0x00, 0x11, 0x01, 0x3f, 0x00, 0x92, 0x01, 0x3f, 0x00, 0x39, 0x00,
        0x3f, 0x00, 0x95, 0x01, 0x3f, 0x00, 0x12, 0x00, 0x3f, 0x00, 0x23, 0x01,
        0x3f, 0x00, 0x66, 0x00, 0x3f, 0x00, 0x0b, 0x01, 0x3f, 0x00, 0xae, 0x00,
        0x3f, 0x00, 0x02, 0x01, 0x3f, 0x00, 0xa8, 0x00, 0x3f, 0x00, 0x50, 0x01,
        0x3f, 0x00, 0xf8, 0x01, 0x3f, 0x00, 0x57, 0x00, 0x3f, 0x00, 0x7d, 0x5e,
        0x00, 0x3f, 0x00, 0x93, 0x00, 0x3f, 0x00, 0xbd, 0x00, 0x3f, 0x00, 0x77,
        0x01, 0x3f, 0x00, 0xb7, 0x00, 0x3f, 0x00, 0xab, 0x00, 0x3f, 0x00, 0x33,
        0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xad, 0xde, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13,
        0x13, 0x50, 0x1f, 0x00, 0x00, 0xff, 0xff, 0x00, 0xaa, 0x19, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb1, 0xc4, 0x7d, 0x5e,
        0x7d, 0x5e, 0x7d, 0x5d, 0x5d, 0x04, 0x58, 0x1b, 0x5b, 0x1b, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x65, 0x69, 0x7e
    };
    static const guint8 raw[] = {
        0x40, 0x03, 0x00, 0x01, 0x00, 0x19, 0xF0, 0x00, 0x16, 0x00, 0x21, 0x00,
        0x1C, 0x00, 0xD8, 0x00, 0x3F, 0x00, 0x56, 0x01, 0x3F, 0x00, 0x15, 0x00,
        0x1A, 0x00, 0x11, 0x01, 0x3F, 0x00, 0x92, 0x01, 0x3F, 0x00, 0x39, 0x00,
        0x3F, 0x00, 0x95, 0x01, 0x3F, 0x00, 0x12, 0x00, 0x3F, 0x00, 0x23, 0x01,
        0x3F, 0x00, 0x66, 0x00, 0x3F, 0x00, 0x0B, 0x01, 0x3F, 0x00, 0xAE, 0x00,
        0x3F, 0x00, 0x02, 0x01, 0x3F, 0x00, 0xA8, 0x00, 0x3F, 0x00, 0x50, 0x01,
        0x3F, 0x00, 0xF8, 0x01, 0x3F, 0x00, 0x57, 0x00, 0x3F, 0x00, 0x7E, 0x00,
        0x3F, 0x00, 0x93, 0x00, 0x3F, 0x00, 0xBD, 0x00, 0x3F, 0x00, 0x77, 0x01,
        0x3F, 0x00, 0xB7, 0x00, 0x3F, 0x00, 0xAB, 0x00, 0x3F, 0x00, 0x33, 0x00,
        0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xAD, 0xDE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x13,
        0x50, 0x1F, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xAA, 0x19, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB1, 0xC4, 0x7E, 0x7E, 0x7D,
        0x5D, 0x04, 0x58, 0x1B, 0x5B, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    framed = g_byte_array_sized_new (sizeof (hdlc));
    g_byte_array_append (framed, hdlc, sizeof (hdlc));

    unframed = mm_hdlc_decapsulate (framed, &bytes_used, &need_more, &error);
    g_assert_no_error (error);
    g_assert (unframed);
    g_assert_cmpint (bytes_used, ==, 221);
    g_assert_cmpint (unframed->len, ==, sizeof (raw));
    g_assert_cmpint (memcmp (unframed->data, raw, sizeof (raw)), ==, 0);
    g_assert (need_more == FALSE);

    g_byte_array_free (framed, TRUE);
    g_byte_array_free (unframed, TRUE);
}

static void
test_hdlc_encapsulate ()
{
    GByteArray *unframed, *framed;
    GError *error = NULL;

    static const guint8 raw[] = {
        0x4B, 0x05, 0x08, 0x00
    };
    static const guint8 encapsulated[] = {
        0x4b, 0x05, 0x08, 0x00, 0x01, 0xdd, 0x7e
    };

    unframed = g_byte_array_sized_new (sizeof (raw));
    g_byte_array_append (unframed, raw, sizeof (raw));

    framed = mm_hdlc_encapsulate (unframed, &error);
    g_assert (framed);
    g_assert_cmpint (framed->len, ==, sizeof (encapsulated));
    g_assert_cmpint (memcmp (framed->data, encapsulated, sizeof (encapsulated)), ==, 0);

    g_byte_array_free (framed, TRUE);
    g_byte_array_free (unframed, TRUE);
}

static void
test_hdlc_bad_frame ()
{
    GByteArray *framed, *unframed;
    guint bytes_used = 0;
    gboolean need_more = FALSE;
    GError *error = NULL;

    static const guint8 buf[] = {
        0x00, 0x0a, 0x6b, 0x74, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x7e
    };

    framed = g_byte_array_sized_new (sizeof (buf));
    g_byte_array_append (framed, buf, sizeof (buf));

    unframed = mm_hdlc_decapsulate (framed, &bytes_used, &need_more, &error);
    g_assert (unframed == NULL);
    g_assert_cmpint (bytes_used, ==, 15);
    g_assert (need_more == FALSE);
    g_assert_error (error, G_IO_ERROR, G_IO_ERROR_FAILED);

    g_byte_array_free (framed, TRUE);
}

int main (int argc, char **argv)
{
    g_type_init ();
    g_test_init (&argc, &argv, NULL);

    g_test_add_func ("/ModemManager/HDLC/decapsulate", test_hdlc_decapsulate);
    g_test_add_func ("/ModemManager/HDLC/encapsulate", test_hdlc_encapsulate);
    g_test_add_func ("/ModemManager/HDLC/bad-frame",   test_hdlc_bad_frame);

    return g_test_run ();
}

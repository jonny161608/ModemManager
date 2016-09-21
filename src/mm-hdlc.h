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

#ifndef MM_HDLC_H
#define MM_HDLC_H

#include <glib.h>
#include <gio/gio.h>

GByteArray *mm_hdlc_encapsulate (GByteArray *bytes, GError **error);

GByteArray *mm_hdlc_decapsulate (GByteArray *bytes,
                                 guint *out_bytes_used,
                                 gboolean *out_need_more,
                                 GError **error);

#endif /* MM_HDLC_H */

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

#ifndef MM_LCP_H
#define MM_LCP_H

#include <glib.h>

typedef struct MMLcp MMLcp;

MMLcp *mm_lcp_new (void);

gboolean mm_lcp_process (MMLcp        *lcp,
                         const gchar  *data,
                         gsize         len,
                         GError      **error);

void mm_lcp_free (MMLcp *lcp);

#endif /* MM_LCP_H */

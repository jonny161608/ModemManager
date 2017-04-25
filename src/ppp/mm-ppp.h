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

#ifndef MM_PPP_H
#define MM_PPP_H

#include <glib.h>
#include <gio/gio.h>
#include <glib-object.h>

#define MM_TYPE_PPP            (mm_ppp_get_type ())
#define MM_PPP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MM_TYPE_PPP, MMPpp))
#define MM_PPP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  MM_TYPE_PPP, MMPppClass))
#define MM_IS_PPP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MM_TYPE_PPP))
#define MM_IS_PPP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  MM_TYPE_PPP))
#define MM_PPP_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  MM_TYPE_PPP, MMPppClass))

typedef struct _MMPpp MMPpp;
typedef struct _MMPppClass MMPppClass;
typedef struct _MMPppPrivate MMPppPrivate;

struct _MMPpp {
    GObject parent;
    MMPppPrivate *priv;
};

struct _MMPppClass {
    GObjectClass parent;
};

typedef enum {
    MM_PPP_AUTH_NONE,
    MM_PPP_AUTH_CHAP,
    MM_PPP_AUTH_PAP,
} MMPppAuth;

/**
 * MMPppError:
 * @MM_PPP_ERROR_UNKNOWN: unknown or unclassified error
 * @MM_PPP_ERROR_MALFORMED_FRAME: the PPP frame is malformed
 */
typedef enum {
	MM_PPP_ERROR_UNKNOWN = 0,
	MM_PPP_ERROR_MALFORMED_FRAME,
	MM_PPP_ERROR_UNHANDLED_FRAME,
} MMPppError;

#define MM_PPP_ERROR (mm_ppp_error_quark ())
GQuark mm_ppp_error_quark (void);

GType  mm_ppp_get_type (void);
MMPpp *mm_ppp_new (MMPppAuth    auth,
                   const gchar *username,
                   const gchar *password);

gboolean mm_ppp_process (MMPpp *ppp,
                         GByteArray *data,
                         GError **error);

#endif /* MM_PPP_H */

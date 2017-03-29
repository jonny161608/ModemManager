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
 * Copyright (C) 2012 Google, Inc.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <ModemManager.h>
#define _LIBMM_INSIDE_MM
#include <libmm-glib.h>

#include "mm-device.h"
#include "mm-plugin.h"
#include "mm-log.h"

G_DEFINE_TYPE (MMDevice, mm_device, G_TYPE_OBJECT);

enum {
    PROP_0,
    PROP_UID,
    PROP_PLUGIN,
    PROP_MODEM,
    PROP_HOTPLUGGED,
    PROP_VIRTUAL,
    PROP_LAST
};

enum {
    SIGNAL_PORT_GRABBED,
    SIGNAL_PORT_RELEASED,
    SIGNAL_LAST
};

static GParamSpec *properties[PROP_LAST];
static guint signals[SIGNAL_LAST];

struct _MMDevicePrivate {
    /* Whether the device is real or virtual */
    gboolean virtual;

    /* Unique id */
    gchar *uid;

    /* If USB, device vid/pid */
    guint16 vendor;
    guint16 product;

    /* Kernel drivers managing this device */
    gchar **drivers;

    /* Best plugin to manage this device */
    MMPlugin *plugin;

    /* Lists of port probes in the device */
    GList *port_probes;
    GList *ignored_port_probes;

    /* The Modem object for this device */
    MMBaseModem *modem;
    gulong       modem_valid_id;

    /* When exported, a reference to the object manager */
    GDBusObjectManagerServer *object_manager;

    /* Whether the device was hot-plugged. */
    gboolean hotplugged;

    /* Virtual ports */
    gchar **virtual_ports;
};

/*****************************************************************************/

static MMPortProbe *
device_find_probe_with_device (MMDevice       *self,
                               MMKernelDevice *kernel_port,
                               gboolean        lookup_ignored)
{
    GList *l;

    for (l = self->priv->port_probes; l; l = g_list_next (l)) {
        MMPortProbe *probe = MM_PORT_PROBE (l->data);

        if (mm_kernel_device_cmp (mm_port_probe_peek_port (probe), kernel_port))
            return probe;
    }

    if (!lookup_ignored)
        return NULL;

    for (l = self->priv->ignored_port_probes; l; l = g_list_next (l)) {
        MMPortProbe *probe = MM_PORT_PROBE (l->data);

        if (mm_kernel_device_cmp (mm_port_probe_peek_port (probe), kernel_port))
            return probe;
    }

    return NULL;
}

gboolean
mm_device_owns_port (MMDevice       *self,
                     MMKernelDevice *kernel_port)
{
    return !!device_find_probe_with_device (self, kernel_port, TRUE);
}

static void
add_port_driver (MMDevice       *self,
                 MMKernelDevice *kernel_port)
{
    const gchar *driver;
    guint n_items;
    guint i;

    driver = mm_kernel_device_get_driver (kernel_port);
    if (!driver)
        return;

    n_items = (self->priv->drivers ? g_strv_length (self->priv->drivers) : 0);
    if (n_items > 0) {
        /* Add driver to our list of drivers, if not already there */
        for (i = 0; self->priv->drivers[i]; i++) {
            if (g_str_equal (self->priv->drivers[i], driver)) {
                driver = NULL;
                break;
            }
        }
    }

    if (!driver)
        return;

    self->priv->drivers = g_realloc (self->priv->drivers, (n_items + 2) * sizeof (gchar *));
    self->priv->drivers[n_items] = g_strdup (driver);
    self->priv->drivers[n_items + 1] = NULL;
}

void
mm_device_grab_port (MMDevice       *self,
                     MMKernelDevice *kernel_port)
{
    MMPortProbe *probe;

    if (mm_device_owns_port (self, kernel_port))
        return;

    /* Get the vendor/product IDs out of the first one that gives us
     * some valid value (it seems we may get NULL reported for VID in QMI
     * ports, e.g. Huawei E367) */
    if (!self->priv->vendor && !self->priv->product) {
        self->priv->vendor  = mm_kernel_device_get_physdev_vid (kernel_port);
        self->priv->product = mm_kernel_device_get_physdev_pid (kernel_port);
    }

    /* Add new port driver */
    add_port_driver (self, kernel_port);

    /* Create and store new port probe */
    probe = mm_port_probe_new (self, kernel_port);
    self->priv->port_probes = g_list_prepend (self->priv->port_probes, probe);

    /* Notify about the grabbed port */
    g_signal_emit (self, signals[SIGNAL_PORT_GRABBED], 0, kernel_port);
}

void
mm_device_release_port (MMDevice       *self,
                        MMKernelDevice *kernel_port)
{
    MMPortProbe *probe;

    probe = device_find_probe_with_device (self, kernel_port, TRUE);
    if (probe) {
        /* Found, remove from lists and destroy probe */
        if (g_list_find (self->priv->port_probes, probe))
            self->priv->port_probes = g_list_remove (self->priv->port_probes, probe);
        else if (g_list_find (self->priv->ignored_port_probes, probe))
            self->priv->ignored_port_probes = g_list_remove (self->priv->ignored_port_probes, probe);
        else
            g_assert_not_reached ();
        g_signal_emit (self, signals[SIGNAL_PORT_RELEASED], 0, mm_port_probe_peek_port (probe));
        g_object_unref (probe);
    }
}

void
mm_device_ignore_port  (MMDevice       *self,
                        MMKernelDevice *kernel_port)
{
    MMPortProbe *probe;

    probe = device_find_probe_with_device (self, kernel_port, FALSE);
    if (probe) {
        /* Found, remove from list and add to the ignored list */
        mm_dbg ("[device %s] fully ignoring port '%s/%s' from now on",
                self->priv->uid,
                mm_kernel_device_get_subsystem (kernel_port),
                mm_kernel_device_get_name (kernel_port));
        self->priv->port_probes = g_list_remove (self->priv->port_probes, probe);
        self->priv->ignored_port_probes = g_list_prepend (self->priv->ignored_port_probes, probe);
    }
}

/*****************************************************************************/

static void
unexport_modem (MMDevice *self)
{
    gchar *path;

    g_assert (MM_IS_BASE_MODEM (self->priv->modem));
    g_assert (G_IS_DBUS_OBJECT_MANAGER (self->priv->object_manager));

    path = g_strdup (g_dbus_object_get_object_path (G_DBUS_OBJECT (self->priv->modem)));
    if (path != NULL) {
        g_dbus_object_manager_server_unexport (self->priv->object_manager, path);
        g_object_set (self->priv->modem,
                      MM_BASE_MODEM_CONNECTION, NULL,
                      NULL);
        mm_dbg ("[device %s] unexported modem from path '%s'", self->priv->uid, path);
        g_free (path);
    }
}

/*****************************************************************************/

static void
export_modem (MMDevice *self)
{
    GDBusConnection *connection = NULL;
    static guint32 id = 0;
    gchar *path;

    g_assert (MM_IS_BASE_MODEM (self->priv->modem));
    g_assert (G_IS_DBUS_OBJECT_MANAGER (self->priv->object_manager));

    /* If modem not yet valid (not fully initialized), don't export it */
    if (!mm_base_modem_get_valid (self->priv->modem)) {
        mm_dbg ("[device %s] modem not yet fully initialized", self->priv->uid);
        return;
    }

    /* Don't export already exported modems */
    g_object_get (self->priv->modem,
                  "g-object-path", &path,
                  NULL);
    if (path) {
        g_free (path);
        mm_dbg ("[device %s] modem already exported", self->priv->uid);
        return;
    }

    /* No outstanding port tasks, so if the modem is valid we can export it */

    path = g_strdup_printf (MM_DBUS_MODEM_PREFIX "/%d", id++);
    g_object_get (self->priv->object_manager,
                  "connection", &connection,
                  NULL);
    g_object_set (self->priv->modem,
                  "g-object-path", path,
                  MM_BASE_MODEM_CONNECTION, connection,
                  NULL);
    g_object_unref (connection);

    g_dbus_object_manager_server_export (self->priv->object_manager,
                                         G_DBUS_OBJECT_SKELETON (self->priv->modem));

    mm_dbg ("[device %s] exported modem at path '%s'", self->priv->uid, path);
    mm_dbg ("[device %s]    plugin:  %s", self->priv->uid, mm_base_modem_get_plugin (self->priv->modem));
    mm_dbg ("[device %s]    vid:pid: 0x%04X:0x%04X",
            self->priv->uid,
            (mm_base_modem_get_vendor_id (self->priv->modem) & 0xFFFF),
            (mm_base_modem_get_product_id (self->priv->modem) & 0xFFFF));
    if (self->priv->virtual)
        mm_dbg ("[device %s]    virtual", self->priv->uid);

    g_free (path);
}

/*****************************************************************************/

static void
clear_modem (MMDevice *self)
{
    if (self->priv->modem_valid_id) {
        g_signal_handler_disconnect (self->priv->modem, self->priv->modem_valid_id);
        self->priv->modem_valid_id = 0;
    }

    if (self->priv->modem) {
        /* Run dispose before unref-ing, in order to cleanup the SIM object,
         * if any (which also holds a reference to the modem object) */
        g_object_run_dispose (G_OBJECT (self->priv->modem));
        g_clear_object (&(self->priv->modem));
    }
}

void
mm_device_remove_modem (MMDevice  *self)
{
    if (!self->priv->modem)
        return;

    unexport_modem (self);
    clear_modem (self);
    g_clear_object (&(self->priv->object_manager));
}

/*****************************************************************************/

static void
modem_valid (MMBaseModem *modem,
             GParamSpec  *pspec,
             MMDevice    *self)
{
    if (!mm_base_modem_get_valid (modem)) {
        GDBusObjectManagerServer *object_manager;

        object_manager = g_object_ref (self->priv->object_manager);

        /* Modem no longer valid */
        mm_device_remove_modem (self);

        if (mm_base_modem_get_reprobe (modem)) {
            GError *error = NULL;

            if (!mm_device_create_modem (self, object_manager, &error)) {
                mm_warn ("Could not recreate modem for device '%s': %s",
                         self->priv->uid,
                         error ? error->message : "unknown");
                g_error_free (error);
            } else {
                mm_dbg ("Modem recreated for device '%s'", self->priv->uid);
            }
        }

        g_object_unref (object_manager);
    } else {
        /* Modem now valid, export it, but only if we really have it around.
         * It may happen that the initialization sequence fails because the
         * modem gets disconnected, and in that case we don't really need
         * to export it */
        if (self->priv->modem)
            export_modem (self);
        else
            mm_dbg ("[device %s] not exporting modem; no longer available", self->priv->uid);
    }
}

gboolean
mm_device_create_modem (MMDevice                  *self,
                        GDBusObjectManagerServer  *object_manager,
                        GError                   **error)
{
    g_assert (self->priv->modem == NULL);
    g_assert (self->priv->object_manager == NULL);

    if (!self->priv->virtual) {
        if (!self->priv->port_probes) {
            g_set_error (error,
                         MM_CORE_ERROR,
                         MM_CORE_ERROR_FAILED,
                         "Not creating a device without ports");
            return FALSE;
        }

        mm_info ("[device %s] creating modem with plugin '%s' and '%u' ports",
                 self->priv->uid,
                 mm_plugin_get_name (self->priv->plugin),
                 g_list_length (self->priv->port_probes));
    } else {
        if (!self->priv->virtual_ports) {
            g_set_error (error,
                         MM_CORE_ERROR,
                         MM_CORE_ERROR_FAILED,
                         "Not creating a virtual device without ports");
            return FALSE;
        }

        mm_info ("[device %s] creating virtual modem with plugin '%s' and '%u' ports",
                 self->priv->uid,
                 mm_plugin_get_name (self->priv->plugin),
                 g_strv_length (self->priv->virtual_ports));
    }

    self->priv->modem = mm_plugin_create_modem (self->priv->plugin, self, error);
    if (self->priv->modem) {
        /* Keep the object manager */
        self->priv->object_manager = g_object_ref (object_manager);

        /* We want to get notified when the modem becomes valid/invalid */
        self->priv->modem_valid_id = g_signal_connect (self->priv->modem,
                                                       "notify::" MM_BASE_MODEM_VALID,
                                                       G_CALLBACK (modem_valid),
                                                       self);
    }

    return !!self->priv->modem;
}

/*****************************************************************************/

const gchar *
mm_device_get_uid (MMDevice *self)
{
    return self->priv->uid;
}

const gchar **
mm_device_get_drivers (MMDevice *self)
{
    return (const gchar **)self->priv->drivers;
}

guint16
mm_device_get_vendor (MMDevice *self)
{
    return self->priv->vendor;
}

guint16
mm_device_get_product (MMDevice *self)
{
    return self->priv->product;
}

void
mm_device_set_plugin (MMDevice *self,
                      GObject  *plugin)
{
    g_object_set (self,
                  MM_DEVICE_PLUGIN, plugin,
                  NULL);
}

GObject *
mm_device_peek_plugin (MMDevice *self)
{
    return (self->priv->plugin ?
            G_OBJECT (self->priv->plugin) :
            NULL);
}

GObject *
mm_device_get_plugin (MMDevice *self)
{
    return (self->priv->plugin ?
            g_object_ref (self->priv->plugin) :
            NULL);
}

MMBaseModem *
mm_device_peek_modem (MMDevice *self)
{
    return (self->priv->modem ?
            MM_BASE_MODEM (self->priv->modem) :
            NULL);
}

MMBaseModem *
mm_device_get_modem (MMDevice *self)
{
    return (self->priv->modem ?
            MM_BASE_MODEM (g_object_ref (self->priv->modem)) :
            NULL);
}

GObject *
mm_device_peek_port_probe (MMDevice       *self,
                           MMKernelDevice *kernel_port)
{
    MMPortProbe *probe;

    probe = device_find_probe_with_device (self, kernel_port, FALSE);
    return (probe ? G_OBJECT (probe) : NULL);
}

GObject *
mm_device_get_port_probe (MMDevice       *self,
                          MMKernelDevice *kernel_port)
{
    MMPortProbe *probe;

    probe = device_find_probe_with_device (self, kernel_port, FALSE);
    return (probe ? g_object_ref (probe) : NULL);
}

GList *
mm_device_peek_port_probe_list (MMDevice *self)
{
    return self->priv->port_probes;
}

GList *
mm_device_get_port_probe_list (MMDevice *self)
{
    GList *copy;

    copy = g_list_copy (self->priv->port_probes);
    g_list_foreach (copy, (GFunc)g_object_ref, NULL);
    return copy;
}

gboolean
mm_device_get_hotplugged (MMDevice *self)
{
    return self->priv->hotplugged;
}

/*****************************************************************************/

void
mm_device_virtual_grab_ports (MMDevice *self,
                              const gchar **ports)
{
    g_return_if_fail (ports != NULL);
    g_return_if_fail (self->priv->virtual);

    /* Setup drivers array */
    self->priv->drivers = g_malloc (2 * sizeof (gchar *));
    self->priv->drivers[0] = g_strdup ("virtual");
    self->priv->drivers[1] = NULL;

    /* Keep virtual port names */
    self->priv->virtual_ports = g_strdupv ((gchar **)ports);
}

const gchar **
mm_device_virtual_peek_ports (MMDevice *self)
{
    g_return_val_if_fail (self->priv->virtual, NULL);

    return (const gchar **)self->priv->virtual_ports;
}

gboolean
mm_device_is_virtual (MMDevice *self)
{
    return self->priv->virtual;
}

/*****************************************************************************/

MMDevice *
mm_device_new (const gchar *uid,
               gboolean     hotplugged,
               gboolean     virtual)
{
    g_return_val_if_fail (uid != NULL, NULL);

    return MM_DEVICE (g_object_new (MM_TYPE_DEVICE,
                                    MM_DEVICE_UID,        uid,
                                    MM_DEVICE_HOTPLUGGED, hotplugged,
                                    MM_DEVICE_VIRTUAL,    virtual,
                                    NULL));
}

static void
mm_device_init (MMDevice *self)
{
    /* Initialize private data */
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, MM_TYPE_DEVICE, MMDevicePrivate);
}

static void
set_property (GObject *object,
              guint prop_id,
              const GValue *value,
              GParamSpec *pspec)
{
    MMDevice *self = MM_DEVICE (object);

    switch (prop_id) {
    case PROP_UID:
        /* construct only */
        self->priv->uid = g_value_dup_string (value);
        break;
    case PROP_PLUGIN:
        g_clear_object (&(self->priv->plugin));
        self->priv->plugin = g_value_dup_object (value);
        break;
    case PROP_MODEM:
        clear_modem (self);
        self->priv->modem = g_value_dup_object (value);
        break;
    case PROP_HOTPLUGGED:
        self->priv->hotplugged = g_value_get_boolean (value);
        break;
    case PROP_VIRTUAL:
        self->priv->virtual = g_value_get_boolean (value);
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
    MMDevice *self = MM_DEVICE (object);

    switch (prop_id) {
    case PROP_UID:
        g_value_set_string (value, self->priv->uid);
        break;
    case PROP_PLUGIN:
        g_value_set_object (value, self->priv->plugin);
        break;
    case PROP_MODEM:
        g_value_set_object (value, self->priv->modem);
        break;
    case PROP_HOTPLUGGED:
        g_value_set_boolean (value, self->priv->hotplugged);
        break;
    case PROP_VIRTUAL:
        g_value_set_boolean (value, self->priv->virtual);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
dispose (GObject *object)
{
    MMDevice *self = MM_DEVICE (object);

    g_clear_object (&(self->priv->plugin));
    g_list_free_full (self->priv->port_probes, g_object_unref);
    self->priv->port_probes = NULL;
    g_list_free_full (self->priv->ignored_port_probes, g_object_unref);
    self->priv->ignored_port_probes = NULL;

    clear_modem (self);

    G_OBJECT_CLASS (mm_device_parent_class)->dispose (object);
}

static void
finalize (GObject *object)
{
    MMDevice *self = MM_DEVICE (object);

    g_free (self->priv->uid);
    g_strfreev (self->priv->drivers);
    g_strfreev (self->priv->virtual_ports);

    G_OBJECT_CLASS (mm_device_parent_class)->finalize (object);
}

static void
mm_device_class_init (MMDeviceClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (object_class, sizeof (MMDevicePrivate));

    /* Virtual methods */
    object_class->get_property = get_property;
    object_class->set_property = set_property;
    object_class->finalize     = finalize;
    object_class->dispose      = dispose;

    properties[PROP_UID] =
        g_param_spec_string (MM_DEVICE_UID,
                             "Unique ID",
                             "Unique device id, e.g. the physical device sysfs path",
                             NULL,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_UID, properties[PROP_UID]);

    properties[PROP_PLUGIN] =
        g_param_spec_object (MM_DEVICE_PLUGIN,
                             "Plugin",
                             "Best plugin to manage this device",
                             MM_TYPE_PLUGIN,
                             G_PARAM_READWRITE);
    g_object_class_install_property (object_class, PROP_PLUGIN, properties[PROP_PLUGIN]);

    properties[PROP_MODEM] =
        g_param_spec_object (MM_DEVICE_MODEM,
                             "Modem",
                             "The modem object",
                             MM_TYPE_BASE_MODEM,
                             G_PARAM_READWRITE);
    g_object_class_install_property (object_class, PROP_MODEM, properties[PROP_MODEM]);

    properties[PROP_HOTPLUGGED] =
        g_param_spec_boolean (MM_DEVICE_HOTPLUGGED,
                              "Hotplugged",
                              "Whether the modem was hotplugged",
                              FALSE,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_HOTPLUGGED, properties[PROP_HOTPLUGGED]);

    properties[PROP_VIRTUAL] =
        g_param_spec_boolean (MM_DEVICE_VIRTUAL,
                              "Virtual",
                              "Whether the device is virtual or real",
                              FALSE,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_VIRTUAL, properties[PROP_VIRTUAL]);

    signals[SIGNAL_PORT_GRABBED] =
        g_signal_new (MM_DEVICE_PORT_GRABBED,
                      G_OBJECT_CLASS_TYPE (object_class),
                      G_SIGNAL_RUN_FIRST,
                      G_STRUCT_OFFSET (MMDeviceClass, port_grabbed),
                      NULL, NULL,
                      g_cclosure_marshal_generic,
                      G_TYPE_NONE, 1, MM_TYPE_KERNEL_DEVICE);

    signals[SIGNAL_PORT_RELEASED] =
        g_signal_new (MM_DEVICE_PORT_RELEASED,
                      G_OBJECT_CLASS_TYPE (object_class),
                      G_SIGNAL_RUN_FIRST,
                      G_STRUCT_OFFSET (MMDeviceClass, port_released),
                      NULL, NULL,
                      g_cclosure_marshal_generic,
                      G_TYPE_NONE, 1, MM_TYPE_KERNEL_DEVICE);
}

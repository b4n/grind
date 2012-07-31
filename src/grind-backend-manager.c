/*
 * Copyright 2011-2012 Colomban Wendling <ban@herbesfolles.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

/* TODO: add language support, not to choose a backend that finally don't
 *       support the required language */

#include "grind-backend-manager.h"

#include <glib.h>
#include <glib-object.h>
#include <string.h>

#include "grind-indenter.h"
#include "backends/grind-astyle.h"
#include "backends/grind-gnuindent.h"
#include "backends/grind-naive.h"


struct _GrindBackendManagerPrivate
{
  GList  *backends;
};

typedef struct _BackendInfo BackendInfo;

struct _BackendInfo
{
  GType           g_type;
  gchar          *name;
  gint            priority;
  GrindIndenter  *instance;
};


G_DEFINE_TYPE (GrindBackendManager, grind_backend_manager, G_TYPE_OBJECT)


static BackendInfo *
backend_info_new (GType         g_type,
                  const gchar  *name,
                  gint          priority)
{
  BackendInfo *info;
  
  info = g_slice_alloc (sizeof *info);
  info->g_type = g_type;
  info->name = g_strdup (name);
  info->priority = priority;
  info->instance = g_object_new (info->g_type, NULL);
  
  return info;
}

static void
backend_info_free (BackendInfo *info)
{
  g_object_unref (info->instance);
  g_free (info->name);
  g_slice_free1 (sizeof *info, info);
}

static gint
backend_info_cmp (const BackendInfo *a,
                  const BackendInfo *b)
{
  gint cmp;
  
  cmp = b->priority - a->priority;
  if (! cmp) {
    cmp = strcmp (a->name, b->name);
  }
  
  return cmp;
}

static void
grind_backend_manager_finalize (GObject *object)
{
  GrindBackendManager *self = GRIND_BACKEND_MANAGER (object);
  
  g_list_free_full (self->priv->backends, (GDestroyNotify)backend_info_free);
  
  G_OBJECT_CLASS (grind_backend_manager_parent_class)->finalize (object);
}

static void
grind_backend_manager_constructed (GObject *object)
{
  GrindBackendManager *self = GRIND_BACKEND_MANAGER (object);
  
  /* register backends */
  grind_backend_manager_register_backend (self, GRIND_TYPE_BACKEND_NAIVE, 0);
  grind_backend_manager_register_backend (self, GRIND_TYPE_BACKEND_GNUINDENT, 1);
  grind_backend_manager_register_backend (self, GRIND_TYPE_BACKEND_ASTYLE, 2);
}

static void
grind_backend_manager_class_init (GrindBackendManagerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  
  object_class->finalize    = grind_backend_manager_finalize;
  object_class->constructed = grind_backend_manager_constructed;
  
  g_type_class_add_private (klass, sizeof (GrindBackendManagerPrivate));
}

static void
grind_backend_manager_init (GrindBackendManager *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GRIND_TYPE_BACKEND_MANAGER,
                                            GrindBackendManagerPrivate);
  
  self->priv->backends = NULL;
}


/**
 * grind_backend_manager_get_default:
 * 
 * Gets the default #GrindBackendManager.
 * 
 * Returns: (transfer none): The default #GrindBackendManager
 */
GrindBackendManager *
grind_backend_manager_get_default (void)
{
  static GrindBackendManager *self = NULL;
  
  if (! self) {
    self = g_object_new (GRIND_TYPE_BACKEND_MANAGER, NULL);
  }
  
  return self;
}

/**
 * grind_backend_manager_register_backend:
 * @self: A #GrindBackendManager
 * @type: The type of the backend to register
 * @priority: The priority of the backend. Priorities should depend on the
 *            quality and faturefullness of the backend.
 * 
 * Registers a backend.
 */
void
grind_backend_manager_register_backend (GrindBackendManager  *self,
                                        GType                 type,
                                        gint                  priority)
{
  GTypeQuery  query;
  
  g_type_query (type, &query);
  if (query.type == 0) {
    g_critical ("Trying to register a backend of unknown type (%lu)", type);
  } else if (! g_type_is_a (type, GRIND_TYPE_INDENTER)) {
    g_critical ("Trying to register a backend that don't implement "
                "GrindIndenter");
  } else {
    BackendInfo  *info;
    
    /* FIXME: handle duplicate cases */
    info = backend_info_new (type, query.type_name, priority);
    self->priv->backends = g_list_insert_sorted (self->priv->backends, info,
                                                 (GCompareFunc)backend_info_cmp);
    g_debug ("Registered backend %s with priority %d",
             info->name, info->priority);
  }
}

/**
 * grind_backend_manager_get_backend:
 * @self: A #BrinBackendManager
 * @name: (allow-none): The name of the preferred backend, or @NULL
 * 
 * Tries get a backend.
 * 
 * Returns: (allow-none) (transfer-full): An instance of an usable backend, or
 *                                        @NULL if none is found.
 */
GrindIndenter *
grind_backend_manager_get_backend (GrindBackendManager *self,
                                   const gchar         *name)
{
  GrindIndenter *indenter = NULL;
  
  if (self->priv->backends) {
    BackendInfo *info = NULL;
    
    if (name) {
      GList *item;
      
      for (item = self->priv->backends; item; item = item->next) {
        BackendInfo *info_tmp = item->data;
        
        if (strcmp (info_tmp->name, name) == 0) {
          info = info_tmp;
        }
      }
    }
    if (! info) {
      /* if no info found here (e.g. no name match, or name == null), take the
       * backend with the higher priority (the first one) */
      info = self->priv->backends->data;
    }
    
    indenter = g_object_ref (info->instance);
  }
  
  return indenter;
}

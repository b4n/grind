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

#ifndef H_GRIND_BACKEND_MANAGER
#define H_GRIND_BACKEND_MANAGER

#include <glib.h>
#include <glib-object.h>

#include "grind-indenter.h"

G_BEGIN_DECLS


#define GRIND_TYPE_BACKEND_MANAGER            (grind_backend_manager_get_type())
#define GRIND_BACKEND_MANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_BACKEND_MANAGER, GrindBackendManager))
#define GRIND_BACKEND_MANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GRIND_TYPE_BACKEND_MANAGER, GrindBackendManagerClass))
#define GRIND_IS_BACKEND_MANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_BACKEND_MANAGER))
#define GRIND_IS_BACKEND_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GRIND_TYPE_BACKEND_MANAGER))
#define GRIND_BACKEND_MANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GRIND_TYPE_BACKEND_MANAGER, GrindBackendManagerClass))


typedef struct _GrindBackendManager         GrindBackendManager;
typedef struct _GrindBackendManagerClass    GrindBackendManagerClass;
typedef struct _GrindBackendManagerPrivate  GrindBackendManagerPrivate;

struct _GrindBackendManager
{
  GObject parent_instance;
  GrindBackendManagerPrivate *priv;
};

struct _GrindBackendManagerClass
{
  GObjectClass parent_class;
};


GType                 grind_backend_manager_get_type          (void) G_GNUC_CONST;
GrindBackendManager  *grind_backend_manager_get_default       (void);
void                  grind_backend_manager_register_backend  (GrindBackendManager *manager,
                                                               GType                type,
                                                               gint                 priority);
GrindIndenter        *grind_backend_manager_get_backend       (GrindBackendManager *manager,
                                                               const gchar         *name);
                             


G_END_DECLS

#endif /* guard */

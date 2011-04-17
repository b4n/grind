/*
 * Copyright 2011 Colomban Wendling <ban@herbesfolles.org>
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

#ifndef H_GRIND_INDENTER
#define H_GRIND_INDENTER

#include <glib.h>
#include <glib-object.h>

#include "geanyplugin.h"
#include "document.h"

G_BEGIN_DECLS


#define GRIND_TYPE_INDENTER               (grind_indenter_get_type ())
#define GRIND_INDENTER(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_INDENTER, GrindIndenter))
#define GRIND_IS_INDENTER(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_INDENTER))
#define GRIND_INDENTER_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GRIND_TYPE_INDENTER, GrindIndenterInterface))

typedef struct _GrindIndenter           GrindIndenter;
typedef struct _GrindIndenterInterface  GrindIndenterInterface;

struct _GrindIndenterInterface
{
  GTypeInterface parent_iface;
  
  gboolean      (*indent)           (GrindIndenter *self,
                                     GeanyDocument *doc,
                                     gint           start,
                                     gint           end);
  /*const gchar  *(*get_author)       (GrindIndenter *self);
  const gchar  *(*get_version)      (GrindIndenter *self);
  const gchar  *(*get_name)         (GrindIndenter *self);
  const gchar  *(*get_description)  (GrindIndenter *self);*/
};


GType           grind_indenter_get_type         (void) G_GNUC_CONST;
gboolean        grind_indenter_indent           (GrindIndenter *self,
                                                 GeanyDocument *doc,
                                                 gint           start,
                                                 gint           end);
gchar          *grind_indenter_get_author       (GrindIndenter *self);
gchar          *grind_indenter_get_version      (GrindIndenter *self);
gchar          *grind_indenter_get_name         (GrindIndenter *self);
gchar          *grind_indenter_get_description  (GrindIndenter *self);


G_END_DECLS

#endif /* guard */

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

#ifndef H_GRIND_INDENTER
#define H_GRIND_INDENTER

#include <glib.h>
#include <glib-object.h>

#include "geanyplugin.h"
#include "document.h"

G_BEGIN_DECLS


#define GRIND_TYPE_INDENTER             (grind_indenter_get_type ())
#define GRIND_INDENTER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_INDENTER, GrindIndenter))
#define GRIND_INDENTER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GRIND_TYPE_INDENTER, GrindIndenterClass))
#define GRIND_IS_INDENTER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_INDENTER))
#define GRIND_IS_INDENTER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GRIND_TYPE_INDENTER))
#define GRIND_INDENTER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GRIND_TYPE_INDENTER, GrindIndenterClass))

#define GRIND_INDENTER_ERROR            (grind_indenter_error_quark ())

typedef enum _GrindIndenterError
{
  GRIND_INDENTER_ERROR_INVALID_TYPE,
  GRIND_INDENTER_ERROR_INVALID_VALUE,
  GRIND_INDENTER_ERROR_FAILED
} GrindIndenterError;

typedef struct _GrindIndenter       GrindIndenter;
typedef struct _GrindIndenterClass  GrindIndenterClass;

struct _GrindIndenter
{
  GObject parent;
};

struct _GrindIndenterClass
{
  GObjectClass parent_class;
  
  const gchar  *(*get_author)       (GrindIndenter *self);
  const gchar  *(*get_description)  (GrindIndenter *self);
  const gchar  *(*get_name)         (GrindIndenter *self);
  const gchar  *(*get_version)      (GrindIndenter *self);
  gboolean      (*indent)           (GrindIndenter *self,
                                     GeanyDocument *doc,
                                     gint           start,
                                     gint           end);
};


GType         grind_indenter_get_type               (void) G_GNUC_CONST;
GQuark        grind_indenter_error_quark            (void) G_GNUC_CONST;
gboolean      grind_indenter_indent                 (GrindIndenter *self,
                                                     GeanyDocument *doc,
                                                     gint           start,
                                                     gint           end);
const gchar  *grind_indenter_get_author             (GrindIndenter *self);
const gchar  *grind_indenter_get_description        (GrindIndenter *self);
const gchar  *grind_indenter_get_name               (GrindIndenter *self);
const gchar  *grind_indenter_get_version            (GrindIndenter *self);

gboolean      grind_indenter_sync_options_from_keyfile  (GrindIndenter *self,
                                                         GKeyFile      *key_file,
                                                         GError       **error);
gboolean      grind_indenter_sync_options_to_keyfile    (GrindIndenter *self,
                                                         GKeyFile      *key_file,
                                                         GError       **error);


G_END_DECLS

#endif /* guard */

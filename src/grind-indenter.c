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

#include "grind-indenter.h"

#include <glib.h>
#include <glib-object.h>

#include "geanyplugin.h"
#include "document.h"


enum
{
  PROP_0,
  
  PROP_AUTHOR,
  PROP_VERSION,
  PROP_NAME,
  PROP_DESCRIPTION
};


static void     grind_indenter_get_property   (GObject     *object,
                                               guint        prop_id,
                                               GValue      *value,
                                               GParamSpec  *pspec);


G_DEFINE_ABSTRACT_TYPE (GrindIndenter, grind_indenter, G_TYPE_OBJECT)


static void
grind_indenter_class_init (GrindIndenterClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  
  object_class->get_property = grind_indenter_get_property;
  
  g_object_class_install_property (object_class, PROP_AUTHOR,
                                   g_param_spec_string ("author",
                                                        "Author",
                                                        "Backend author",
                                                        NULL,
                                                        G_PARAM_STATIC_NAME |
                                                        G_PARAM_STATIC_NICK |
                                                        G_PARAM_STATIC_BLURB |
                                                        G_PARAM_READABLE));
  g_object_class_install_property (object_class, PROP_DESCRIPTION,
                                   g_param_spec_string ("description",
                                                        "Description",
                                                        "Backend description",
                                                        NULL,
                                                        G_PARAM_STATIC_NAME |
                                                        G_PARAM_STATIC_NICK |
                                                        G_PARAM_STATIC_BLURB |
                                                        G_PARAM_READABLE));
  g_object_class_install_property (object_class, PROP_NAME,
                                   g_param_spec_string ("name",
                                                        "Name",
                                                        "Backend name",
                                                        NULL,
                                                        G_PARAM_STATIC_NAME |
                                                        G_PARAM_STATIC_NICK |
                                                        G_PARAM_STATIC_BLURB |
                                                        G_PARAM_READABLE));
  g_object_class_install_property (object_class, PROP_VERSION,
                                   g_param_spec_string ("version",
                                                        "Version",
                                                        "Backend version",
                                                        NULL,
                                                        G_PARAM_STATIC_NAME |
                                                        G_PARAM_STATIC_NICK |
                                                        G_PARAM_STATIC_BLURB |
                                                        G_PARAM_READABLE));
}

static void
grind_indenter_init (GrindIndenter *self)
{
  
}

static void
grind_indenter_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  GrindIndenter *self = GRIND_INDENTER (object);
  
  switch (prop_id) {
    case PROP_AUTHOR:
      g_value_set_string (value, grind_indenter_get_author (self));
      break;
    
    case PROP_DESCRIPTION:
      g_value_set_string (value, grind_indenter_get_description (self));
      break;
    
    case PROP_NAME:
      g_value_set_string (value, grind_indenter_get_name (self));
      break;
    
    case PROP_VERSION:
      g_value_set_string (value, grind_indenter_get_version (self));
      break;
    
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}


gboolean
grind_indenter_indent (GrindIndenter *self,
                       GeanyDocument *doc,
                       gint           start,
                       gint           end)
{
  return GRIND_INDENTER_GET_CLASS (self)->indent (self, doc, start, end);
}

const gchar *
grind_indenter_get_author (GrindIndenter *self)
{
  return GRIND_INDENTER_GET_CLASS (self)->get_author (self);
}

const gchar *
grind_indenter_get_description (GrindIndenter *self)
{
  return GRIND_INDENTER_GET_CLASS (self)->get_description (self);
}

const gchar *
grind_indenter_get_name (GrindIndenter *self)
{
  return GRIND_INDENTER_GET_CLASS (self)->get_name (self);
}

const gchar *
grind_indenter_get_version (GrindIndenter *self)
{
  return GRIND_INDENTER_GET_CLASS (self)->get_version (self);
}

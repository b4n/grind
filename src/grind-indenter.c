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


G_DEFINE_INTERFACE (GrindIndenter, grind_indenter, G_TYPE_OBJECT)


static void
grind_indenter_default_init (GrindIndenterInterface *iface)
{
  static gboolean initialized = FALSE;
  
  if (G_UNLIKELY (! initialized)) {
    initialized = TRUE;
    
    g_object_interface_install_property (iface,
                                         g_param_spec_string ("author",
                                                              "Author",
                                                              "Backend author",
                                                              NULL,
                                                              G_PARAM_STATIC_NAME |
                                                              G_PARAM_STATIC_NICK |
                                                              G_PARAM_STATIC_BLURB |
                                                              G_PARAM_READABLE));
    g_object_interface_install_property (iface,
                                         g_param_spec_string ("version",
                                                              "Version",
                                                              "Backend version",
                                                              NULL,
                                                              G_PARAM_STATIC_NAME |
                                                              G_PARAM_STATIC_NICK |
                                                              G_PARAM_STATIC_BLURB |
                                                              G_PARAM_READABLE));
    g_object_interface_install_property (iface,
                                         g_param_spec_string ("name",
                                                              "Name",
                                                              "Backend name",
                                                              NULL,
                                                              G_PARAM_STATIC_NAME |
                                                              G_PARAM_STATIC_NICK |
                                                              G_PARAM_STATIC_BLURB |
                                                              G_PARAM_READABLE));
    g_object_interface_install_property (iface,
                                         g_param_spec_string ("description",
                                                              "Description",
                                                              "Backend description",
                                                              NULL,
                                                              G_PARAM_STATIC_NAME |
                                                              G_PARAM_STATIC_NICK |
                                                              G_PARAM_STATIC_BLURB |
                                                              G_PARAM_READABLE));
  }
}

gboolean
grind_indenter_indent (GrindIndenter *self,
                       GeanyDocument *doc,
                       gint           start,
                       gint           end)
{
  return GRIND_INDENTER_GET_INTERFACE (self)->indent (self, doc, start, end);
}

gchar *
grind_indenter_get_author (GrindIndenter *self)
{
  gchar *author;
  
  g_object_get (self, "author", &author, NULL);
  
  return author;
}

gchar *
grind_indenter_get_version (GrindIndenter *self)
{
  gchar *version;
  
  g_object_get (self, "version", &version, NULL);
  
  return version;
}

gchar *
grind_indenter_get_name (GrindIndenter *self)
{
  gchar *name;
  
  g_object_get (self, "name", &name, NULL);
  
  return name;
}

gchar *
grind_indenter_get_description (GrindIndenter *self)
{
  gchar *description;
  
  g_object_get (self, "description", &description, NULL);
  
  return description;
}

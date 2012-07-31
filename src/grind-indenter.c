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

#include "grind-indenter.h"

#include <glib.h>
#include <glib-object.h>

#include "grind-indenter-option.h"

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


/* <standard> */
GQuark
grind_indenter_error_quark (void)
{
  static GQuark error_quark = 0;
  
  if (G_UNLIKELY (error_quark == 0)) {
    error_quark = g_quark_from_static_string ("GrindIndenterOptionError");
  }
  
  return error_quark;
}


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



static gboolean
grind_indenter_sync_option_from_keyfile (GrindIndenterOption *option,
                                         GKeyFile            *key_file,
                                         const gchar         *group_name,
                                         GrindIndenter       *object,
                                         GError             **error)
{
  GParamSpec   *pspec = (GParamSpec *) option;
  const gchar  *name;
  gboolean      success = FALSE;
  
  name = g_param_spec_get_name (pspec);
  switch (G_TYPE_FUNDAMENTAL (pspec->value_type)) {
    
    #define HANDLE_TYPE(T_N, min, max, k_t_n, k_c_t, c_t)                      \
      case G_TYPE_##T_N: {                                                     \
        GError *err = NULL;                                                    \
        k_c_t   kf_val;                                                        \
                                                                               \
        kf_val = g_key_file_get_##k_t_n (key_file, group_name, name, &err);    \
        if (err) {                                                             \
          g_propagate_error (error, err);                                      \
        } else if (kf_val < min || kf_val > max) {                             \
          g_set_error (error, GRIND_INDENTER_ERROR,                            \
                       GRIND_INDENTER_ERROR_INVALID_VALUE,                     \
                       "Value for option \"%s\" is not valid", name);          \
        } else {                                                               \
          c_t val = (c_t) kf_val;                                              \
          g_object_set (object, name, val, NULL);                              \
          success = TRUE;                                                      \
        }                                                                      \
      } break;
    
    HANDLE_TYPE (CHAR,   G_MININT8,   G_MAXINT8,   integer, gint,    gint8)
    HANDLE_TYPE (UCHAR,  0U,          G_MAXUINT8,  integer, gint,    guint8)
    HANDLE_TYPE (INT,    G_MININT,    G_MAXINT,    integer, gint,    gint)
    HANDLE_TYPE (UINT,   0U,          G_MAXUINT,   uint64,  guint64, guint)
    HANDLE_TYPE (INT64,  G_MININT64,  G_MAXINT64,  int64,   gint64,  gint64)
    HANDLE_TYPE (UINT64, 0U,          G_MAXUINT64, uint64,  guint64, guint64)
    HANDLE_TYPE (LONG,   G_MINLONG,   G_MAXLONG,   int64,   gint64,  glong)
    HANDLE_TYPE (ULONG,  0U,          G_MAXULONG,  uint64,  guint64, gulong)
    HANDLE_TYPE (FLOAT,  G_MINFLOAT,  G_MAXFLOAT,  double,  gdouble, gfloat)
    HANDLE_TYPE (DOUBLE, G_MINDOUBLE, G_MAXDOUBLE, double,  gdouble, gdouble)
    
    #undef HANDLE_TYPE
    
    case G_TYPE_BOOLEAN: {
      GError   *err = NULL;
      gboolean  val;
      
      val = g_key_file_get_boolean (key_file, group_name, name, &err);
      if (err) {
        g_propagate_error (error, err);
      } else {
        g_object_set (object, name, val, NULL);
        success = TRUE;
      }
    } break;
    
    case G_TYPE_ENUM: {
      gchar *val;
      
      val = g_key_file_get_string (key_file, group_name, name, error);
      if (val) {
        GEnumClass *enum_class;
        GEnumValue *enum_value;
        
        enum_class = g_type_class_ref (pspec->value_type);
        enum_value = g_enum_get_value_by_nick (enum_class, val);
        if (enum_class) {
          g_object_set (object, name, enum_value->value, NULL);
          success = TRUE;
        } else {
          g_set_error (error, GRIND_INDENTER_ERROR,
                       GRIND_INDENTER_ERROR_INVALID_VALUE,
                       "Value \"%s\" for option \"%s\" is not valid",
                       val, name);
        }
        g_type_class_unref (enum_class);
        g_free (val);
      }
    } break;
    
    case G_TYPE_STRING: {
      gchar *val;
      
      val = g_key_file_get_string (key_file, group_name, name, error);
      if (val) {
        g_object_set (object, name, val, NULL);
        g_free (val);
        success = TRUE;
      }
    } break;
    
    default:
      g_set_error (error, GRIND_INDENTER_ERROR,
                   GRIND_INDENTER_ERROR_INVALID_TYPE,
                   "Unsupported type %s for option %s",
                   g_type_name (pspec->value_type), name);
      break;
  }
  
  return success;
}

static gboolean
grind_indenter_sync_option_to_keyfile (GrindIndenterOption *option,
                                       GKeyFile            *key_file,
                                       const gchar         *group_name,
                                       GrindIndenter       *object,
                                       GError             **error)
{
  GParamSpec   *pspec = (GParamSpec *) option;
  const gchar  *name;
  gboolean      success = TRUE;
  
  name = g_param_spec_get_name (pspec);
  switch (G_TYPE_FUNDAMENTAL (pspec->value_type)) {
    
    #define HANDLE_TYPE(T_N, k_t_n, k_c_t, c_t)                                \
      case G_TYPE_##T_N: {                                                     \
        k_c_t kf_val;                                                          \
        c_t   val;                                                             \
                                                                               \
        g_object_get (object, name, &val, NULL);                               \
        kf_val = (k_c_t) val;                                                  \
        g_key_file_set_##k_t_n (key_file, group_name, name, kf_val);           \
      } break;
    
    HANDLE_TYPE (CHAR,   integer, gint,    gint8)
    HANDLE_TYPE (UCHAR,  integer, gint,    guint8)
    HANDLE_TYPE (INT,    integer, gint,    gint)
    HANDLE_TYPE (UINT,   uint64,  guint64, guint)
    HANDLE_TYPE (INT64,  int64,   gint64,  gint64)
    HANDLE_TYPE (UINT64, uint64,  guint64, guint64)
    HANDLE_TYPE (LONG,   int64,   gint64,  glong)
    HANDLE_TYPE (ULONG,  uint64,  guint64, gulong)
    HANDLE_TYPE (FLOAT,  double,  gdouble, gfloat)
    HANDLE_TYPE (DOUBLE, double,  gdouble, gdouble)
    
    #undef HANDLE_TYPE
    
    case G_TYPE_BOOLEAN: {
      gboolean val;
      
      g_object_get (object, name, &val, NULL);
      g_key_file_set_boolean (key_file, group_name, name, val);
    } break;
    
    case G_TYPE_ENUM: {
      GEnumClass *enum_class;
      GEnumValue *enum_value;
      gint        val;
      
      g_object_get (object, name, &val, NULL);
      
      enum_class = g_type_class_ref (pspec->value_type);
      enum_value = g_enum_get_value (enum_class, val);
      if (! enum_value) {
        g_critical ("Failed to get name of value %d for enumeration %s",
                    val, g_type_name (pspec->value_type));
      } else {
        g_key_file_set_string (key_file, group_name, name,
                               enum_value->value_nick);
      }
      g_type_class_unref (enum_class);
    } break;
    
    case G_TYPE_STRING: {
      gchar *val;
      
      g_object_get (object, name, &val, NULL);
      g_key_file_set_string (key_file, group_name, name, val);
      g_free (val);
    } break;
    
    default:
      g_set_error (error, GRIND_INDENTER_ERROR,
                   GRIND_INDENTER_ERROR_INVALID_TYPE,
                   "Unsupported type %s for option %s",
                   g_type_name (pspec->value_type), name);
      success = FALSE;
      break;
  }
  
  return success;
}

gboolean
grind_indenter_sync_options_from_keyfile (GrindIndenter *self,
                                          GKeyFile      *key_file,
                                          GError       **error)
{
  GrindIndenterOption **options;
  guint                 n_options;
  guint                 i;
  gboolean              success = TRUE;
  const gchar          *group_name;
  
  group_name = grind_indenter_get_name (self);
  options = grind_indenter_option_list_class_options (G_OBJECT_GET_CLASS (self),
                                                      &n_options);
  for (i = 0; i < n_options && success; i++) {
    success = grind_indenter_sync_option_from_keyfile (options[i], key_file,
                                                       group_name, self, error);
  }
  g_free (options);
  
  return success;
}

gboolean
grind_indenter_sync_options_to_keyfile (GrindIndenter *self,
                                        GKeyFile      *key_file,
                                        GError       **error)
{
  GrindIndenterOption **options;
  guint                 n_options;
  guint                 i;
  gboolean              success = TRUE;
  const gchar          *group_name;
  
  group_name = grind_indenter_get_name (self);
  options = grind_indenter_option_list_class_options (G_OBJECT_GET_CLASS (self),
                                                      &n_options);
  for (i = 0; i < n_options && success; i++) {
    success = grind_indenter_sync_option_to_keyfile (options[i], key_file,
                                                     group_name, self, error);
  }
  g_free (options);
  
  return success;
}

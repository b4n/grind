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

/*
 * A GParamSpec subclass that automates getting an setting properties by
 * reading or writing to an object's private structure, given the field
 * offset.
 * 
 * This is inspired by GProperty draft, with some less features and some
 * things implemented another way.
 */

#include "grind-indenter-option.h"

#include <glib.h>
#include <glib-object.h>


/* the general stuff */

G_DEFINE_TYPE (GrindIndenterOption,
               grind_indenter_option,
               G_TYPE_PARAM)

static void
grind_indenter_option_class_init (GrindIndenterOptionClass *klass)
{
  GParamSpecClass *pspec_class = G_PARAM_SPEC_CLASS (klass);
  
  pspec_class->value_type = G_TYPE_INVALID;
}

static void
grind_indenter_option_init (GrindIndenterOption *self)
{
  GParamSpec *pspec = G_PARAM_SPEC (self);
  
  pspec->value_type = G_TYPE_INVALID;
  pspec->owner_type = G_TYPE_INVALID;
  
  self->field_offset = 0;
}

static inline void
grind_indenter_option_get_value (GrindIndenterOption *option,
                                 gpointer             gobject,
                                 GValue              *value)
{
  GRIND_INDENTER_OPTION_GET_CLASS (option)->get_value (option, gobject, value);
}

static inline void
grind_indenter_option_set_value (GrindIndenterOption *option,
                                 gpointer             gobject,
                                 const GValue        *value)
{
  GRIND_INDENTER_OPTION_GET_CLASS (option)->set_value (option, gobject, value);
}

/* a get_property() implementation that automates property getting for
 * GrindIndenterOptions */
void
grind_indenter_option_get_property_impl (GObject    *object,
                                         guint       prop_id,
                                         GValue     *value,
                                         GParamSpec *pspec)
{
  g_return_if_fail (GRIND_IS_INDENTER_OPTION (pspec));
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (pspec->flags & G_PARAM_READABLE);
  
  grind_indenter_option_get_value (GRIND_INDENTER_OPTION (pspec),
                                   object, value);
}

/* a set_property() implementation that automates property setting for
 * GrindIndenterOptions */
void
grind_indenter_option_set_property_impl (GObject      *object,
                                         guint         prop_id,
                                         const GValue *value,
                                         GParamSpec   *pspec)
{
  g_return_if_fail (GRIND_IS_INDENTER_OPTION (pspec));
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (pspec->flags & G_PARAM_WRITABLE);
  
  grind_indenter_option_set_value (GRIND_INDENTER_OPTION (pspec),
                                   object, value);
}

/* lists all options of a given GObjectClass */
GrindIndenterOption **
grind_indenter_option_list_class_options (GObjectClass *klass,
                                          guint        *n_options_)
{
  GParamSpec          **pspecs;
  GrindIndenterOption **options;
  GSList               *option_list = NULL;
  GSList               *item;
  guint                 i;
  guint                 n_pspecs;
  guint                 n_options = 0;
  
  g_return_val_if_fail (G_IS_OBJECT_CLASS (klass), NULL);
  
  pspecs = g_object_class_list_properties (klass, &n_pspecs);
  for (i = 0; i < n_pspecs; i++) {
    if (GRIND_IS_INDENTER_OPTION (pspecs[i])) {
      option_list = g_slist_prepend (option_list, pspecs[i]);
    }
  }
  option_list = g_slist_reverse (option_list);
  g_free (pspecs);
  
  options = g_malloc (sizeof *options * n_pspecs);
  for (item = option_list; item != NULL; item = item->next) {
    options[n_options++] = item->data;
  }
  *n_options_ = n_options;
  g_slist_free (option_list);
  
  return options;
}

/* gets a pointer to the field storing an option's value */
static inline gpointer
grind_indenter_option_get_field_p (GrindIndenterOption *option,
                                   gpointer             gobject)
{
  gpointer priv = g_type_instance_get_private ((GTypeInstance*) gobject,
                                               ((GParamSpec *) option)->owner_type);
  
  g_return_val_if_fail (priv != NULL, NULL);
  
  return G_STRUCT_MEMBER_P (priv, option->field_offset);
}


/* The options themeselves */


/* defines a integer-style option type, with minimum, maximum, default */
#define DEFINE_SIMPLE_OPTION_TYPE(Tn, T_N, t_n, c_t)                           \
/*                                                                               \
typedef struct _Grind##Tn##IndenterOption Grind##Tn##IndenterOption;           \
typedef GrindIndenterOptionClass          Grind##Tn##IndenterOptionClass;      \
                                                                               \
struct _Grind##Tn##IndenterOption                                              \
{                                                                              \
  GrindIndenterOption parent;                                                  \
                                                                               \
  c_t min_value;                                                               \
  c_t max_value;                                                               \
  c_t default_value;                                                           \
};                                                                             \
                                                                               \
GType grind_##t_n##_indenter_option_get_type (void) G_GNUC_CONST;              \
*/                                                                               \
G_DEFINE_TYPE (Grind##Tn##IndenterOption,                                      \
               grind_##t_n##_indenter_option,                                  \
               GRIND_TYPE_INDENTER_OPTION)                                     \
                                                                               \
static void                                                                    \
grind_##t_n##_indenter_option_value_set_default (GParamSpec  *pspec,           \
                                                 GValue      *value)           \
{                                                                              \
  Grind##Tn##IndenterOption *internal = (Grind##Tn##IndenterOption *) pspec;   \
                                                                               \
  g_value_set_##t_n (value, internal->default_value);                          \
}                                                                              \
                                                                               \
static gint                                                                    \
grind_##t_n##_indenter_option_values_cmp (GParamSpec   *pspec,                 \
                                          const GValue *value_a,               \
                                          const GValue *value_b)               \
{                                                                              \
  c_t a = g_value_get_##t_n (value_a);                                         \
  c_t b = g_value_get_##t_n (value_b);                                         \
                                                                               \
  if (a < b) {                                                                 \
    return -1;                                                                 \
  } else if (a > b) {                                                          \
    return 1;                                                                  \
  } else {                                                                     \
    return 0;                                                                  \
  }                                                                            \
}                                                                              \
                                                                               \
static gboolean                                                                \
grind_##t_n##_indenter_option_value_validate (GParamSpec *pspec,               \
                                              GValue     *value)               \
{                                                                              \
  Grind##Tn##IndenterOption  *internal = (Grind##Tn##IndenterOption *) pspec;  \
  c_t                         val = g_value_get_##t_n (value);                 \
                                                                               \
  if (val < internal->min_value || val > internal->max_value) {                \
    g_value_set_##t_n (value, internal->default_value);                        \
    return TRUE;                                                               \
  } else {                                                                     \
    return FALSE;                                                              \
  }                                                                            \
}                                                                              \
                                                                               \
static void                                                                    \
grind_##t_n##_indenter_option_set_value (GrindIndenterOption *option,          \
                                         gpointer             gobject,         \
                                         const GValue        *value)           \
{                                                                              \
  gpointer  field_p = grind_indenter_option_get_field_p (option, gobject);     \
  c_t       val     = g_value_get_##t_n (value);                               \
  c_t      *val_p   = (c_t *) field_p;                                         \
                                                                               \
  if (val != *val_p) {                                                         \
    *val_p = val;                                                              \
    g_object_notify_by_pspec (gobject, (GParamSpec *) option);                 \
  }                                                                            \
}                                                                              \
                                                                               \
static void                                                                    \
grind_##t_n##_indenter_option_get_value (GrindIndenterOption *option,          \
                                         gpointer             gobject,         \
                                         GValue              *value)           \
{                                                                              \
  gpointer field_p = grind_indenter_option_get_field_p (option, gobject);      \
                                                                               \
  g_value_set_##t_n (value, (* (c_t *) field_p));                              \
}                                                                              \
                                                                               \
static void                                                                    \
grind_##t_n##_indenter_option_class_init (Grind##Tn##IndenterOptionClass *klass)\
{                                                                              \
  GParamSpecClass          *pspec_class  = G_PARAM_SPEC_CLASS (klass);         \
  GrindIndenterOptionClass *option_class = GRIND_INDENTER_OPTION_CLASS (klass);\
                                                                               \
  pspec_class->value_type         = G_TYPE_##T_N;                              \
  pspec_class->value_set_default  = grind_##t_n##_indenter_option_value_set_default;\
  pspec_class->value_validate     = grind_##t_n##_indenter_option_value_validate;\
  pspec_class->values_cmp         = grind_##t_n##_indenter_option_values_cmp;  \
                                                                               \
  option_class->get_value         = grind_##t_n##_indenter_option_get_value;   \
  option_class->set_value         = grind_##t_n##_indenter_option_set_value;   \
}                                                                              \
                                                                               \
static void                                                                    \
grind_##t_n##_indenter_option_init (Grind##Tn##IndenterOption *option)         \
{                                                                              \
                                                                               \
}                                                                              \
                                                                               \
GParamSpec *                                                                   \
grind_##t_n##_indenter_option_new (const gchar *name,                          \
                                   const gchar *nick,                          \
                                   const gchar *blurb,                         \
                                   c_t          min_value,                     \
                                   c_t          max_value,                     \
                                   c_t          default_value,                 \
                                   GParamFlags  flags,                         \
                                   gsize        offset)                        \
{                                                                              \
  GParamSpec                *pspec;                                            \
  GrindIndenterOption       *option;                                           \
  Grind##Tn##IndenterOption *internal;                                         \
                                                                               \
  pspec = g_param_spec_internal (grind_##t_n##_indenter_option_get_type (),    \
                                 name, nick, blurb, flags);                    \
                                                                               \
  pspec->value_type = G_TYPE_##T_N;                                            \
                                                                               \
  option = (GrindIndenterOption *) pspec;                                      \
  option->field_offset = offset;                                               \
                                                                               \
  internal = (Grind##Tn##IndenterOption *) option;                             \
  internal->min_value     = min_value;                                         \
  internal->max_value     = max_value;                                         \
  internal->default_value = default_value;                                     \
                                                                               \
  return (GParamSpec *) option;                                                \
}


DEFINE_SIMPLE_OPTION_TYPE(Char,   CHAR,   char,   gint8)
DEFINE_SIMPLE_OPTION_TYPE(UChar,  UCHAR,  uchar,  guint8)
DEFINE_SIMPLE_OPTION_TYPE(Int,    INT,    int,    gint)
DEFINE_SIMPLE_OPTION_TYPE(UInt,   UINT,   uint,   guint)
DEFINE_SIMPLE_OPTION_TYPE(Int64,  INT64,  int64,  gint64)
DEFINE_SIMPLE_OPTION_TYPE(UInt64, UINT64, uint64, guint64)
DEFINE_SIMPLE_OPTION_TYPE(Long,   LONG,   long,   glong)
DEFINE_SIMPLE_OPTION_TYPE(ULong,  ULONG,  ulong,  gulong)
DEFINE_SIMPLE_OPTION_TYPE(Float,  FLOAT,  float,  gfloat)
DEFINE_SIMPLE_OPTION_TYPE(Double, DOUBLE, double, gdouble)


/* Boolean */

G_DEFINE_TYPE (GrindBooleanIndenterOption,
               grind_boolean_indenter_option,
               GRIND_TYPE_INDENTER_OPTION)

static void
grind_boolean_indenter_option_value_set_default (GParamSpec *pspec,
                                                GValue     *value)
{
  GrindBooleanIndenterOption *internal = (GrindBooleanIndenterOption *) pspec;
  
  g_value_set_boolean (value, internal->default_value);
}

static gint
grind_boolean_indenter_option_values_cmp (GParamSpec   *pspec,
                                          const GValue *value_a,
                                          const GValue *value_b)
{
  gboolean a = g_value_get_boolean (value_a);
  gboolean b = g_value_get_boolean (value_b);
  
  return a - b;
}

static void
grind_boolean_indenter_option_set_value (GrindIndenterOption *option,
                                         gpointer             gobject,
                                         const GValue        *value)
{
  gpointer  field_p = grind_indenter_option_get_field_p (option, gobject);
  gboolean  val     = g_value_get_boolean (value);
  gboolean *val_p   = (gboolean *) field_p;
  
  if (val != *val_p) {
    *val_p = val;
    g_object_notify_by_pspec (gobject, (GParamSpec *) option);
  }
}

static void
grind_boolean_indenter_option_get_value (GrindIndenterOption *option,
                                         gpointer             gobject,
                                         GValue              *value)
{
  gpointer field_p = grind_indenter_option_get_field_p (option, gobject);
  
  g_value_set_boolean (value, (* (gboolean *) field_p));
}

static void
grind_boolean_indenter_option_class_init (GrindBooleanIndenterOptionClass *klass)
{
  GParamSpecClass          *pspec_class   = G_PARAM_SPEC_CLASS (klass);
  GrindIndenterOptionClass *option_class  = GRIND_INDENTER_OPTION_CLASS (klass);
  
  pspec_class->value_type         = G_TYPE_BOOLEAN;
  pspec_class->value_set_default  = grind_boolean_indenter_option_value_set_default;
  pspec_class->values_cmp         = grind_boolean_indenter_option_values_cmp;
  
  option_class->get_value         = grind_boolean_indenter_option_get_value;
  option_class->set_value         = grind_boolean_indenter_option_set_value;
}

static void
grind_boolean_indenter_option_init (GrindBooleanIndenterOption *pspec)
{
  
}

GParamSpec *
grind_boolean_indenter_option_new (const gchar *name,
                                   const gchar *nick,
                                   const gchar *blurb,
                                   gboolean     default_value,
                                   GParamFlags  flags,
                                   gsize        offset)
{
  GParamSpec                 *pspec;
  GrindIndenterOption        *option;
  GrindBooleanIndenterOption *internal;
  
  pspec = g_param_spec_internal (grind_boolean_indenter_option_get_type (),
                                 name, nick, blurb, flags);
  
  pspec->value_type = G_TYPE_BOOLEAN;
  
  option = (GrindIndenterOption *) pspec;
  option->field_offset = offset;
  
  internal = (GrindBooleanIndenterOption *) option;
  internal->default_value = default_value;
  
  return (GParamSpec *) option;
}


/* Enum */

G_DEFINE_TYPE (GrindEnumIndenterOption,
               grind_enum_indenter_option,
               GRIND_TYPE_INDENTER_OPTION)

static void
grind_enum_indenter_option_value_set_default (GParamSpec *pspec,
                                              GValue     *value)
{
  GrindEnumIndenterOption *internal = (GrindEnumIndenterOption *) pspec;
  
  g_value_set_enum (value, internal->default_value);
}

static gint
grind_enum_indenter_option_values_cmp (GParamSpec   *pspec,
                                       const GValue *value_a,
                                       const GValue *value_b)
{
  gint a = g_value_get_enum (value_a);
  gint b = g_value_get_enum (value_b);
  
  if (a < b) {
    return -1;
  }
  if (a > b) {
    return 1;
  }
  return 0;
}

static gboolean
grind_enum_indenter_option_value_validate (GParamSpec *pspec,
                                           GValue     *value)
{
  GrindEnumIndenterOption  *internal = (GrindEnumIndenterOption *) pspec;
  gboolean                  valid = FALSE;
  gint                      enum_value = g_value_get_enum (value);
  
  if (enum_value >= internal->enum_class->minimum &&
      enum_value <= internal->enum_class->maximum) {
    guint i;
    
    for (i = 0; i < internal->enum_class->n_values && ! valid; i++) {
      if (internal->enum_class->values[i].value == enum_value) {
        valid = TRUE;
      }
    }
  }
  if (! valid) {
    g_value_set_enum (value, internal->default_value);
  }
  
  return ! valid;
}

static void
grind_enum_indenter_option_set_value (GrindIndenterOption *option,
                                      gpointer             gobject,
                                      const GValue        *value)
{
  gpointer  field_p     = grind_indenter_option_get_field_p (option, gobject);
  gint      enum_value  = g_value_get_enum (value);
  gint     *enum_p      = (gint *) field_p;
  
  if (enum_value != *enum_p) {
    *enum_p = enum_value;
    g_object_notify_by_pspec (gobject, (GParamSpec *) option);
  }
}

static void
grind_enum_indenter_option_get_value (GrindIndenterOption *option,
                                      gpointer             gobject,
                                      GValue              *value)
{
  gpointer field_p = grind_indenter_option_get_field_p (option, gobject);
  
  g_value_set_enum (value, (* (gint *) field_p));
}

static void
grind_enum_indenter_option_finalize (GParamSpec *pspec)
{
  GrindEnumIndenterOption *internal = (GrindEnumIndenterOption *) pspec;
  
  if (internal->enum_class) {
    g_type_class_unref (internal->enum_class);
    internal->enum_class = NULL;
  }
  
  G_PARAM_SPEC_CLASS (grind_enum_indenter_option_parent_class)->finalize (pspec);
}

static void
grind_enum_indenter_option_class_init (GrindEnumIndenterOptionClass *klass)
{
  GParamSpecClass          *pspec_class   = G_PARAM_SPEC_CLASS (klass);
  GrindIndenterOptionClass *option_class  = GRIND_INDENTER_OPTION_CLASS (klass);
  
  pspec_class->value_type         = G_TYPE_ENUM;
  pspec_class->value_set_default  = grind_enum_indenter_option_value_set_default;
  pspec_class->value_validate     = grind_enum_indenter_option_value_validate;
  pspec_class->values_cmp         = grind_enum_indenter_option_values_cmp;
  pspec_class->finalize           = grind_enum_indenter_option_finalize;
  
  option_class->get_value         = grind_enum_indenter_option_get_value;
  option_class->set_value         = grind_enum_indenter_option_set_value;
}

static void
grind_enum_indenter_option_init (GrindEnumIndenterOption *pspec)
{
  
}

GParamSpec *
grind_enum_indenter_option_new (const gchar *name,
                                const gchar *nick,
                                const gchar *blurb,
                                GType        enum_type,
                                gint         default_value,
                                GParamFlags  flags,
                                gsize        offset)
{
  GParamSpec               *pspec;
  GrindIndenterOption      *option;
  GrindEnumIndenterOption  *internal;
  
  pspec = g_param_spec_internal (grind_enum_indenter_option_get_type (),
                                 name, nick, blurb, flags);
  
  pspec->value_type = enum_type;
  
  option = (GrindIndenterOption *) pspec;
  option->field_offset = offset;
  
  internal = (GrindEnumIndenterOption *) option;
  internal->enum_class    = g_type_class_ref (pspec->value_type);
  internal->default_value = default_value;
  
  return (GParamSpec *) option;
}


/* String */

G_DEFINE_TYPE (GrindStringIndenterOption,
               grind_string_indenter_option,
               GRIND_TYPE_INDENTER_OPTION)

static void
grind_string_indenter_option_value_set_default (GParamSpec  *pspec,
                                                GValue      *value)
{
  GrindStringIndenterOption *internal = (GrindStringIndenterOption *) pspec;
  
  g_value_set_string (value, internal->default_value);
}

static gint
grind_string_indenter_option_values_cmp (GParamSpec   *pspec,
                                         const GValue *value_a,
                                         const GValue *value_b)
{
  return g_strcmp0 (g_value_get_string (value_a), g_value_get_string (value_b));
}

static void
grind_string_indenter_option_set_value (GrindIndenterOption *option,
                                        gpointer             gobject,
                                        const GValue        *value)
{
  gpointer      field_p   = grind_indenter_option_get_field_p (option, gobject);
  const gchar  *str_value = g_value_get_string (value);
  gchar       **str_p     = (gchar **) field_p;
  
  if (g_strcmp0 (str_value, *str_p) != 0) {
    g_free (*str_p);
    *str_p = g_strdup (str_value);
    g_object_notify_by_pspec (gobject, (GParamSpec *) option);
  }
}

static void
grind_string_indenter_option_get_value (GrindIndenterOption *option,
                                        gpointer             gobject,
                                        GValue              *value)
{
  gpointer field_p = grind_indenter_option_get_field_p (option, gobject);
  
  g_value_set_string (value, (* (gpointer *) field_p));
}

static void
grind_string_indenter_option_finalize (GParamSpec *pspec)
{
  GrindStringIndenterOption *internal = (GrindStringIndenterOption *) pspec;
  
  g_free (internal->default_value);
  internal->default_value = NULL;
  
  G_PARAM_SPEC_CLASS (grind_string_indenter_option_parent_class)->finalize (pspec);
}

static void
grind_string_indenter_option_class_init (GrindStringIndenterOptionClass *klass)
{
  GParamSpecClass          *pspec_class   = G_PARAM_SPEC_CLASS (klass);
  GrindIndenterOptionClass *option_class  = GRIND_INDENTER_OPTION_CLASS (klass);
  
  pspec_class->value_type         = G_TYPE_STRING;
  pspec_class->finalize           = grind_string_indenter_option_finalize;
  pspec_class->value_set_default  = grind_string_indenter_option_value_set_default;
  pspec_class->values_cmp         = grind_string_indenter_option_values_cmp;
  
  option_class->get_value         = grind_string_indenter_option_get_value;
  option_class->set_value         = grind_string_indenter_option_set_value;
}

static void
grind_string_indenter_option_init (GrindStringIndenterOption *pspec)
{
  
}

GParamSpec *
grind_string_indenter_option_new (const gchar *name,
                                  const gchar *nick,
                                  const gchar *blurb,
                                  const gchar *default_value,
                                  GParamFlags  flags,
                                  gsize        offset)
{
  GParamSpec                 *pspec;
  GrindIndenterOption        *option;
  GrindStringIndenterOption  *internal;
  
  pspec = g_param_spec_internal (grind_string_indenter_option_get_type (),
                                 name, nick, blurb, flags);
  
  pspec->value_type = G_TYPE_STRING;
  
  option = (GrindIndenterOption *) pspec;
  option->field_offset = offset;
  
  internal = (GrindStringIndenterOption *) option;
  internal->default_value = g_strdup (default_value);
  
  return (GParamSpec *) option;
}

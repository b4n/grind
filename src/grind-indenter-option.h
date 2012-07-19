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

#ifndef H_GRIND_INDENTER_OPTION
#define H_GRIND_INDENTER_OPTION

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS


#define GRIND_TYPE_INDENTER_OPTION            (grind_indenter_option_get_type ())
#define GRIND_INDENTER_OPTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_INDENTER_OPTION, GrindIndenterOption))
#define GRIND_INDENTER_OPTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GRIND_TYPE_INDENTER_OPTION, GrindIndenterOptionClass))
#define GRIND_IS_INDENTER_OPTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_INDENTER_OPTION))
#define GRIND_IS_INDENTER_OPTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRIND_TYPE_INDENTER_OPTION))
#define GRIND_INDENTER_OPTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GRIND_TYPE_INDENTER_OPTION, GrindIndenterOptionClass))

typedef struct _GrindIndenterOption       GrindIndenterOption;
typedef struct _GrindIndenterOptionClass  GrindIndenterOptionClass;

struct _GrindIndenterOption
{
  GParamSpec parent;
  
  gsize field_offset;
};

struct _GrindIndenterOptionClass
{
  GParamSpecClass parent;
  
  void    (*set_value)  (GrindIndenterOption *option,
                         gpointer             gobject,
                         const GValue        *value);
  void    (*get_value)  (GrindIndenterOption *option,
                         gpointer             gobject,
                         GValue              *value);
};


GType                 grind_indenter_option_get_type            (void) G_GNUC_CONST;
void                  grind_indenter_option_get_property_impl   (GObject     *object,
                                                                 guint        prop_id,
                                                                 GValue      *value,
                                                                 GParamSpec    *pspec);
void                  grind_indenter_option_set_property_impl   (GObject       *object,
                                                                 guint          prop_id,
                                                                 const GValue  *value,
                                                                 GParamSpec    *pspec);
GrindIndenterOption **grind_indenter_option_list_class_options  (GObjectClass *klass,
                                                                 guint        *n_options_);


#define GRIND_TYPE_CHAR_INDENTER_OPTION     (grind_char_indenter_option_get_type ())
#define GRIND_CHAR_INDENTER_OPTION(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_CHAR_INDENTER_OPTION, GrindCharIndenterOption))
#define GRIND_IS_CHAR_INDENTER_OPTION(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_CHAR_INDENTER_OPTION))

typedef struct _GrindCharIndenterOption GrindCharIndenterOption;
typedef GrindIndenterOptionClass        GrindCharIndenterOptionClass;

struct _GrindCharIndenterOption
{
  GrindIndenterOption parent;
  
  gchar min_value;
  gchar max_value;
  gchar default_value;
};

GType                 grind_char_indenter_option_get_type       (void) G_GNUC_CONST;
GParamSpec           *grind_char_indenter_option_new            (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 gint8        min_value,
                                                                 gint8        max_value,
                                                                 gint8        default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);

#define GRIND_TYPE_UCHAR_INDENTER_OPTION    (grind_uchar_indenter_option_get_type ())
#define GRIND_UCHAR_INDENTER_OPTION(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_UCHAR_INDENTER_OPTION, GrindUCharIndenterOption))
#define GRIND_IS_UCHAR_INDENTER_OPTION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_UCHAR_INDENTER_OPTION))

typedef struct _GrindUCharIndenterOption  GrindUCharIndenterOption;
typedef GrindIndenterOptionClass          GrindUCharIndenterOptionClass;

struct _GrindUCharIndenterOption
{
  GrindIndenterOption parent;
  
  guchar min_value;
  guchar max_value;
  guchar default_value;
};

GType                 grind_uchar_indenter_option_get_type      (void) G_GNUC_CONST;
GParamSpec           *grind_uchar_indenter_option_new           (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 guint8       min_value,
                                                                 guint8       max_value,
                                                                 guint8       default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);

#define GRIND_TYPE_INT_INDENTER_OPTION    (grind_int_indenter_option_get_type ())
#define GRIND_INT_INDENTER_OPTION(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_INT_INDENTER_OPTION, GrindIntIndenterOption))
#define GRIND_IS_INT_INDENTER_OPTION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_INT_INDENTER_OPTION))

typedef struct _GrindIntIndenterOption  GrindIntIndenterOption;
typedef GrindIndenterOptionClass        GrindIntIndenterOptionClass;

struct _GrindIntIndenterOption
{
  GrindIndenterOption parent;
  
  gint min_value;
  gint max_value;
  gint default_value;
};

GType                 grind_int_indenter_option_get_type        (void) G_GNUC_CONST;
GParamSpec           *grind_int_indenter_option_new             (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 gint         min_value,
                                                                 gint         max_value,
                                                                 gint         default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);

#define GRIND_TYPE_UINT_INDENTER_OPTION     (grind_uint_indenter_option_get_type ())
#define GRIND_UINT_INDENTER_OPTION(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_UINT_INDENTER_OPTION, GrindUIntIndenterOption))
#define GRIND_IS_UINT_INDENTER_OPTION(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_UINT_INDENTER_OPTION))

typedef struct _GrindUIntIndenterOption GrindUIntIndenterOption;
typedef GrindIndenterOptionClass        GrindUIntIndenterOptionClass;

struct _GrindUIntIndenterOption
{
  GrindIndenterOption parent;
  
  guint min_value;
  guint max_value;
  guint default_value;
};

GType                 grind_uint_indenter_option_get_type       (void) G_GNUC_CONST;
GParamSpec           *grind_uint_indenter_option_new            (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 guint        min_value,
                                                                 guint        max_value,
                                                                 guint        default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);

#define GRIND_TYPE_INT64_INDENTER_OPTION    (grind_int64_indenter_option_get_type ())
#define GRIND_INT64_INDENTER_OPTION(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_INT64_INDENTER_OPTION, GrindInt64IndenterOption))
#define GRIND_IS_INT64_INDENTER_OPTION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_INT64_INDENTER_OPTION))

typedef struct _GrindInt64IndenterOption  GrindInt64IndenterOption;
typedef GrindIndenterOptionClass          GrindInt64IndenterOptionClass;

struct _GrindInt64IndenterOption
{
  GrindIndenterOption parent;
  
  gint64 min_value;
  gint64 max_value;
  gint64 default_value;
};

GType                 grind_int64_indenter_option_get_type      (void) G_GNUC_CONST;
GParamSpec           *grind_int64_indenter_option_new           (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 gint64       min_value,
                                                                 gint64       max_value,
                                                                 gint64       default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);

#define GRIND_TYPE_UINT64_INDENTER_OPTION     (grind_uint64_indenter_option_get_type ())
#define GRIND_UINT64_INDENTER_OPTION(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_UINT64_INDENTER_OPTION, GrindUInt64IndenterOption))
#define GRIND_IS_UINT64_INDENTER_OPTION(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_UINT64_INDENTER_OPTION))

typedef struct _GrindUInt64IndenterOption  GrindUInt64IndenterOption;
typedef GrindIndenterOptionClass            GrindUInt64IndenterOptionClass;

struct _GrindUInt64IndenterOption
{
  GrindIndenterOption parent;
  
  guint64 min_value;
  guint64 max_value;
  guint64 default_value;
};

GType                 grind_uint64_indenter_option_get_type     (void) G_GNUC_CONST;
GParamSpec           *grind_uint64_indenter_option_new          (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 guint64      min_value,
                                                                 guint64      max_value,
                                                                 guint64      default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);

#define GRIND_TYPE_LONG_INDENTER_OPTION     (grind_long_indenter_option_get_type ())
#define GRIND_LONG_INDENTER_OPTION(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_LONG_INDENTER_OPTION, GrindLongIndenterOption))
#define GRIND_IS_LONG_INDENTER_OPTION(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_LONG_INDENTER_OPTION))

typedef struct _GrindLongIndenterOption GrindLongIndenterOption;
typedef GrindIndenterOptionClass        GrindLongIndenterOptionClass;

struct _GrindLongIndenterOption
{
  GrindIndenterOption parent;
  
  glong min_value;
  glong max_value;
  glong default_value;
};

GType                 grind_long_indenter_option_get_type       (void) G_GNUC_CONST;
GParamSpec           *grind_long_indenter_option_new            (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 glong        min_value,
                                                                 glong        max_value,
                                                                 glong        default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);

#define GRIND_TYPE_ULONG_INDENTER_OPTION    (grind_ulong_indenter_option_get_type ())
#define GRIND_ULONG_INDENTER_OPTION(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_ULONG_INDENTER_OPTION, GrindULongIndenterOption))
#define GRIND_IS_ULONG_INDENTER_OPTION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_ULONG_INDENTER_OPTION))

typedef struct _GrindULongIndenterOption  GrindULongIndenterOption;
typedef GrindIndenterOptionClass          GrindULongIndenterOptionClass;

struct _GrindULongIndenterOption
{
  GrindIndenterOption parent;
  
  gulong min_value;
  gulong max_value;
  gulong default_value;
};

GType                 grind_ulong_indenter_option_get_type      (void) G_GNUC_CONST;
GParamSpec           *grind_ulong_indenter_option_new           (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 gulong       min_value,
                                                                 gulong       max_value,
                                                                 gulong       default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);

#define GRIND_TYPE_FLOAT_INDENTER_OPTION    (grind_float_indenter_option_get_type ())
#define GRIND_FLOAT_INDENTER_OPTION(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_FLOAT_INDENTER_OPTION, GrindFloatIndenterOption))
#define GRIND_IS_FLOAT_INDENTER_OPTION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_FLOAT_INDENTER_OPTION))

typedef struct _GrindFloatIndenterOption  GrindFloatIndenterOption;
typedef GrindIndenterOptionClass          GrindFloatIndenterOptionClass;

struct _GrindFloatIndenterOption
{
  GrindIndenterOption parent;
  
  gfloat min_value;
  gfloat max_value;
  gfloat default_value;
};

GType                 grind_float_indenter_option_get_type      (void) G_GNUC_CONST;
GParamSpec           *grind_float_indenter_option_new           (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 gfloat       min_value,
                                                                 gfloat       max_value,
                                                                 gfloat       default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);

#define GRIND_TYPE_DOUBLE_INDENTER_OPTION     (grind_double_indenter_option_get_type ())
#define GRIND_DOUBLE_INDENTER_OPTION(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_DOUBLE_INDENTER_OPTION, GrindDoubleIndenterOption))
#define GRIND_IS_DOUBLE_INDENTER_OPTION(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_DOUBLE_INDENTER_OPTION))

typedef struct _GrindDoubleIndenterOption GrindDoubleIndenterOption;
typedef GrindIndenterOptionClass          GrindDoubleIndenterOptionClass;

struct _GrindDoubleIndenterOption
{
  GrindIndenterOption parent;
  
  gdouble min_value;
  gdouble max_value;
  gdouble default_value;
};

GType                 grind_double_indenter_option_get_type     (void) G_GNUC_CONST;
GParamSpec           *grind_double_indenter_option_new          (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 gdouble      min_value,
                                                                 gdouble      max_value,
                                                                 gdouble      default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);

#define GRIND_TYPE_BOOLEAN_INDENTER_OPTION    (grind_boolean_indenter_option_get_type ())
#define GRIND_BOOLEAN_INDENTER_OPTION(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_BOOLEAN_INDENTER_OPTION, GrindBooleanIndenterOption))
#define GRIND_IS_BOOLEAN_INDENTER_OPTION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_BOOLEAN_INDENTER_OPTION))

typedef struct _GrindBooleanIndenterOption  GrindBooleanIndenterOption;
typedef GrindIndenterOptionClass            GrindBooleanIndenterOptionClass;

struct _GrindBooleanIndenterOption
{
  GrindIndenterOption parent;
  
  gboolean default_value;
};

GType                 grind_boolean_indenter_option_get_type    (void) G_GNUC_CONST;
GParamSpec           *grind_boolean_indenter_option_new         (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 gboolean     default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);

#define GRIND_TYPE_ENUM_INDENTER_OPTION     (grind_enum_indenter_option_get_type ())
#define GRIND_ENUM_INDENTER_OPTION(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_ENUM_INDENTER_OPTION, GrindEnumIndenterOption))
#define GRIND_IS_ENUM_INDENTER_OPTION(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_ENUM_INDENTER_OPTION))

typedef struct _GrindEnumIndenterOption GrindEnumIndenterOption;
typedef GrindIndenterOptionClass        GrindEnumIndenterOptionClass;

struct _GrindEnumIndenterOption
{
  GrindIndenterOption parent;
  
  GEnumClass *enum_class;
  gint        default_value;
};

GType                 grind_enum_indenter_option_get_type       (void) G_GNUC_CONST;
GParamSpec           *grind_enum_indenter_option_new            (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 GType        enum_type,
                                                                 gint         default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);

#define GRIND_TYPE_STRING_INDENTER_OPTION    (grind_string_indenter_option_get_type ())
#define GRIND_STRING_INDENTER_OPTION(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_STRING_INDENTER_OPTION, GrindStringIndenterOption))
#define GRIND_IS_STRING_INDENTER_OPTION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_STRING_INDENTER_OPTION))

typedef struct _GrindStringIndenterOption GrindStringIndenterOption;
typedef GrindIndenterOptionClass          GrindStringIndenterOptionClass;

struct _GrindStringIndenterOption
{
  GrindIndenterOption parent;
  
  gchar *default_value;
};

GType                 grind_string_indenter_option_get_type     (void) G_GNUC_CONST;
GParamSpec           *grind_string_indenter_option_new          (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 const gchar *default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);


G_END_DECLS

#endif /* guard */

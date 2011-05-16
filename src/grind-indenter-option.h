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

GParamSpec           *grind_char_indenter_option_new            (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 gint8        min_value,
                                                                 gint8        max_value,
                                                                 gint8        default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);
GParamSpec           *grind_uchar_indenter_option_new           (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 guint8       min_value,
                                                                 guint8       max_value,
                                                                 guint8       default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);
GParamSpec           *grind_int_indenter_option_new             (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 gint         min_value,
                                                                 gint         max_value,
                                                                 gint         default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);
GParamSpec           *grind_uint_indenter_option_new            (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 guint        min_value,
                                                                 guint        max_value,
                                                                 guint        default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);
GParamSpec           *grind_int64_indenter_option_new           (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 gint64       min_value,
                                                                 gint64       max_value,
                                                                 gint64       default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);
GParamSpec           *grind_uint64_indenter_option_new          (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 guint64      min_value,
                                                                 guint64      max_value,
                                                                 guint64      default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);
GParamSpec           *grind_long_indenter_option_new            (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 glong        min_value,
                                                                 glong        max_value,
                                                                 glong        default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);
GParamSpec           *grind_ulong_indenter_option_new           (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 gulong       min_value,
                                                                 gulong       max_value,
                                                                 gulong       default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);
GParamSpec           *grind_float_indenter_option_new           (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 gfloat       min_value,
                                                                 gfloat       max_value,
                                                                 gfloat       default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);
GParamSpec           *grind_double_indenter_option_new          (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 gdouble      min_value,
                                                                 gdouble      max_value,
                                                                 gdouble      default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);
GParamSpec           *grind_boolean_indenter_option_new         (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 gboolean     default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);
GParamSpec           *grind_enum_indenter_option_new            (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 GType        enum_type,
                                                                 gint         default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);
GParamSpec           *grind_string_indenter_option_new          (const gchar *name,
                                                                 const gchar *nick,
                                                                 const gchar *blurb,
                                                                 const gchar *default_value,
                                                                 GParamFlags  flags,
                                                                 gsize        offset);


G_END_DECLS

#endif /* guard */

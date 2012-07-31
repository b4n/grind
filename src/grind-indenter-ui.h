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

#ifndef H_GRIND_INDENTER_UI
#define H_GRIND_INDENTER_UI

#include <glib.h>
#include <gtk/gtk.h>

#include "grind-indenter.h"

G_BEGIN_DECLS


#define GRIND_TYPE_INDENTER_UI            (grind_indenter_ui_get_type ())
#define GRIND_INDENTER_UI(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_INDENTER_UI, GrindIndenterUi))
#define GRIND_INDENTER_UI_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GRIND_TYPE_INDENTER_UI, GrindIndenterUiClass))
#define GRIND_IS_INDENTER_UI(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_INDENTER_UI))
#define GRIND_IS_INDENTER_UI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRIND_TYPE_INDENTER_UI))
#define GRIND_INDENTER_UI_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GRIND_TYPE_INDENTER_UI, GrindIndenterUiClass))

typedef struct _GrindIndenterUi         GrindIndenterUi;
typedef struct _GrindIndenterUiClass    GrindIndenterUiClass;
typedef struct _GrindIndenterUiPrivate  GrindIndenterUiPrivate;

struct _GrindIndenterUi
{
  GtkVBox parent;
  /*< private >*/
  GrindIndenterUiPrivate *priv;
};

struct _GrindIndenterUiClass
{
  GtkVBoxClass parent_class;
};


GType           grind_indenter_ui_get_type            (void) G_GNUC_CONST;
GtkWidget      *grind_indenter_ui_new                 (GrindIndenter *indenter);
GrindIndenter  *grind_indenter_ui_get_indenter        (GrindIndenterUi *self);
void            grind_indenter_ui_set_indenter        (GrindIndenterUi *self,
                                                       GrindIndenter   *indenter);
gchar          *grind_indenter_ui_get_preview_text    (GrindIndenterUi *self);
void            grind_indenter_ui_set_preview_text    (GrindIndenterUi *self,
                                                       const gchar     *text,
                                                       gssize           length);


G_END_DECLS

#endif /* guard */

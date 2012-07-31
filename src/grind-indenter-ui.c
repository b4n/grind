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

#include "grind-indenter-ui.h"

#include <glib.h>
#include <gtk/gtk.h>

#include "grind-indenter.h"
#include "grind-indenter-option.h"


#define GRIND_INDENTER_UI_DEFAULT_PREVIEW_TEXT                                 \
"#include <stdlib.h>\n"                                                        \
"\n"                                                                           \
"struct a_struct_s {\n"                                                        \
"  int          member1;\n"                                                    \
"  const char  *member2;\n"                                                    \
"  void      *(*delegate) (int);\n"                                            \
"};\n"                                                                         \
"\n"                                                                           \
"struct a_struct_s *\n"                                                        \
"constructor (void)\n"                                                         \
"{\n"                                                                          \
"  struct a_struct_s *self;\n"                                                 \
"  \n"                                                                         \
"  self = malloc (sizeof *self);\n"                                            \
"  if (self) {\n"                                                              \
"    self->member1   = 42;\n"                                                  \
"    self->member2   = \"42\";\n"                                              \
"    self->delegate  = NULL;\n"                                                \
"  }\n"                                                                        \
"  \n"                                                                         \
"  return self;\n"                                                             \
"}\n"                                                                          \
"\n"


struct _GrindIndenterUiPrivate
{
  GrindIndenter  *indenter;
  
  GtkWidget      *prop_box;
  GtkTextBuffer  *preview_buffer;
  GtkSizeGroup   *options_label_size_group;
};


enum
{
  PROP_0,
  
  PROP_INDENTER,
  PROP_PREVIEW_TEXT
};


static void     grind_indenter_ui_get_property  (GObject     *object,
                                                 guint        prop_id,
                                                 GValue      *value,
                                                 GParamSpec  *pspec);
static void     grind_indenter_ui_set_property  (GObject       *object,
                                                 guint          prop_id,
                                                 const GValue  *value,
                                                 GParamSpec    *pspec);
static void     grind_indenter_ui_finalize      (GObject *object);


G_DEFINE_TYPE (GrindIndenterUi, grind_indenter_ui, GTK_TYPE_VBOX)


static void
grind_indenter_ui_class_init (GrindIndenterUiClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  
  object_class->get_property  = grind_indenter_ui_get_property;
  object_class->set_property  = grind_indenter_ui_set_property;
  object_class->finalize      = grind_indenter_ui_finalize;
  
  g_object_class_install_property (object_class, PROP_INDENTER,
                                   g_param_spec_object ("indenter",
                                                        "Indenter",
                                                        "The indenter to configure",
                                                        GRIND_TYPE_INDENTER,
                                                        G_PARAM_STATIC_NAME |
                                                        G_PARAM_STATIC_NICK |
                                                        G_PARAM_STATIC_BLURB |
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (object_class, PROP_PREVIEW_TEXT,
                                   g_param_spec_string ("preview-text",
                                                        "Preview text",
                                                        "The text for previewing",
                                                        GRIND_INDENTER_UI_DEFAULT_PREVIEW_TEXT,
                                                        G_PARAM_STATIC_NAME |
                                                        G_PARAM_STATIC_NICK |
                                                        G_PARAM_STATIC_BLURB |
                                                        G_PARAM_READWRITE));
  
  g_type_class_add_private (klass, sizeof (GrindIndenterUiPrivate));
}

static GtkWidget *
create_frame (const gchar *label,
              GtkWidget   *child)
{
  GtkWidget      *frame;
  GtkWidget      *alignment;
  GtkWidget      *frame_label;
  PangoAttrList  *label_attrs;
  
  frame_label = gtk_label_new (label);
  label_attrs = pango_attr_list_new ();
  pango_attr_list_insert (label_attrs,
                          pango_attr_weight_new (PANGO_WEIGHT_BOLD));
  gtk_label_set_attributes (GTK_LABEL (frame_label), label_attrs);
  pango_attr_list_unref (label_attrs);
  
  alignment = g_object_new (GTK_TYPE_ALIGNMENT,
                            "left-padding", 12,
                            "child", child,
                            NULL);
  
  frame = g_object_new (GTK_TYPE_FRAME,
                        "shadow-type", GTK_SHADOW_NONE,
                        "label-widget", frame_label,
                        "child", alignment,
                        NULL);
  
  return frame;
}

static void
grind_indenter_ui_init (GrindIndenterUi *self)
{
  GtkWidget      *paned;
  GtkWidget      *preview;
  GtkWidget      *preview_scroll;
  GtkWidget      *prop_box_scroll;
  PangoFontDescription *font_desc;
  
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GRIND_TYPE_INDENTER_UI,
                                            GrindIndenterUiPrivate);
  
  self->priv->indenter = NULL;
  
  self->priv->options_label_size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
  
  self->priv->preview_buffer = gtk_text_buffer_new (NULL);
  gtk_text_buffer_set_text (self->priv->preview_buffer,
                            GRIND_INDENTER_UI_DEFAULT_PREVIEW_TEXT, -1);
  
  self->priv->prop_box = gtk_vbox_new (FALSE, 0);
  prop_box_scroll = g_object_new (GTK_TYPE_SCROLLED_WINDOW,
                                  "hscrollbar-policy", GTK_POLICY_AUTOMATIC,
                                  "vscrollbar-policy", GTK_POLICY_AUTOMATIC,
                                  "shadow-type", GTK_SHADOW_NONE,
                                  NULL);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (prop_box_scroll),
                                         self->priv->prop_box);
  
  /* FIXME: indenters need a GeanyDocument, FUUUUU */
  preview = gtk_text_view_new_with_buffer (self->priv->preview_buffer);
  preview_scroll = g_object_new (GTK_TYPE_SCROLLED_WINDOW,
                                 "hscrollbar-policy", GTK_POLICY_AUTOMATIC,
                                 "vscrollbar-policy", GTK_POLICY_AUTOMATIC,
                                 "shadow-type", GTK_SHADOW_IN,
                                 NULL);
  font_desc = pango_font_description_from_string ("monospace");
  gtk_widget_modify_font (preview, font_desc);
  pango_font_description_free (font_desc);
  gtk_container_add (GTK_CONTAINER (preview_scroll), preview);
  
  paned = gtk_vpaned_new ();
  gtk_paned_pack1 (GTK_PANED (paned),
                   create_frame (_("Options"), prop_box_scroll), TRUE, FALSE);
  gtk_paned_pack2 (GTK_PANED (paned),
                   create_frame (_("Preview"), preview_scroll), FALSE, FALSE);
  
  gtk_box_set_spacing (GTK_BOX (self), 6);
  gtk_box_pack_start (GTK_BOX (self), paned, TRUE, TRUE, 0);
}

static void
grind_indenter_ui_finalize (GObject *object)
{
  GrindIndenterUi *self = GRIND_INDENTER_UI (object);
  
  if (self->priv->indenter) {
    g_object_unref (self->priv->indenter);
    self->priv->indenter = NULL;
  }
  
  G_OBJECT_CLASS (grind_indenter_ui_parent_class)->finalize (object);
}

static void
grind_indenter_ui_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GrindIndenterUi *self = GRIND_INDENTER_UI (object);
  
  switch (prop_id) {
    case PROP_INDENTER:
      g_value_set_object (value, self->priv->indenter);
      break;
    
    case PROP_PREVIEW_TEXT:
      g_value_take_string (value, grind_indenter_ui_get_preview_text (self));
      break;
    
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
grind_indenter_ui_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GrindIndenterUi *self = GRIND_INDENTER_UI (object);
  
  switch (prop_id) {
    case PROP_INDENTER:
      grind_indenter_ui_set_indenter (self, g_value_get_object (value));
      break;
    
    case PROP_PREVIEW_TEXT:
      grind_indenter_ui_set_preview_text (self, g_value_get_string (value), -1);
      break;
    
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}



/**
 * grind_indenter_ui_new:
 * @indenter: The #GrindIndenter to display or %NULL
 * 
 * Creates a new widget for configuring options of a #GrindIndenter
 * 
 * Returns: The new widget
 */
GtkWidget *
grind_indenter_ui_new (GrindIndenter *indenter)
{
  return g_object_new (GRIND_TYPE_INDENTER_UI,
                       "indenter", indenter,
                       NULL);
}

/**
 * grind_indenter_ui_get_indenter:
 * @self: A #GrindIndenterUi
 * 
 * Gets the #GrindIndenter object displayed by a #GrindIndenterUi.
 * 
 * Returns: The current #GrindIndenter or %NULL if none
 */
GrindIndenter *
grind_indenter_ui_get_indenter (GrindIndenterUi *self)
{
  g_return_val_if_fail (GRIND_IS_INDENTER_UI (self), NULL);
  
  return self->priv->indenter;
}

static void
on_int_changed (GtkRange         *range,
                GrindIndenterUi  *self)
{
  GParamSpec *pspec = g_object_get_data (G_OBJECT (range), "pspec");
  gdouble     val   = gtk_range_get_value (range);
  
  /*g_debug ("new value of %s: %g", g_param_spec_get_name (pspec), val);*/
  g_object_set (self->priv->indenter,
                g_param_spec_get_name (pspec), (gint) val,
                NULL);
}

static GtkWidget *
create_widget_for_int_option (GrindIndenterUi              *self,
                              const GrindIntIndenterOption *option)
{
#if 1
  GtkWidget *box;
  GtkWidget *label;
  GtkWidget *range;
  gint       val;
  gchar     *label_text;
  
  box = gtk_hbox_new (FALSE, 6);
  label_text = g_strdup_printf ("%s:",
                                g_param_spec_get_nick (G_PARAM_SPEC (option)));
  label = gtk_label_new (label_text);
  g_free (label_text);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  range = gtk_hscale_new_with_range (option->min_value, option->max_value, 1);
  gtk_scale_set_digits (GTK_SCALE (range), 0);
  gtk_scale_set_draw_value (GTK_SCALE (range), TRUE);
  gtk_scale_set_value_pos (GTK_SCALE (range), GTK_POS_LEFT);
#if 0
  gtk_scale_add_mark (GTK_SCALE (range), option->default_value,
                      GTK_POS_BOTTOM, /*"default value"*/NULL);
#endif
  
  g_object_get (self->priv->indenter,
                g_param_spec_get_name (G_PARAM_SPEC (option)), &val,
                NULL);
  gtk_range_set_value (GTK_RANGE (range), val);
  
  gtk_box_pack_start (GTK_BOX (box), label, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (box), range, TRUE, TRUE, 0);
  
  gtk_size_group_add_widget (self->priv->options_label_size_group, label);
  
  g_object_set_data_full (G_OBJECT (range), "pspec",
                          g_param_spec_ref (G_PARAM_SPEC (option)),
                          (GDestroyNotify) g_param_spec_unref);
  g_signal_connect (range, "value-changed", G_CALLBACK (on_int_changed), self);
  
  return box;
#else
  GtkWidget *box;
  GtkWidget *label;
  GtkWidget *range;
  gint       val;
  gchar     *label_text;
  
  box = gtk_hbox_new (FALSE, 6);
  label_text = g_strdup_printf ("%s:",
                                g_param_spec_get_nick (G_PARAM_SPEC (option)));
  label = gtk_label_new (label_text);
  g_free (label_text);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  range = gtk_spin_button_new_with_range (option->min_value, option->max_value,
                                          1);
  gtk_spin_button_set_digits (GTK_SPIN_BUTTON (range), 0);
  
  g_object_get (self->priv->indenter,
                g_param_spec_get_name (G_PARAM_SPEC (option)), &val,
                NULL);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (range), val);
  
  gtk_box_pack_start (GTK_BOX (box), label, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (box), range, TRUE, TRUE, 0);
  
  gtk_size_group_add_widget (self->priv->options_label_size_group, label);
  
  return box;
#endif
}

static void
on_enum_changed (GtkComboBox     *combo,
                 GrindIndenterUi *self)
{
  GParamSpec *pspec = g_object_get_data (G_OBJECT (combo), "pspec");
  GtkTreeIter iter;
  
  if (gtk_combo_box_get_active_iter (combo, &iter)) {
    GtkTreeModel *model = gtk_combo_box_get_model (combo);
    gint          val;
    
    gtk_tree_model_get (model, &iter, 0, &val, -1);
    
    g_object_set (self->priv->indenter,
                  g_param_spec_get_name (pspec), val,
                  NULL);
  }
}

static GtkWidget *
create_widget_for_enum_option (GrindIndenterUi               *self,
                               const GrindEnumIndenterOption *option)
{
  GtkWidget        *box;
  GtkWidget        *label;
  GtkWidget        *combo;
  GtkCellRenderer  *cell;
  GtkListStore     *store;
  guint             i;
  gint              val;
  gchar            *label_text;
  
  box = gtk_hbox_new (FALSE, 6);
  label_text = g_strdup_printf ("%s:",
                                g_param_spec_get_nick (G_PARAM_SPEC (option)));
  label = gtk_label_new (label_text);
  g_free (label_text);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  
  g_object_get (self->priv->indenter,
                g_param_spec_get_name (G_PARAM_SPEC (option)), &val,
                NULL);
  store = gtk_list_store_new (3, G_TYPE_INT, G_TYPE_STRING, PANGO_TYPE_WEIGHT);
  combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (store));
  for (i = 0; i < option->enum_class->n_values; i++) {
    GtkTreeIter iter;
    PangoWeight weight;
    
    if (option->enum_class->values[i].value == option->default_value) {
      weight = PANGO_WEIGHT_BOLD;
    } else {
      weight = PANGO_WEIGHT_NORMAL;
    }
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                        0, option->enum_class->values[i].value,
                        1, option->enum_class->values[i].value_nick,
                        2, weight,
                        -1);
    if (option->enum_class->values[i].value == val) {
      gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo), &iter);
    }
  }
  cell = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), cell, TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), cell,
                                  "text", 1,
                                  "weight", 2,
                                  NULL);
  
  gtk_box_pack_start (GTK_BOX (box), label, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (box), combo, TRUE, TRUE, 0);
  
  gtk_size_group_add_widget (self->priv->options_label_size_group, label);
  
  g_object_set_data_full (G_OBJECT (combo), "pspec",
                          g_param_spec_ref (G_PARAM_SPEC (option)),
                          (GDestroyNotify) g_param_spec_unref);
  g_signal_connect (combo, "changed", G_CALLBACK (on_enum_changed), self);
  
  return box;
}

static void
on_boolean_notify (GrindIndenter   *indenter,
                   GParamSpec      *pspec,
                   GtkToggleButton *toggle)
{
  gboolean state;
  
  g_object_get (indenter,
                g_param_spec_get_name (pspec), &state,
                NULL);
  gtk_toggle_button_set_active (toggle, state);
}

static void
on_boolean_toggled (GtkToggleButton *toggle,
                    GrindIndenterUi *self)
{
  GParamSpec *pspec = g_object_get_data (G_OBJECT (toggle), "pspec");
  
  g_object_set (self->priv->indenter,
                g_param_spec_get_name (pspec),
                gtk_toggle_button_get_active (toggle),
                NULL);
}

static GtkWidget *
create_widget_for_boolean_option (GrindIndenterUi                  *self,
                                  const GrindBooleanIndenterOption *option)
{
  GtkWidget    *check;
  gboolean      val;
  const gchar  *prop_name;
  gchar        *notify_name;
  
  prop_name = g_param_spec_get_name (G_PARAM_SPEC (option));
  g_object_get (self->priv->indenter, prop_name, &val, NULL);
  check = gtk_check_button_new_with_label (g_param_spec_get_nick (G_PARAM_SPEC (option)));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check), val);
  
  /* FIXME: disconnect this signal when changing indenter */
  /*notify_name = g_strconcat ("notify::", prop_name, NULL);
  g_signal_connect (self->priv->indenter, notify_name,
                    G_CALLBACK (on_boolean_notify), check);
  g_free (notify_name);*/
  g_object_set_data_full (G_OBJECT (check), "pspec",
                          g_param_spec_ref (G_PARAM_SPEC (option)),
                          (GDestroyNotify) g_param_spec_unref);
  g_signal_connect (check, "toggled", G_CALLBACK (on_boolean_toggled), self);
  
  return check;
}

static GtkWidget *
create_widget_for_option (GrindIndenterUi           *self,
                          const GrindIndenterOption *option)
{
  GtkWidget *widget = NULL;
  
  switch (G_TYPE_FUNDAMENTAL (G_PARAM_SPEC (option)->value_type)) {
    case G_TYPE_INT:
      widget = create_widget_for_int_option (self,
                                             GRIND_INT_INDENTER_OPTION (option));
      break;
    
    case G_TYPE_BOOLEAN:
      widget = create_widget_for_boolean_option (self,
                                                 GRIND_BOOLEAN_INDENTER_OPTION (option));
      break;
    
    case G_TYPE_ENUM:
      widget = create_widget_for_enum_option (self,
                                              GRIND_ENUM_INDENTER_OPTION (option));
      break;
    
    /* ... */
    
    default:
      g_critical ("No widget for option %s of type %s",
                  g_param_spec_get_name (G_PARAM_SPEC (option)),
                  g_type_name (G_PARAM_SPEC (option)->value_type));
      break;
  }
  gtk_widget_set_tooltip_text (widget,
                               g_param_spec_get_blurb (G_PARAM_SPEC (option)));
  
  return widget;
}

static void
rebuild_option_widgets (GrindIndenterUi *self)
{
  gtk_container_foreach (GTK_CONTAINER (self->priv->prop_box),
                         (GtkCallback) gtk_widget_destroy, NULL);
  
  if (self->priv->indenter) {
    GrindIndenterOption **options;
    guint                 n_options;
    guint                 i;
    
    options = grind_indenter_option_list_class_options (G_OBJECT_GET_CLASS (self->priv->indenter),
                                                        &n_options);
    if (n_options > 0) {
      for (i = 0; i < n_options; i++) {
        gtk_box_pack_start (GTK_BOX (self->priv->prop_box),
                            create_widget_for_option (self, options[i]),
                            FALSE, TRUE, 0);
      }
    } else {
      GtkWidget *label;
      
      label = gtk_label_new (_("This indenter has no configurable options"));
      gtk_widget_set_sensitive (label, FALSE);
      gtk_box_pack_start (GTK_BOX (self->priv->prop_box), label,
                          TRUE, TRUE, 0);
    }
    g_free (options);
    
    gtk_widget_show_all (self->priv->prop_box);
  }
}

/**
 * grind_indenter_ui_set_indenter:
 * @self: A #GrindIndenterUi
 * @indenter: The #GrindIndenter to display, or %NULL
 * 
 * Changes the #GrindIndenter being edited.
 */
void
grind_indenter_ui_set_indenter (GrindIndenterUi *self,
                                GrindIndenter   *indenter)
{
  g_return_if_fail (GRIND_IS_INDENTER_UI (self));
  g_return_if_fail (! indenter || GRIND_IS_INDENTER (indenter));
  
  if (self->priv->indenter != indenter) {
    if (self->priv->indenter) {
      g_object_unref (self->priv->indenter);
    }
    self->priv->indenter = indenter ? g_object_ref (indenter) : NULL;
    
    /* FIXME: update more stuff here? */
    rebuild_option_widgets (self);
    
    g_object_notify (G_OBJECT (self), "indenter");
  }
}

/**
 * grind_indenter_ui_get_preview_text:
 * @self: A #GrindIndenterUi
 * 
 * Gets the preview text.
 * 
 * Returns: The preview text
 */
gchar *
grind_indenter_ui_get_preview_text (GrindIndenterUi *self)
{
  GtkTextIter start;
  GtkTextIter end;
  gchar      *text;
  
  g_return_val_if_fail (GRIND_IS_INDENTER_UI (self), NULL);
  
  gtk_text_buffer_get_start_iter (self->priv->preview_buffer, &start);
  gtk_text_buffer_get_end_iter (self->priv->preview_buffer, &end);
  text = gtk_text_buffer_get_text (self->priv->preview_buffer,
                                   &start, &end, FALSE);
  
  return text;
}

/**
 * grind_indenter_ui_set_preview_text:
 * @self: A #GrindIndenterUi
 * @text: The new preview text
 * @length: The length of @text or %NULL
 * 
 * Sets the preview text of a #GrindIndenterUi.
 */
void
grind_indenter_ui_set_preview_text (GrindIndenterUi *self,
                                    const gchar     *text,
                                    gssize           length)
{
  g_return_if_fail (GRIND_IS_INDENTER_UI (self));
  
  gtk_text_buffer_set_text (self->priv->preview_buffer, text, length);
  g_object_notify (G_OBJECT (self), "preview-text");
}

/*
 *  Copyright 2011-2012 Colomban Wendling  <ban@herbesfolles.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "grind-plugin.h"

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "geanyplugin.h"
#include "document.h"

#include "grind-indenter.h"
#include "grind-indenter-ui.h"
#include "grind-backend-manager.h"


GeanyPlugin    *geany_plugin;
GeanyData      *geany_data;
GeanyFunctions *geany_functions;


PLUGIN_VERSION_CHECK (206)
PLUGIN_SET_INFO ("GRInd",
                 "Re-indents source code",
                 "0.1",
                 "Colomban Wendling <ban@herbesfolles.org>")


static GSList* G_destroy_widget_stack = NULL;


static void
reindent (GeanyDocument  *doc,
          GrindIndenter  *indenter)
{
  if (DOC_VALID (doc)) {
    int               start;
    int               end;
    ScintillaObject  *sci = doc->editor->sci;
    
    if (sci_has_selection (sci)) {
      start = sci_get_line_from_position (sci, sci_get_selection_start (sci));
      end = sci_get_line_from_position (sci, sci_get_selection_end (sci));
    } else {
      start = 0;
      end = sci_get_line_count (sci);
    }
    
    if (start != end) {
      g_debug ("Using indenter \"%s\" by \"%s\"",
               grind_indenter_get_name (indenter),
               grind_indenter_get_author (indenter));
      
      sci_start_undo_action (sci);
      if (grind_indenter_indent (indenter, doc,
                                 sci_get_position_from_line (sci, start),
                                 sci_get_line_end_position (sci, end))) {
        msgwin_status_add ("Reindented \"%s\"", DOC_FILENAME (doc));
      }
      sci_end_undo_action (sci);
    }
  }
}


static void
reindent_activate_handler (GtkActivatable *activatable,
                           gpointer        data)
{
  GrindBackendManager  *manager;
  GrindIndenter        *indenter;
  
  manager = grind_backend_manager_get_default ();
  indenter = grind_backend_manager_get_backend (manager, NULL);
  if (indenter) {
    reindent (document_get_current (), indenter);
    g_object_unref (indenter);
  } else {
    g_warning ("No indenter available");
  }
}

void
plugin_init (GeanyData *data)
{
  GtkWidget *item;
  
  /* we register GTypes, we can't unload them */
  plugin_module_make_resident (geany_plugin);
  
  item = gtk_menu_item_new_with_label ("Re-indent the selection");
  g_signal_connect (item, "activate",
                    G_CALLBACK (reindent_activate_handler), NULL);
  gtk_menu_append (GTK_MENU (geany_data->main_widgets->tools_menu), item);
  G_destroy_widget_stack = g_slist_append (G_destroy_widget_stack, item);
  gtk_widget_show (item);
}

void
plugin_cleanup (void)
{
  g_slist_free_full (G_destroy_widget_stack,
                     (GDestroyNotify)gtk_widget_destroy);
}

enum {
  CONF_COL_BACKEND,
  CONF_COL_NAME,
  CONF_COL_CONF,
  CONF_N_COLS
};

static void
on_configure_response (GtkDialog     *dialog,
                       gint           response_id,
                       GtkTreeModel  *model)
{
  GtkTreeIter iter;
  
  /* save or restore each indenter's settings */
  if (gtk_tree_model_get_iter_first (model, &iter)) {
    do {
      GrindIndenter  *indenter;
      GKeyFile       *conf;
      GError         *err = NULL;
      
      gtk_tree_model_get (model, &iter,
                          CONF_COL_BACKEND, &indenter,
                          CONF_COL_CONF, &conf,
                          -1);
      
      switch (response_id) {
        case GTK_RESPONSE_ACCEPT:
        case GTK_RESPONSE_APPLY:
        case GTK_RESPONSE_OK:
        case GTK_RESPONSE_YES:
          /* update saved configuration so if the user later clicks "cancel"
           * it won't revert the applying */
          /* FIXME: only do that upon apply? */
          if (! grind_indenter_sync_options_to_keyfile (indenter, conf, &err)) {
            g_warning ("failed to save backend configuration: %s",
                       err->message);
            g_error_free (err);
          }
          break;
        
        default:
          if (! grind_indenter_sync_options_from_keyfile (indenter, conf,
                                                          &err)) {
            g_warning ("failed to restore backend configuration: %s",
                       err->message);
            g_error_free (err);
          }
          break;
      }
      
      g_object_unref (indenter);
    } while (gtk_tree_model_iter_next (model, &iter));
  }
}

static void
on_configure_backend_changed (GtkComboBox     *combo,
                              GrindIndenterUi *ui)
{
  GtkTreeIter iter;
  
  if (gtk_combo_box_get_active_iter (combo, &iter)) {
    GtkTreeModel   *model = gtk_combo_box_get_model (combo);
    GrindIndenter  *indenter;
    
    gtk_tree_model_get (model, &iter, CONF_COL_BACKEND, &indenter, -1);
    grind_indenter_ui_set_indenter (ui, indenter);
    g_object_unref (indenter);
  }
}

/* free combo's model elements that won't be destroyed automatically
 * (GKeyFile) */
static void
on_combo_destroy (GtkComboBox  *combo,
                  gpointer      dummy)
{
  GtkTreeModel *model = gtk_combo_box_get_model (combo);
  GtkTreeIter iter;
  
  if (gtk_tree_model_get_iter_first (model, &iter)) {
    do {
      GKeyFile *conf;
      
      gtk_tree_model_get (model, &iter, CONF_COL_CONF, &conf, -1);
      g_key_file_free (conf);
    } while (gtk_tree_model_iter_next (model, &iter));
  }
}

GtkWidget *
plugin_configure (GtkDialog *dialog)
{
  GtkWidget        *box = NULL;
  GtkListStore     *store;
  GtkWidget        *combo;
  GtkCellRenderer  *cell;
  GtkWidget        *indenter_ui;
  GrindIndenter   **backends;
  guint             n_backends;
  
  backends = grind_backend_manager_list_backends (grind_backend_manager_get_default (),
                                                  &n_backends);
  if (n_backends > 0) {
    guint i;
  
    box = gtk_vbox_new (FALSE, 0);
    
    store = gtk_list_store_new (CONF_N_COLS,
                                GRIND_TYPE_INDENTER,
                                G_TYPE_STRING,
                                G_TYPE_POINTER);
    combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (store));
    for (i = 0; i < n_backends; i++) {
      GtkTreeIter iter;
      GKeyFile   *conf = g_key_file_new ();
      GError     *err = NULL;
      
      if (! grind_indenter_sync_options_to_keyfile (backends[i], conf, &err)) {
        g_warning ("failed to backup backend configuration: %s", err->message);
        g_error_free (err);
        g_key_file_free (conf);
        continue;
      }
      
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter,
                          CONF_COL_BACKEND, backends[i],
                          CONF_COL_NAME, grind_indenter_get_name (backends[i]),
                          CONF_COL_CONF, conf,
                          -1);
    }
    cell = gtk_cell_renderer_text_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), cell, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), cell,
                                    "text", CONF_COL_NAME, NULL);
    
    gtk_box_pack_start (GTK_BOX (box), combo, FALSE, FALSE, 0);
    
    indenter_ui = grind_indenter_ui_new (NULL);
    gtk_box_pack_start (GTK_BOX (box), indenter_ui, TRUE, TRUE, 0);
    
    g_signal_connect (combo, "destroy", G_CALLBACK (on_combo_destroy), NULL);
    g_signal_connect (combo, "changed",
                      G_CALLBACK (on_configure_backend_changed), indenter_ui);
    g_signal_connect (dialog, "response",
                      G_CALLBACK (on_configure_response), store);
    
    /* set the active iter after connecting signals so the handler is called */
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0); /* FIXME: */
    
    gtk_widget_show_all (box);
  }
  g_free (backends);
  
  return box;
}

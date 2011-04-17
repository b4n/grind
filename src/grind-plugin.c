/*
 *  Copyright 2011 Colomban Wendling  <ban@herbesfolles.org>
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
      gchar *name = grind_indenter_get_name (indenter);
      gchar *author = grind_indenter_get_author (indenter);
      
      g_debug ("Using indenter \"%s\" by \"%s\"", name, author);
      
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

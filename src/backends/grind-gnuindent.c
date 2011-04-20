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

#include "grind-gnuindent.h"

#include <glib.h>
#include <glib-object.h>

#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "geanyplugin.h"

#include "grind-plugin.h"
#include "grind-indenter.h"


static GrindIndenterInterface  *grind_backend_gnuindent_indenter_parent_iface = NULL;

enum
{
  PROP_0,
  PROP_AUTHOR,
  PROP_VERSION,
  PROP_NAME,
  PROP_DESCRIPTION
};


static void     grind_backend_gnuindent_indenter_iface_init   (GrindIndenterInterface *iface);
static gboolean grind_backend_gnuindent_real_indent           (GrindIndenter *base,
                                                               GeanyDocument *doc,
                                                               gint           start,
                                                               gint           end);


G_DEFINE_TYPE_WITH_CODE (GrindBackendGNUIndent, grind_backend_gnuindent,
                         G_TYPE_OBJECT,
                         {
                           G_IMPLEMENT_INTERFACE (GRIND_TYPE_INDENTER,
                                                  grind_backend_gnuindent_indenter_iface_init);
                         })


static void
grind_backend_gnuindent_get_property (GObject    *object,
                                      guint       property_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  switch (property_id) {
    case PROP_AUTHOR:
      g_value_set_string (value, "The GRInd authors");
      break;
    
    case PROP_VERSION:
      g_value_set_string (value, "0.1");
      break;
    
    case PROP_NAME:
      g_value_set_string (value, "GNU Indent");
      break;
    
    case PROP_DESCRIPTION:
      g_value_set_string (value, "Indenter using GNUIndent");
      break;
    
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
grind_backend_gnuindent_class_init (GrindBackendGNUIndentClass *klass)
{
  grind_backend_gnuindent_parent_class = g_type_class_peek_parent (klass);
  
  G_OBJECT_CLASS (klass)->get_property = grind_backend_gnuindent_get_property;
  
  g_object_class_override_property (G_OBJECT_CLASS (klass),
                                    PROP_AUTHOR, "author");
  g_object_class_override_property (G_OBJECT_CLASS (klass),
                                    PROP_VERSION, "version");
  g_object_class_override_property (G_OBJECT_CLASS (klass),
                                    PROP_NAME, "name");
  g_object_class_override_property (G_OBJECT_CLASS (klass),
                                    PROP_DESCRIPTION, "description");
}

static void
grind_backend_gnuindent_indenter_iface_init (GrindIndenterInterface *iface)
{
  grind_backend_gnuindent_indenter_parent_iface = g_type_interface_peek_parent (iface);
  
  iface->indent           = grind_backend_gnuindent_real_indent;
  /*iface->get_author       = grind_gnuindent_backend_real_get_author;
  iface->get_version      = grind_gnuindent_backend_real_get_version;
  iface->get_name         = grind_gnuindent_backend_real_get_name;
  iface->get_description  = grind_gnuindent_backend_real_get_description;*/
}

static void grind_backend_gnuindent_init (GrindBackendGNUIndent *self)
{
  
}



typedef struct _BlockData BlockData;
struct _BlockData
{
  GError     *error;
  GMainLoop  *loop;
  
  GString    *output_str;
  GString    *error_str;
};


static gboolean
is_language_supported (GeanyFiletype   *ft)
{
  return ft->id == GEANY_FILETYPES_C;
}

static GIOChannel *
create_io_channel (int fd)
{
#ifdef G_OS_WIN32
  return g_io_channel_win32_new_fd (fd);
#else
  return g_io_channel_unix_new (fd);
#endif
}

static GSource *
create_watch_fd (int           fd,
                 GIOCondition  cond,
                 GIOFunc       func,
                 gpointer      data)
{
  GIOChannel *channel;
  GSource    *source;
  
  channel = create_io_channel (fd);
  
  source = g_io_create_watch (channel, cond);
  g_source_set_callback (source, (GSourceFunc)func, data, NULL);
  
  g_io_channel_unref (channel);
  
  return source;
}

static void
add_watch_fd (int           fd,
              GIOCondition  cond,
              GMainContext *ctx,
              GIOFunc       func,
              gpointer      data)
{
  GSource *source;
  
  source = create_watch_fd (fd, cond, func, data);
  g_source_attach (source, ctx);
  g_source_unref (source);
}

static void
child_watch_handler (GPid        pid,
                     gint        status,
                     BlockData  *data)
{
  g_debug ("child exited");
  if (! WIFEXITED (status)) {
    if (data->error) {
      /* override possible errors */
      g_error_free (data->error);
    }
    if (WIFSIGNALED (status)) {
      data->error = g_error_new (G_SPAWN_ERROR, G_SPAWN_ERROR_FAILED,
                                 "Child received signal %d (%s)",
                                 WTERMSIG (status), data->error_str->str);
    } else {
      data->error = g_error_new (G_SPAWN_ERROR, G_SPAWN_ERROR_FAILED,
                                 "Child crashed (%s)", data->error_str->str);
    }
  } else if (WEXITSTATUS (status) != 0) {
    if (data->error) {
      /* override possible errors */
      g_error_free (data->error);
    }
    data->error = g_error_new (G_SPAWN_ERROR, G_SPAWN_ERROR_FAILED,
                               "Child returned with error code %d (%s)",
                               WEXITSTATUS (status), data->error_str->str);
  } else {
    g_debug ("child succeeded");
  }
  g_spawn_close_pid (pid);
  g_main_loop_quit (data->loop);
}

static gboolean
handle_output_ready (GIOChannel  *channel,
                     GIOCondition condition,
                     GString     *str,
                     BlockData   *data)
{
  g_debug ("output handler");
  
  /* if there is aready an error, abort */
  if (data->error) {
    return FALSE;
  }
  
  if (condition & (G_IO_IN | G_IO_PRI)) {
    GIOStatus status;
    
    do {
      gchar  *line;
      gsize   len;
      
      /*g_debug ("reading...");*/
      status = g_io_channel_read_line (channel, &line, &len, NULL, &data->error);
      /*g_debug ("read %lu bytes: %s", len, line);*/
      if (line != NULL) {
        g_string_append_len (str, line, (gssize)len);
        g_free (line);
      }
    } while (status == G_IO_STATUS_NORMAL || status == G_IO_STATUS_AGAIN);
  }
  g_io_channel_shutdown(channel, FALSE, data->error ? NULL : &data->error);
  
  return FALSE;
}

static gboolean
output_ready_handler (GIOChannel   *channel,
                      GIOCondition  condition,
                      BlockData    *data)
{
  return handle_output_ready (channel, condition, data->output_str, data);
}

static gboolean
error_ready_handler (GIOChannel    *channel,
                     GIOCondition   condition,
                     BlockData     *data)
{
  return handle_output_ready (channel, condition, data->error_str, data);
}

static gboolean
write_input (int          fd,
             const gchar *data,
             GError     **error)
{
  GIOChannel *channel = create_io_channel (fd);
  GIOStatus   status;
  
  do {
    status = g_io_channel_write_chars (channel, data, -1, NULL, error);
  } while (status == G_IO_STATUS_AGAIN);
  /* don't care about shutdown errors if we already have an error */
  if (status != G_IO_CHANNEL_ERROR) {
    status = g_io_channel_shutdown(channel, TRUE, error);
  } else {
    g_io_channel_shutdown(channel, TRUE, NULL);
  }
  
  return status != G_IO_STATUS_ERROR;
}

static gchar **
build_args (GrindBackendGNUIndent  *self,
            GeanyDocument          *doc)
{
  GPtrArray              *array;
  const GeanyIndentPrefs *iprefs;
  
  array = g_ptr_array_new ();
  
  g_ptr_array_add (array, g_strdup ("indent"));
  g_ptr_array_add (array, g_strdup ("-st"));  /* write to stdout */
  
  iprefs = editor_get_indent_prefs (doc->editor);
  
  /* indentation width */
  g_ptr_array_add (array, g_strdup_printf ("-i%d", iprefs->width));
  /* brache indentation defautls to the indentation size, may be set to 0
   * later */
  g_ptr_array_add (array, g_strdup_printf ("-bli%d", iprefs->width));
  /* line length */
  g_ptr_array_add (array, g_strdup_printf ("-l%d", geany_data->editor_prefs->line_break_column));
  switch (iprefs->type) {
    case GEANY_INDENT_TYPE_SPACES:
      g_ptr_array_add (array, g_strdup ("-nut"));
      break;
    
    case GEANY_INDENT_TYPE_BOTH:
      g_ptr_array_add (array, g_strdup ("-ut"));
      g_ptr_array_add (array, g_strdup_printf ("-ts%d", iprefs->width));
      break;
    
    case GEANY_INDENT_TYPE_TABS:
      g_ptr_array_add (array, g_strdup ("-ut"));
      g_ptr_array_add (array, g_strdup_printf ("-ts%d", iprefs->hard_tab_width));
      break;
  }
  
  g_ptr_array_add (array, NULL);
  
  return g_ptr_array_free (array, FALSE);
}

static gchar *
gnuindent (GrindBackendGNUIndent *self,
           GeanyDocument         *doc,
           const gchar           *input,
           GError               **error)
{
  GPid    pid;
  int     input_fd;
  int     output_fd;
  int     error_fd;
  gchar  *output = NULL;
  gchar **argv;
  
  argv = build_args (self, doc);
  if (g_spawn_async_with_pipes (NULL, argv, NULL,
                                G_SPAWN_SEARCH_PATH | G_SPAWN_DO_NOT_REAP_CHILD,
                                NULL, NULL, &pid, &input_fd, &output_fd,
                                &error_fd, error)) {
    GSource      *source;
    GMainContext *ctx = g_main_context_new ();
    BlockData     data;
    
    data.error = NULL;
    data.loop = g_main_loop_new (ctx, FALSE);
    data.output_str = g_string_new (NULL);
    data.error_str = g_string_new (NULL);
    
    source = g_child_watch_source_new (pid);
    g_source_set_callback (source, (GSourceFunc)child_watch_handler,
                           &data, NULL);
    g_source_attach (source, ctx);
    g_source_unref (source);
    
    /* output */
    add_watch_fd (output_fd,
                  G_IO_IN | G_IO_PRI | G_IO_ERR | G_IO_HUP | G_IO_NVAL, ctx,
                  (GIOFunc)output_ready_handler, &data);
    /* error */
    add_watch_fd (error_fd,
                  G_IO_IN | G_IO_PRI | G_IO_ERR | G_IO_HUP | G_IO_NVAL, ctx,
                  (GIOFunc)error_ready_handler, &data);
    
    if (write_input (input_fd, input, &data.error)) {
      g_main_loop_run (data.loop);
    }
      
    if (! data.error) {
      output = g_string_free (data.output_str, FALSE);
    } else {
      g_string_free (data.output_str, TRUE);
      g_propagate_error (error, data.error);
    }
    g_string_free (data.error_str, TRUE);
    g_main_loop_unref (data.loop);
    g_main_context_unref (ctx);
  }
  g_strfreev (argv);
  
  return output;
}

static gboolean
grind_backend_gnuindent_real_indent (GrindIndenter *base,
                                     GeanyDocument *doc,
                                     gint           start,
                                     gint           end)
{
  ScintillaObject  *sci = doc->editor->sci;
  gchar            *input;
  gchar            *output;
  GError           *err = NULL;
  
  if (! is_language_supported (doc->file_type)) {
    return FALSE;
  }
  
  input = sci_get_contents_range (sci, start, end);
  output = gnuindent (GRIND_BACKEND_GNUINDENT (base), doc, input, &err);
  g_free (input);
  if (! output) {
    g_critical ("GNUIndent failed: %s", err->message);
    g_error_free (err);
  } else {
    sci_set_target_start (sci, start);
    sci_set_target_end (sci, end);
    sci_replace_target (sci, output, FALSE);
  }
  g_free (output);
  
  return output != NULL;
}

GrindIndenter *
grind_backend_gnuindent_new (void)
{
  return g_object_new (GRIND_TYPE_BACKEND_GNUINDENT, NULL);
}

/*static const gchar *
grind_backend_gnuindent_real_get_author (GrindIndenter *base)
{
  return "The GRInd authors";
}

static const gchar *
grind_backend_gnuindent_real_get_version (GrindIndenter *base)
{
  return "0.1";
}

static const gchar *
grind_backend_gnuindent_real_get_name (GrindIndenter *base)
{
  return "GNU Indent";
}

static const gchar *
grind_backend_gnuindent_real_get_description (GrindIndenter *base)
{
  return "Indenter using GNUIndent";
}*/

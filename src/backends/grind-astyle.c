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

#include "grind-astyle.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#include <glib.h>
#include <glib-object.h>
#include <glib/gstdio.h>

#include "geanyplugin.h"

#include "grind-plugin.h"
#include "grind-indenter.h"


static GrindIndenterInterface  *grind_backend_astyle_indenter_parent_iface = NULL;

enum
{
  PROP_0,
  PROP_AUTHOR,
  PROP_VERSION,
  PROP_NAME,
  PROP_DESCRIPTION
};


static void     grind_backend_astyle_indenter_iface_init   (GrindIndenterInterface *iface);
static gboolean grind_backend_astyle_real_indent           (GrindIndenter *base,
                                                               GeanyDocument *doc,
                                                               gint           start,
                                                               gint           end);


G_DEFINE_TYPE_WITH_CODE (GrindBackendAStyle, grind_backend_astyle,
                         G_TYPE_OBJECT,
                         {
                           G_IMPLEMENT_INTERFACE (GRIND_TYPE_INDENTER,
                                                  grind_backend_astyle_indenter_iface_init);
                         })


static void
grind_backend_astyle_get_property (GObject    *object,
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
      g_value_set_string (value, "Artistic Style");
      break;
    
    case PROP_DESCRIPTION:
      g_value_set_string (value, "Indenter using AStyle");
      break;
    
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
grind_backend_astyle_class_init (GrindBackendAStyleClass *klass)
{
  grind_backend_astyle_parent_class = g_type_class_peek_parent (klass);
  
  G_OBJECT_CLASS (klass)->get_property = grind_backend_astyle_get_property;
  
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
grind_backend_astyle_indenter_iface_init (GrindIndenterInterface *iface)
{
  grind_backend_astyle_indenter_parent_iface = g_type_interface_peek_parent (iface);
  
  iface->indent = grind_backend_astyle_real_indent;
}

static void grind_backend_astyle_init (GrindBackendAStyle *self)
{
  
}


static void
build_args_add_indent_type (GrindBackendAStyle *self,
                            GeanyDocument      *doc,
                            GPtrArray          *array)
{
  const GeanyIndentPrefs *iprefs;
  
  iprefs = editor_get_indent_prefs (doc->editor);
  switch (iprefs->type) {
    case GEANY_INDENT_TYPE_SPACES:
      g_ptr_array_add (array, g_strdup_printf ("-s%d", iprefs->width));
      break;
    
    case GEANY_INDENT_TYPE_BOTH:
      /* AStyle don't seem to know tab&space indentation mode, so only use
       * spaces. Maybe warn the user? */
      /*g_ptr_array_add (array, g_strdup_printf ("-t%d", iprefs->width));*/
      g_ptr_array_add (array, g_strdup_printf ("-s%d", iprefs->width));
      break;
    
    case GEANY_INDENT_TYPE_TABS:
      /* is -T really wanted? maybe -t is better, not sure. */
      g_ptr_array_add (array, g_strdup_printf ("-T%d", iprefs->width));
      break;
  }
}

#ifndef sci_get_eol_mode
#define sci_get_eol_mode(sci) \
  (scintilla_send_message ((sci), SCI_GETEOLMODE, 0, 0))
#endif

static void
build_args_add_eol_type (GrindBackendAStyle  *self,
                         GeanyDocument       *doc,
                         GPtrArray           *array)
{
  switch (sci_get_eol_mode (doc->editor->sci)) {
    case SC_EOL_CR:
      g_ptr_array_add (array, g_strdup ("--lineend=macold"));
      break;
    
    case SC_EOL_CRLF:
      g_ptr_array_add (array, g_strdup ("--lineend=windows"));
      break;
    
    case SC_EOL_LF:
      g_ptr_array_add (array, g_strdup ("--lineend=linux"));
      break;
  }
}

static void
build_args_add_file_type (GrindBackendAStyle *self,
                          GeanyDocument      *doc,
                          GPtrArray          *array)
{
  switch (doc->file_type->id) {
    case GEANY_FILETYPES_CS:
    case GEANY_FILETYPES_VALA:
      g_ptr_array_add (array, g_strdup ("--mode=cs"));
      break;
    
    case GEANY_FILETYPES_JAVA:
      g_ptr_array_add (array, g_strdup ("--mode=java"));
      break;
    
    default:
      g_ptr_array_add (array, g_strdup ("--mode=c"));
      break;
  }
}

static gchar **
build_args (GrindBackendAStyle *self,
            GeanyDocument      *doc,
            const gchar        *filename)
{
  GPtrArray  *array;
  
  array = g_ptr_array_new ();
  
  g_ptr_array_add (array, g_strdup ("astyle"));
  g_ptr_array_add (array, g_strdup ("-n")); /* don't create backups */
  
  build_args_add_indent_type (self, doc, array);
  build_args_add_eol_type (self, doc, array);
  build_args_add_file_type (self, doc, array);
  
  g_ptr_array_add (array, g_strdup (filename)); /* process filename */
  g_ptr_array_add (array, NULL);
  
  /*{
    gchar  *e;
    guint   i;
    
    foreach_ptr_array (e, i, array) {
      g_debug ("argv[%u] = %s", i, e);
    }
  }*/
  
  return (gchar **)g_ptr_array_free (array, FALSE);
}

static gboolean
is_language_supported (GeanyFiletype *ft)
{
  switch (ft->id)
  {
    case GEANY_FILETYPES_C:
    case GEANY_FILETYPES_CPP:
    case GEANY_FILETYPES_CS:
    case GEANY_FILETYPES_JAVA:
    case GEANY_FILETYPES_VALA: /* vala is similar to CS/JAVA, should work too */
      return TRUE;
    
    default:
      return FALSE;
  }
}

static gchar *
write_tempfile (const gchar  *data,
                GError      **error)
{
  gchar  *filename;
  gint    fd;
  
  fd = g_file_open_tmp ("grind-astyle.XXXXXX", &filename, error);
  if (fd >= 0) {
    gsize   len = strlen (data);
    gssize  nw;
    
    nw = write (fd, data, len);
    if (nw < 0 || (gsize)nw < len) {
      gint save_errno = errno;
      
      g_set_error_literal (error, G_IO_ERROR,
                           g_io_error_from_errno (save_errno),
                           g_strerror (save_errno));
      g_unlink (filename);
      g_free (filename);
      filename = NULL;
    }
    if (close (fd) < 0 && filename != NULL /* don't override error */) {
      gint save_errno = errno;
      
      g_set_error_literal (error, G_IO_ERROR,
                           g_io_error_from_errno (save_errno),
                           g_strerror (save_errno));
      g_unlink (filename);
      g_free (filename);
      filename = NULL;
    }
  }
  
  return filename;
}

static gboolean
check_child_status (gint          status,
                    const gchar  *child_stderr,
                    GError      **error)
{
  gboolean success = FALSE;
  
  if (! WIFEXITED (status)) {
    if (WIFSIGNALED (status)) {
      g_set_error (error, G_SPAWN_ERROR, G_SPAWN_ERROR_FAILED,
                   "Child received signal %d (%s)",
                   WTERMSIG (status), child_stderr);
    } else {
      g_set_error (error, G_SPAWN_ERROR, G_SPAWN_ERROR_FAILED,
                   "Child crashed (%s)", child_stderr);
    }
  } else if (WEXITSTATUS (status) != 0) {
    g_set_error (error, G_SPAWN_ERROR, G_SPAWN_ERROR_FAILED,
                 "Child returned with error code %d (%s)",
                 WEXITSTATUS (status), child_stderr);
  } else {
    success = TRUE;
  }
  
  return success;
}

static gchar *
astyle (GrindBackendAStyle *self,
        GeanyDocument      *doc,
        const gchar        *input,
        GError            **error)
{
  gchar  *output = NULL;
  gchar  *filename;
  
  filename = write_tempfile (input, error);
  if (filename) {
    gchar **argv;
    gchar  *child_stderr;
    gint    status;
    
    argv = build_args (self, doc, filename);
    if (g_spawn_sync (NULL, argv, NULL,
                      G_SPAWN_SEARCH_PATH | G_SPAWN_STDOUT_TO_DEV_NULL,
                      NULL, NULL, NULL, &child_stderr, &status, error)) {
      if (! check_child_status (status, child_stderr, error)) {
        g_debug ("errors = %s", child_stderr);
      } else {
        if (! g_file_get_contents (filename, &output, NULL, error)) {
          output = NULL;
        }
      }
      g_free (child_stderr);
    }
    g_strfreev (argv);
    g_unlink (filename);
    g_free (filename);
  }
  
  return output;
}

static gboolean
grind_backend_astyle_real_indent (GrindIndenter  *base,
                                  GeanyDocument  *doc,
                                  gint            start,
                                  gint            end)
{
  ScintillaObject  *sci = doc->editor->sci;
  gchar            *input;
  gchar            *output;
  GError           *err = NULL;
  
  if (! is_language_supported (doc->file_type)) {
    return FALSE;
  }
  
  input = sci_get_contents_range (sci, start, end);
  output = astyle (GRIND_BACKEND_ASTYLE (base), doc, input, &err);
  g_free (input);
  if (! output) {
    g_critical ("AStyle failed: %s", err->message);
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
grind_backend_astyle_new (void)
{
  return g_object_new (GRIND_TYPE_BACKEND_ASTYLE, NULL);
}

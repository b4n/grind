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

#include "grind-enum-types.h"
#include "grind-plugin.h"
#include "grind-indenter.h"
#include "grind-indenter-option.h"


struct _GrindBackendAStylePrivate
{
  gint      style;
  gint      indent_type;
  gint      indent_width;
  gint      brackets;
  gboolean  indent_classes;
  gboolean  indent_switches;
  gboolean  indent_cases;
  gboolean  indent_brackets;
  gboolean  indent_blocks;
  gboolean  indent_namespaces;
  gboolean  indent_labels;
  gboolean  indent_preprocessor;
  gboolean  indent_col1_comments;
  gint      max_instatement_indent;
  gint      min_conditional_indent;
};


static const gchar *grind_backend_astyle_real_get_author      (GrindIndenter *base);
static const gchar *grind_backend_astyle_real_get_description (GrindIndenter *base);
static const gchar *grind_backend_astyle_real_get_name        (GrindIndenter *base);
static const gchar *grind_backend_astyle_real_get_version     (GrindIndenter *base);
static gboolean     grind_backend_astyle_real_indent          (GrindIndenter *base,
                                                               GeanyDocument *doc,
                                                               gint           start,
                                                               gint           end);


G_DEFINE_TYPE (GrindBackendAStyle, grind_backend_astyle, GRIND_TYPE_INDENTER)


static void
grind_backend_astyle_class_init (GrindBackendAStyleClass *klass)
{
  GObjectClass       *object_class    = G_OBJECT_CLASS (klass);
  GrindIndenterClass *indenter_class  = GRIND_INDENTER_CLASS (klass);
  guint               id = 0;
  
  object_class->get_property = grind_indenter_option_get_property_impl;
  object_class->set_property = grind_indenter_option_set_property_impl;
  
  indenter_class->get_author      = grind_backend_astyle_real_get_author;
  indenter_class->get_description = grind_backend_astyle_real_get_description;
  indenter_class->get_name        = grind_backend_astyle_real_get_name;
  indenter_class->get_version     = grind_backend_astyle_real_get_version;
  indenter_class->indent          = grind_backend_astyle_real_indent;
  
  g_object_class_install_property (object_class, ++id,
    grind_enum_indenter_option_new ("style",
                                    "Style",
                                    "Predefined styles",
                                    GRIND_TYPE_BACKEND_ASTYLE_STYLE,
                                    GRIND_BACKEND_ASTYLE_STYLE_ANSI,
                                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                    G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                     style)));
  g_object_class_install_property (object_class, ++id,
    grind_enum_indenter_option_new ("indent-type",
                                    "Indentation type",
                                    "The type of the indentation. "
                                    "\"dcoument setting\" uses the document's indentation type",
                                    GRIND_TYPE_BACKEND_ASTYLE_INDENT_TYPE,
                                    GRIND_BACKEND_ASTYLE_INDENT_TYPE_DOCUMENT_SETTING,
                                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                    G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                     indent_type)));
  g_object_class_install_property (object_class, ++id,
    grind_int_indenter_option_new ("indent-width",
                                   "Indentation width",
                                   "The width of the indentation, in spaces. "
                                   "A value of 0 uses the document's indentation width",
                                   0,
                                   20,
                                   0,
                                   G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                   G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                    indent_width)));
  g_object_class_install_property (object_class, ++id,
    grind_enum_indenter_option_new ("brackets",
                                    "Brackets",
                                    "Brackets breaking style",
                                    GRIND_TYPE_BACKEND_ASTYLE_BRACKETS,
                                    GRIND_BACKEND_ASTYLE_BRACKETS_BREAK,
                                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                    G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                     brackets)));
  g_object_class_install_property (object_class, ++id,
    grind_boolean_indenter_option_new ("indent-classes",
                                       "Indent classes",
                                       "Indent 'class' blocks, so that the "
                                       "inner 'public:', 'protected:' and "
                                       "'private:' headers are indented in "
                                       "relation to the class block. "
                                       "This option has no effect on Java and "
                                       "C# files.",
                                       FALSE,
                                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                       G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                        indent_classes)));
  g_object_class_install_property (object_class, ++id,
    grind_boolean_indenter_option_new ("indent-switches",
                                       "Indent switches",
                                       "Indent 'switch' blocks, so that the "
                                       "inner 'case X:' headers are indented "
                                       "in relation to the switch block. "
                                       "The entire case block is indented.",
                                       FALSE,
                                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                       G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                        indent_switches)));
  g_object_class_install_property (object_class, ++id,
    grind_boolean_indenter_option_new ("indent-cases",
                                       "Indent cases",
                                       "Indent 'case X:' lines, so that they "
                                       "are flush with their bodies. Case "
                                       "statements not enclosed in blocks are "
                                       "NOT indented.",
                                       FALSE,
                                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                       G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                        indent_cases)));
  g_object_class_install_property (object_class, ++id,
    grind_boolean_indenter_option_new ("indent-brackets",
                                       "Indent brackets",
                                       "Add extra indentation to '{' and '}' "
                                       "block brackets. This is the option "
                                       "used for Whitesmith and Banner style "
                                       "for‐matting/indenting. "
                                       "If both indent-brackets and "
                                       "indent-blocks are used the result will "
                                       "be indent-blocks.",
                                       FALSE,
                                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                       G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                        indent_brackets)));
  g_object_class_install_property (object_class, ++id,
    grind_boolean_indenter_option_new ("indent-blocks",
                                       "Indent blocks",
                                       "Add extra indentation to blocks within "
                                       "a function. The opening bracket for "
                                       "namespaces, classes, and functions is "
                                       "not indented. This is the option used "
                                       "for GNU style formatting/indenting.",
                                       FALSE,
                                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                       G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                        indent_blocks)));
  g_object_class_install_property (object_class, ++id,
    grind_boolean_indenter_option_new ("indent-namespaces",
                                       "Indent namespaces",
                                       "Add extra indentation to namespace "
                                       "blocks. This option has no effect on "
                                       "Java files.",
                                       FALSE,
                                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                       G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                        indent_namespaces)));
  g_object_class_install_property (object_class, ++id,
    grind_boolean_indenter_option_new ("indent-labels",
                                       "Indent labels",
                                       "Indent labels so that they appear one "
                                       "indent less than the current "
                                       "indentation level, rather than being "
                                       "flushed completely to the left.",
                                       FALSE,
                                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                       G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                        indent_labels)));
  g_object_class_install_property (object_class, ++id,
    grind_boolean_indenter_option_new ("indent-preprocessor",
                                       "Indent preprocessor",
                                       "Indent multi-line preprocessor "
                                       "definitions ending with a backslash. "
                                       "Should be used with convert-tabs for "
                                       "proper results. Does a pretty good "
                                       "job, but can not perform miracles in "
                                       "obfuscated preprocessor definitions.",
                                       FALSE,
                                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                       G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                        indent_preprocessor)));
  g_object_class_install_property (object_class, ++id,
    grind_boolean_indenter_option_new ("indent-col1-comments",
                                       "Indent first column comments",
                                       "Indent C++ comments  beginning in "
                                       "column one. By default C++ comments "
                                       "beginning in column one are not "
                                       "indented. This option will allow the "
                                       "comments to be indented with the code.",
                                       FALSE,
                                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                       G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                        indent_col1_comments)));
  g_object_class_install_property (object_class, ++id,
    grind_int_indenter_option_new ("max-instatement-indent",
                                   "Max in-statement indent",
                                   "Indent a maximum of N spaces in a "
                                   "continuous statement, relative to the "
                                   "previous line.",
                                   0,
                                   80,
                                   40,
                                   G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                   G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                    max_instatement_indent)));
  /* whatever AStyle's manual page or --help says, this is a FLAG that chooses
   * between 0/0.5/1/2 times the current indent */
  g_object_class_install_property (object_class, ++id,
    grind_enum_indenter_option_new ("min-conditional-indent",
                                    "Min conditional indent",
                                    "Indent a minimal N*indent spaces in a"
                                    "continuous conditional belonging to a"
                                    "conditional header.",
                                    GRIND_TYPE_BACKEND_ASTYLE_MIN_CONDITIONAL_MODE,
                                    GRIND_BACKEND_ASTYLE_MIN_CONDITIONAL_MODE_TWO,
                                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS,
                                    G_STRUCT_OFFSET (GrindBackendAStylePrivate,
                                                     min_conditional_indent)));
  
  g_type_class_add_private (klass, sizeof (GrindBackendAStylePrivate));
}

static void grind_backend_astyle_init (GrindBackendAStyle *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            GRIND_TYPE_BACKEND_ASTYLE,
                                            GrindBackendAStylePrivate);
  
  self->priv->style = GRIND_BACKEND_ASTYLE_STYLE_ANSI;
  self->priv->indent_type = GRIND_BACKEND_ASTYLE_INDENT_TYPE_DOCUMENT_SETTING;
  self->priv->indent_width = 0;
  self->priv->brackets = GRIND_BACKEND_ASTYLE_BRACKETS_BREAK;
  self->priv->indent_classes = FALSE;
  self->priv->indent_switches = FALSE;
  self->priv->indent_cases = FALSE;
  self->priv->indent_brackets = FALSE;
  self->priv->indent_blocks = FALSE;
  self->priv->indent_namespaces = FALSE;
  self->priv->indent_labels = FALSE;
  self->priv->indent_preprocessor = FALSE;
  self->priv->indent_col1_comments = FALSE;
  self->priv->max_instatement_indent = 40;
  self->priv->min_conditional_indent = GRIND_BACKEND_ASTYLE_MIN_CONDITIONAL_MODE_TWO;
}


static void
apply_indent_settings (GrindBackendAStyle  *self,
                       GeanyDocument       *doc)
{
  GeanyIndentType         type;
  gint                    width   = self->priv->indent_width;
  const GeanyIndentPrefs *iprefs  = editor_get_indent_prefs (doc->editor);
  
  if (width == 0) {
    width = iprefs->width;
  }
  switch (self->priv->indent_type) {
    case GRIND_BACKEND_ASTYLE_INDENT_TYPE_DOCUMENT_SETTING:
      type = iprefs->type;
      break;
    
    case GRIND_BACKEND_ASTYLE_INDENT_TYPE_FORCE_TABS:
    case GRIND_BACKEND_ASTYLE_INDENT_TYPE_TABS:
      type = GEANY_INDENT_TYPE_TABS;
      break;
    
    default:
      type = GEANY_INDENT_TYPE_SPACES;
  }
  
  if (type != iprefs->type || width != iprefs->width) {
    editor_set_indent (doc->editor, type, width);
  }
}

static void
build_args_add_indent_type (GrindBackendAStyle *self,
                            GeanyDocument      *doc,
                            GPtrArray          *array)
{
  const GeanyIndentPrefs       *iprefs  = editor_get_indent_prefs (doc->editor);
  gint                          width   = self->priv->indent_width;
  GrindBackendAStyleIndentType  type    = self->priv->indent_type;
  gchar                         arg;
  
  if (width == 0) {
    width = iprefs->width;
  }
  if (type == GRIND_BACKEND_ASTYLE_INDENT_TYPE_DOCUMENT_SETTING) {
    switch (iprefs->type) {
      case GEANY_INDENT_TYPE_BOTH:
        /* AStyle don't seem to know tab&space indentation mode, so only use
         * spaces. Maybe warn the user? */
      case GEANY_INDENT_TYPE_SPACES:
        type = GRIND_BACKEND_ASTYLE_INDENT_TYPE_SPACES;
        break;
      
      case GEANY_INDENT_TYPE_TABS:
        /* is "force-tabs" really wanted? maybe "tabs" is better, not sure. */
        type = GRIND_BACKEND_ASTYLE_INDENT_TYPE_FORCE_TABS;
        break;
    }
  }
  
  switch (type) {
    default:
    case GRIND_BACKEND_ASTYLE_INDENT_TYPE_SPACES:     arg = 's';  break;
    case GRIND_BACKEND_ASTYLE_INDENT_TYPE_TABS:       arg = 't';  break;
    case GRIND_BACKEND_ASTYLE_INDENT_TYPE_FORCE_TABS: arg = 'T';  break;
  }
  
  g_ptr_array_add (array, g_strdup_printf ("-%c%d", arg, width));
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

static void
build_args_add_brackets_type (GrindBackendAStyle *self,
                              GPtrArray          *array)
{
  GEnumClass *enum_class;
  GEnumValue *enum_value;
  
  enum_class = g_type_class_ref (GRIND_TYPE_BACKEND_ASTYLE_BRACKETS);
  enum_value = g_enum_get_value (enum_class, self->priv->brackets);
  g_ptr_array_add (array, g_strdup_printf ("--brackets=%s",
                                           enum_value->value_nick));
  g_type_class_unref (enum_class);
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
  build_args_add_brackets_type (self, array);
  
  if (self->priv->indent_classes) {
    g_ptr_array_add (array, g_strdup ("--indent-classes"));
  }
  if (self->priv->indent_switches) {
    g_ptr_array_add (array, g_strdup ("--indent-switches"));
  }
  if (self->priv->indent_cases) {
    g_ptr_array_add (array, g_strdup ("--indent-cases"));
  }
  if (self->priv->indent_brackets) {
    g_ptr_array_add (array, g_strdup ("--indent-brackets"));
  }
  if (self->priv->indent_blocks) {
    g_ptr_array_add (array, g_strdup ("--indent-blocks"));
  }
  if (self->priv->indent_namespaces) {
    g_ptr_array_add (array, g_strdup ("--indent-namespaces"));
  }
  if (self->priv->indent_labels) {
    g_ptr_array_add (array, g_strdup ("--indent-labels"));
  }
  if (self->priv->indent_preprocessor) {
    g_ptr_array_add (array, g_strdup ("--indent-preprocessor"));
  }
  if (self->priv->indent_col1_comments) {
    g_ptr_array_add (array, g_strdup ("--indent-col1-comments"));
  }
  g_ptr_array_add (array, g_strdup_printf ("--max-instatement-indent=%d",
                                           self->priv->max_instatement_indent));
  /* although this is a flag it's passed as a digit */
  g_ptr_array_add (array, g_strdup_printf ("--min-conditional-indent=%d",
                                           self->priv->min_conditional_indent));
  
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
  
  /* we use a temp file rather than piping data to astyle because astyle
   * hangs if the piped data isn't seekable */
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
  GrindBackendAStyle *self = GRIND_BACKEND_ASTYLE (base);
  ScintillaObject    *sci = doc->editor->sci;
  gchar              *input;
  gchar              *output;
  GError             *err = NULL;
  
  if (! is_language_supported (doc->file_type)) {
    return FALSE;
  }
  
  input = sci_get_contents_range (sci, start, end);
  output = astyle (self, doc, input, &err);
  g_free (input);
  if (! output) {
    g_critical ("AStyle failed: %s", err->message);
    g_error_free (err);
  } else {
    sci_set_target_start (sci, start);
    sci_set_target_end (sci, end);
    sci_replace_target (sci, output, FALSE);
    
    apply_indent_settings (self, doc);
  }
  g_free (output);
  
  return output != NULL;
}

static const gchar *
grind_backend_astyle_real_get_author (GrindIndenter *base)
{
  return "The GRInd authors";
}

static const gchar *
grind_backend_astyle_real_get_description (GrindIndenter *base)
{
  return "Indenter using AStyle";
}

static const gchar *
grind_backend_astyle_real_get_name (GrindIndenter *base)
{
  return "Artistic Style";
}

static const gchar *
grind_backend_astyle_real_get_version (GrindIndenter *base)
{
  return "0.1";
}


GrindIndenter *
grind_backend_astyle_new (void)
{
  return g_object_new (GRIND_TYPE_BACKEND_ASTYLE, NULL);
}

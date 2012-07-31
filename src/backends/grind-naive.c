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

#include "grind-naive.h"

#include <glib.h>
#include <glib-object.h>

#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "geanyplugin.h"

#include "grind-plugin.h"
#include "grind-indenter.h"


static const gchar *grind_backend_naive_real_get_author       (GrindIndenter *base);
static const gchar *grind_backend_naive_real_get_description  (GrindIndenter *base);
static const gchar *grind_backend_naive_real_get_name         (GrindIndenter *base);
static const gchar *grind_backend_naive_real_get_version      (GrindIndenter *base);
static gboolean     grind_backend_naive_real_indent           (GrindIndenter *base,
                                                               GeanyDocument *doc,
                                                               gint           start,
                                                               gint           end);


G_DEFINE_TYPE (GrindBackendNaive, grind_backend_naive, GRIND_TYPE_INDENTER)


static void
grind_backend_naive_class_init (GrindBackendNaiveClass *klass)
{
  GrindIndenterClass *indenter_class = GRIND_INDENTER_CLASS (klass);
  
  indenter_class->get_author      = grind_backend_naive_real_get_author;
  indenter_class->get_description = grind_backend_naive_real_get_description;
  indenter_class->get_name        = grind_backend_naive_real_get_name;
  indenter_class->get_version     = grind_backend_naive_real_get_version;
  indenter_class->indent          = grind_backend_naive_real_indent;
}

static void grind_backend_naive_init (GrindBackendNaive *self)
{
  
}


static gboolean
is_indent (char ch)
{
  return ch == ' ' || ch == '\t';
}

/* sets the indentation of @line and return the difference between the new
 * and old indents (in characters) */
static int
set_line_indentation (ScintillaObject  *sci,
                      int               line,
                      int               indent)
{
  int diff = 0;
  int start = sci_get_position_from_line (sci, line);
  int pos;
  
  for (pos = start; is_indent (sci_get_char_at (sci, pos)); pos ++) {
    diff --;
  }
  sci_set_line_indentation (sci, line, indent);
  for (pos = start; is_indent (sci_get_char_at (sci, pos)); pos ++) {
    diff ++;
  }
  
  return diff;
}

/* checks whether the language is supposedly supported (e.g. has braces) */
static gboolean
is_language_supported (GeanyFiletype *ft)
{
  switch (ft->id)
  {
    case GEANY_FILETYPES_AS:
    case GEANY_FILETYPES_C:
    case GEANY_FILETYPES_CPP:
    case GEANY_FILETYPES_CS:
    case GEANY_FILETYPES_CSS:
    case GEANY_FILETYPES_D:
    case GEANY_FILETYPES_GLSL:
    case GEANY_FILETYPES_HTML: /* for PHP & JS */
    case GEANY_FILETYPES_JAVA:
    case GEANY_FILETYPES_JS:
    case GEANY_FILETYPES_SH:
    case GEANY_FILETYPES_PHP:
    case GEANY_FILETYPES_PERL:
    case GEANY_FILETYPES_TCL:
    case GEANY_FILETYPES_VALA:
      return TRUE;
    
    default:
      return FALSE;
  }
}

#ifndef sci_get_eol_mode
#define sci_get_eol_mode(sci) \
  (scintilla_send_message ((sci), SCI_GETEOLMODE, 0, 0))
#endif

/* checks whether @ch is the last of a line */
static gboolean
is_line_end_ch (ScintillaObject  *sci,
                char              ch)
{
  return ch == '\n' || (ch == '\r' && sci_get_eol_mode (sci) == SC_EOL_CR);
}

static gboolean
grind_backend_naive_real_indent (GrindIndenter *base,
                                 GeanyDocument *doc,
                                 gint           start,
                                 gint           end)
{
  ScintillaObject  *sci = doc->editor->sci;
  gint              line;
  gint              i;
  gint              width;
  gint              step;
  gint              lexer;
  
  if (! is_language_supported (doc->file_type)) {
    return FALSE;
  }
  
  line = sci_get_line_from_position (sci, start);
  width = (line > 0) ? sci_get_line_indentation (sci, line - 1) : 0;
  step = editor_get_indent_prefs (doc->editor)->width;
  lexer = sci_get_lexer (sci);
  
  for (i = start; i < end; i++) {
    gint      diff    = 0;
    gchar     ch      = sci_get_char_at (sci, i);
    gboolean  is_code = highlighting_is_code_style (lexer,
                                                    sci_get_style_at (sci, i));
    
    if (is_code) {
      switch (ch) {
        case '{':
          width += step;
          break;
        
        case '}':
          width -= step;
          diff = set_line_indentation (sci, line, width);
          i += diff;
          break;
      }
    }
    if (is_line_end_ch (sci, ch)) {
      line ++;
      if (is_code) {
        diff = set_line_indentation (sci, line, width);
      }
    }
    end += diff;
  }
  
  return TRUE;
}

static const gchar *
grind_backend_naive_real_get_author (GrindIndenter *base)
{
  return "The GRInd authors";
}

static const gchar *
grind_backend_naive_real_get_description (GrindIndenter *base)
{
  return "A naive braces-only indenter";
}

static const gchar *
grind_backend_naive_real_get_name (GrindIndenter *base)
{
  return "Naive";
}

static const gchar *
grind_backend_naive_real_get_version (GrindIndenter *base)
{
  return "0.1";
}


GrindIndenter *
grind_backend_naive_new (void)
{
  return g_object_new (GRIND_TYPE_BACKEND_NAIVE, NULL);
}

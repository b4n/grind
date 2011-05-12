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

#ifndef H_GRIND_BACKEND_ASTYLE
#define H_GRIND_BACKEND_ASTYLE

#include <glib.h>
#include <glib-object.h>

#include "grind-indenter.h"

G_BEGIN_DECLS


#define GRIND_TYPE_BACKEND_ASTYLE             (grind_backend_astyle_get_type ())
#define GRIND_BACKEND_ASTYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_BACKEND_ASTYLE, GrindBackendAStyle))
#define GRIND_BACKEND_ASTYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  GRIND_TYPE_BACKEND_ASTYLE, GrindBackendAStyleClass))
#define GRIND_IS_BACKEND_ASTYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_BACKEND_ASTYLE))
#define GRIND_IS_BACKEND_ASTYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  GRIND_TYPE_BACKEND_ASTYLE))
#define GRIND_BACKEND_ASTYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  GRIND_TYPE_BACKEND_ASTYLE, GrindBackendAStyleClass))

typedef struct _GrindBackendAStyle        GrindBackendAStyle;
typedef struct _GrindBackendAStyleClass   GrindBackendAStyleClass;
typedef struct _GrindBackendAStylePrivate GrindBackendAStylePrivate;

struct _GrindBackendAStyle
{
  GrindIndenter parent_instance;
  GrindBackendAStylePrivate *priv;
};

struct _GrindBackendAStyleClass
{
  GrindIndenterClass parent_class;
};


GType               grind_backend_astyle_get_type     (void) G_GNUC_CONST;
GrindIndenter      *grind_backend_astyle_new          (void);


G_END_DECLS

#endif /* guard */

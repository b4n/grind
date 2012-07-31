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

#ifndef H_GRIND_BACKEND_NAIVE
#define H_GRIND_BACKEND_NAIVE

#include <glib.h>
#include <glib-object.h>

#include "grind-indenter.h"

G_BEGIN_DECLS


#define GRIND_TYPE_BACKEND_NAIVE            (grind_backend_naive_get_type ())
#define GRIND_BACKEND_NAIVE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRIND_TYPE_BACKEND_NAIVE, GrindBackendNaive))
#define GRIND_BACKEND_NAIVE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GRIND_TYPE_BACKEND_NAIVE, GrindBackendNaiveClass))
#define GRIND_IS_BACKEND_NAIVE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRIND_TYPE_BACKEND_NAIVE))
#define GRIND_IS_BACKEND_NAIVE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GRIND_TYPE_BACKEND_NAIVE))
#define GRIND_BACKEND_NAIVE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GRIND_TYPE_BACKEND_NAIVE, GrindBackendNaiveClass))

typedef struct _GrindBackendNaive         GrindBackendNaive;
typedef struct _GrindBackendNaiveClass    GrindBackendNaiveClass;
typedef struct _GrindBackendNaivePrivate  GrindBackendNaivePrivate;

struct _GrindBackendNaive
{
  GrindIndenter parent_instance;
  GrindBackendNaivePrivate *priv;
};

struct _GrindBackendNaiveClass
{
  GrindIndenterClass parent_class;
};


GType               grind_backend_naive_get_type    (void) G_GNUC_CONST;
GrindIndenter      *grind_backend_naive_new         (void);


G_END_DECLS

#endif /* guard */

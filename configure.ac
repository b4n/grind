
AC_PREREQ([2.64])
AC_INIT([GRInd],
        [0.1],
        [ban@herbesfolles.org],
        [grind])
AC_CONFIG_SRCDIR([src/grind-plugin.c])
AC_CONFIG_MACRO_DIR([build/m4])
AC_CONFIG_AUX_DIR([build/aux])
AM_INIT_AUTOMAKE([1.11.1 -Wall -Werror foreign subdir-objects])
AC_CONFIG_HEADERS([config.h])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Checks for programs.
LT_PREREQ([2.2.0])
LT_INIT

AC_PROG_CC
AC_PROG_CC_C_O

# Checks for libraries.
PKG_CHECK_MODULES([GRIND], [geany >= 0.21
                            gtk+-2.0
                            glib-2.0 >= 2.28])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/backends/Makefile])
AC_OUTPUT


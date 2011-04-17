
lib_LTLIBRARIES = libgrind-backend-naive.la \
                  libgrind-backend-gnuindent.la \
                  libgrind-backend-astyle.la


AM_CFLAGS   = $(GRIND_CFLAGS) -I$(srcdir)/..
AM_LDFLAGS  = $(GRIND_LIBS) -module -avoid-version

libgrind_backend_naive_la_SOURCES     = grind-naive.c
#libgrind_backend_naive_la_CFLAGS      = $(GRIND_CFLAGS)
#libgrind_backend_naive_la_LIBADD      = $(GRIND_LIBS)

libgrind_backend_gnuindent_la_SOURCES = grind-gnuindent.c
#libgrind_backend_gnuindent_la_CFLAGS  = $(GRIND_CFLAGS)
#libgrind_backend_gnuindent_la_LIBADD  = $(GRIND_LIBS)

libgrind_backend_astyle_la_SOURCES    = grind-astyle.c
#libgrind_backend_astyle_la_CFLAGS     = $(GRIND_CFLAGS)
#libgrind_backend_astyle_la_LIBADD     = $(GRIND_LIBS)

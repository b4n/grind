#SUBDIRS = backends

lib_LTLIBRARIES = libgrind-plugin.la


sources                     = grind-plugin.c \
                              grind-indenter.c \
                              grind-indenter-option.c \
                              grind-indenter-ui.c \
                              grind-backend-manager.c \
                              backends/grind-astyle.c \
                              backends/grind-gnuindent.c \
                              backends/grind-naive.c
headers                     = grind-plugin.h \
                              grind-indenter.h \
                              grind-indenter-option.h \
                              grind-indenter-ui.h \
                              grind-backend-manager.h \
                              backends/grind-astyle.h \
                              backends/grind-gnuindent.h \
                              backends/grind-naive.h
generated_sources           = grind-enum-types.c
generated_headers           = grind-enum-types.h


CLEANFILES = $(generated_sources) \
             $(generated_headers)
EXTRA_DIST = $(srcdir)/grind-enum-types.c.template \
             $(srcdir)/grind-enum-types.h.template


libgrind_plugin_la_SOURCES  = $(generated_sources) \
                              $(sources) \
                              $(generated_headers) \
                              $(headers)
libgrind_plugin_la_CFLAGS   = $(GRIND_CFLAGS) \
                              -DG_LOG_DOMAIN="\"GRInd\""
libgrind_plugin_la_LIBADD   = $(GRIND_LIBS)
libgrind_plugin_la_LDFLAGS  = -module -avoid-version


grind-enum-types.c: $(srcdir)/grind-enum-types.c.template \
                    grind-enum-types.h \
                    $(headers) \
                    Makefile
	$(AM_V_GEN) $(GLIB_MKENUMS) --template $< $(headers:%=$(srcdir)/%) > $@
grind-enum-types.h: $(srcdir)/grind-enum-types.h.template \
                    $(headers) \
                    Makefile
	$(AM_V_GEN) $(GLIB_MKENUMS) --template $< $(headers:%=$(srcdir)/%) > $@

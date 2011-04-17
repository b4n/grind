
#SUBDIRS = backends

lib_LTLIBRARIES = libgrind-plugin.la


libgrind_plugin_la_SOURCES  = grind-plugin.c \
                              grind-indenter.c \
                              grind-backend-manager.c \
                              backends/grind-astyle.c \
                              backends/grind-gnuindent.c \
                              backends/grind-naive.c
libgrind_plugin_la_CFLAGS   = $(GRIND_CFLAGS)
libgrind_plugin_la_LIBADD   = $(GRIND_LIBS)
libgrind_plugin_la_LDFLAGS  = -module -avoid-version

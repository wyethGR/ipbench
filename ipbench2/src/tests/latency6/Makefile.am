AM_CPPFLAGS = $(WARNINGCFLAGS)

plugindir=$(libdir)/ipbench
plugin_LTLIBRARIES = liblatency6.la
liblatency6_la_CFLAGS = -I$(top_srcdir)/src
liblatency6_la_LIBADD = ../../lib/libipbench.la
liblatency6_la_SOURCES=latency.c ip.c raw.c latency.h

AUTOMAKE_OPTIONS=foreign
SUBDIRS = core os/unix
bin_PROGRAMS=linc
linc_SOURCES=main.c
AM_CPPFLAGS=-I core
AM_CPPFLAGS+=-I os/unix 
linc_LDADD= core/libcore.la os/unix/libosunix.la $(FLIBS)

## Process this file with automake to produce Makefile.in

TESTS = check_clanchat
check_PROGRAMS = check_clanchat
check_clanchat_SOURCES = check_clanchat.c $(top_builddir)/src/clanchat.h
check_clanchat_CFLAGS = @CHECK_CFLAGS@
check_clanchat_LDADD = $(top_builddir)/src/libclanchat.la @CHECK_LIBS@


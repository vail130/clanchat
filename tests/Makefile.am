## Process this file with automake to produce Makefile.in

TESTS = check_clanchat check_clanchatapi
check_PROGRAMS = check_clanchat check_clanchatapi

check_clanchat_SOURCES = check_clanchat.c $(top_builddir)/src/clanchat.h
check_clanchat_CFLAGS = @CHECK_CFLAGS@
check_clanchat_LDADD = $(top_builddir)/src/libclanchat.la @CHECK_LIBS@

check_clanchatapi_SOURCES = check_clanchatapi.c $(top_builddir)/src/clanchatapi.h
check_clanchatapi_CFLAGS = @CHECK_CFLAGS@
check_clanchatapi_LDADD = $(top_builddir)/src/libclanchat.la @CHECK_LIBS@

if DARWIN
# Homebrew installs it there on Macs
check_clanchatapi_LDADD += -L/usr/local/opt/curl/lib -lcurl
endif


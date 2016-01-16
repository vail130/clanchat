lib_LTLIBRARIES = libclanchat.la
libclanchat_la_SOURCES = clanchat.c clanchat.h clanchatapi.c clanchatapi.h

bin_PROGRAMS = clanchat
clanchat_CFLAGS = # `pkg-config --cflags glib-2.0`
clanchat_LDFLAGS = # `pkg-config --libs glib-2.0`
clanchat_SOURCES = main.c # list out all source files
clanchat_LDADD = libclanchat.la

if DARWIN
# Brew installs it there on Macs
#clanchat_LDADD += -L/usr/local/opt/curl/lib -lcurl
endif


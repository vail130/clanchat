#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([CLANCHAT], [0.0.1], [vail130@gmail.com])
AM_PROG_AR

AM_INIT_AUTOMAKE([-Wall -Werror foreign 1.11.2])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_SRCDIR([src/clanchat.c])
AC_CONFIG_HEADERS([src/config.h])

AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_LIBTOOL

# Conditionals
AM_CONDITIONAL([DARWIN], [test $(uname) = "Darwin"])

if [[ $(uname) == "Darwin" ]]; then
    # OSX
    
    # Homebrew installs packages here
    LDFLAGS="-L/usr/local/opt/libmicrohttpd/lib -lmicrohttpd -L/usr/local/opt/ossp-uuid/lib -luuid"
    CFLAGS="-I/usr/local/opt/libmicrohttpd/include -I/usr/local/opt/ossp-uuid/include"

    DARWIN=1
else
    LDFLAGS="-lossp-uuid -lcurl"
    DARWIN=
fi

# Checks for libraries.
PKG_CHECK_MODULES([LIBMICROHTTPD], [libmicrohttpd >= 0.9.27])
PKG_CHECK_MODULES([LIBCURL], [libcurl >= 7.22.0])
PKG_CHECK_MODULES([CHECK], [check >= 0.9.6])

if [[ ! -z ${DARWIN} ]]; then
    PKG_CHECK_MODULES([OSSP_UUID], [uuid >= 1.6.2])
else
    PKG_CHECK_MODULES([OSSP_UUID], [ossp-uuid >= 1.6.2])
fi

AC_CHECK_LIB(microhttpd, MHD_start_daemon)
AC_CHECK_LIB(uuid, uuid_create)
AC_CHECK_LIB(curl, curl_easy_init)

# Checks for header files.
AC_CHECK_HEADERS([microhttpd.h])
AC_CHECK_HEADERS([ossp/uuid.h])
AC_CHECK_HEADERS([curl/curl.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Configure subdirs with Makefiles
AC_CONFIG_FILES([
                 Makefile
                 src/Makefile
                 tests/Makefile
                 ])
AC_OUTPUT


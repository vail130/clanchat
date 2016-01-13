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

# Checks for libraries.
PKG_CHECK_MODULES([CHECK], [check >= 0.9.6])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([
                 Makefile
                 src/Makefile
                 tests/Makefile
                 ])
AC_OUTPUT


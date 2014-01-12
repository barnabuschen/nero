#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([gtkMain], [1], [sdfsdf])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_CONFIG_SRCDIR([gtkMain.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_CPP
AC_PROG_MKDIR_P

# Checks for libraries.
PKG_CHECK_MODULES([gtkMain],[
    glib-2.0
    gtk+-3.0
])
PKG_PROG_PKG_CONFIG
# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h malloc.h netinet/in.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([ftruncate getcwd memset munmap strerror strstr strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

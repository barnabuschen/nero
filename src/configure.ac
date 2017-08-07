#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([neroMain], [1], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([neroMain.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([neroMain],[
    glib-2.0
])
PKG_PROG_PKG_CONFIG
#pthread为链接库名字， pthread_create为库中的一个函数
AC_CHECK_LIB(pthread, pthread_create)  
AC_CHECK_LIB(hiredis, redisCommand)  
AC_CHECK_LIB(m, sin)  
# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h malloc.h math.h netinet/in.h stdlib.h string.h unistd.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([ftruncate getcwd memset munmap strerror strstr gettimeofday memmove strcasecmp strtol ])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT


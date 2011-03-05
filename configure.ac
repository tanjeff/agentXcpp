# Very simple configure script input file.
# 

AC_INIT(agentXcpp, 0.1-alpha-dev)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/Gauge32.h])
AM_CONFIG_HEADER(config.h)

#AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_LANG(C++)
AC_CHECK_PROG([DOXYGEN], [doxygen], [doxygen])
AC_ARG_WITH([doxygen],
  [AS_HELP_STRING([--with-doxygen], [Use doxygen to generate API documentation.])],
  [case "$withval" in
    yes) test -z "$DOXYGEN" && AC_MSG_ERROR([doxygen not found.]) ;;
    no) DOXYGEN="" ;;
    *)   AC_MSG_ERROR([Bad value ${withval} for --with-doxygen.]) ;;
   esac])
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])

# Checks for libraries.
AC_PROG_LIBTOOL
LT_INIT
AC_CHECK_LIB(stdc++, main,,AC_MSG_ERROR(gdu requires libstdc++))

# Checks for header files.
AC_CHECK_HEADERS([boost/asio.hpp],,[AC_MSG_ERROR([cannot find Boost asio header files])])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([socket])

AC_CONFIG_FILES([Makefile 
                 src/Makefile
                 doc/Makefile])
AC_OUTPUT

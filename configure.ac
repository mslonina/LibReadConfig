#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([libreadconfig], [0.12], [mariusz.slonina@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/libreadconfig.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_PREFIX_DEFAULT(`pwd`/build)

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_C_INLINE
AC_TYPE_SSIZE_T
LT_INIT
AC_PROG_LIBTOOL

# Checks for HDF5 support.
AC_ARG_ENABLE([hdf], [  --enable-hdf enable hdf5 support], hdf=yes, hdf=no)

if test "x${hdf}" = xyes; then
  echo "------------------------------------------------------"
  echo "  Checking libraries needed for HDF5 support."
  echo "------------------------------------------------------"
  
  have_hdf5=no
  AC_SEARCH_LIBS([H5open], [hdf5], [have_hdf5=yes])

  if test "x${have_hdf5}" = xyes; then
    AC_CHECK_HEADERS([hdf5.h], [], [have_hdf5=no])
    echo "------------------------------------------------------"
    echo "  HDF5 was found on your system."
    echo ""

    # Check if HDF5 version >= 1.8.0
    have_hdf5_18=no
    AC_SEARCH_LIBS([H5Dopen2], [hdf5], [have_hdf5_18=yes])

    echo ""
    if test "x${have_hdf5_18}" = xyes; then
      have_hdf5=yes
      echo "  HDF5 support is enabled."
    else
      echo "  You need to have at least HDF5 1.8.0 installed!"
      (exit 1); exit 1
    fi
      echo "------------------------------------------------------"
  fi

  if test "x${have_hdf5}" = xno; then
    echo "------------------------------------------------------"
    echo "  HDF5 was not found on your system."
    echo "  HDF5 support is disabled."
    echo "------------------------------------------------------"
    hdf=no
  fi
fi

if test "x${hdf}" = xyes; then
  AC_DEFINE([HAVE_HDF5_SUPPORT], 1, [Define hdf5 support])
fi

# Check for doxygen program.
# if test -z "$DOXYGEN"; then
#   echo "-----------------------------------------"
#   echo " No Doxygen program found - continuing"
#   echo " without Doxygen documentation support."
#   echo "-----------------------------------------"
# fi
# AM_CONDITIONAL([HAVE_DOXYGEN],[test -n "$DOXYGEN"])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([memmove strcspn strspn strstr strtol])

AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 src/Makefile])
AC_OUTPUT

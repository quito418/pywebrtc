#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([pywebrtc], [0.0.0], [jemmons@cs.stanford.edu])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/libwebrtc_hl.hh])
AC_CONFIG_HEADERS([config.h])

VERSION_INFO=`echo $VERSION | sed -e 's/\./:/g'`
AC_SUBST(VERSION_INFO)

# include extra macros
AC_CONFIG_MACRO_DIRS([m4])
m4_include(m4/python.m4)

# Add picky CXXFLAGS
CXX14_FLAGS="-std=c++14"
PICKY_CXXFLAGS="-Wall -Wextra -Wno-non-virtual-dtor"
AC_SUBST([CXX14_FLAGS])
AC_SUBST([PICKY_CXXFLAGS])

# add flags for python wrapper compilation
PYWRAPPER_EXTRA_CFLAGS="-std=c++14 -Wall -Wextra -Weffc++"
PYWRAPPER_EXTRA_LDFLAGS=""
AC_SUBST([PYWRAPPER_EXTRA_CFLAGS])
AC_SUBST([PYWRAPPER_EXTRA_LDFLAGS])

# Checks for programs.
AC_PROG_CXX
AM_PROG_LIBTOOL

# Checks for libraries.
PKG_CHECK_MODULES([LIBWEBRTC], [LibWebRTC])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# define macros for checking python libraries

# check for python libraries
AC_PATH_PROG([PYTHON],[python3])
AX_PYTHON_VERSION([3.5.0])

AX_PYTHON_MODULE_VERSION([setuptools], [20.7.0], [python3])
AX_PYTHON_MODULE_VERSION([wheel], [0.29.0], [python3])

AC_CONFIG_FILES([Makefile 
         src/Makefile
         python/Makefile
])

AC_OUTPUT

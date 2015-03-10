# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# $Id: configure.ac,v 1.1 2005/12/14 01:48:47 oliver Exp $

# This file is part of pam_dotfile.
#
# pam_dotfile is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# pam_dotfile is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with pam_dotfile; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.

AC_PREREQ(2.59)
AC_INIT([pam_nis], [0.1], [oliver.kurth@cyclades.com])
AC_CONFIG_SRCDIR([src/pam_nis.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign -Wall])

AM_DISABLE_STATIC

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_CXX

# If using GCC specifiy some additional parameters
if test "x$GCC" = "xyes" ; then
   CFLAGS="$CFLAGS -pipe -Wall"
fi

#AC_SUBST(PACKAGE_URL, [http://0pointer.de/lennart/projects/pam_dotfile/])

CFLAGS="$CFLAGS -L/lib"

# Checks for libraries.
AC_CHECK_HEADER([security/pam_modules.h],, [AC_MSG_ERROR([*** Sorry, you have to install the PAM development files ***])])

LIBS="$LIBS -ldl -lpam -lpam_misc -lnsl -lcrypt"

case "$host" in
  *-*-linux*)
    PAM_MODDIR="/lib/security"
    ;;
  *)
    PAM_MODDIR="/usr/lib"
    ;;
esac
AC_SUBST(PAM_MODDIR)

# Checks for header files.

AC_CHECK_FUNCS([dup2 memset strchr strerror strrchr])
AC_FUNC_FORK
AC_FUNC_LSTAT
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_VPRINTF

AC_CHECK_HEADERS([fcntl.h limits.h syslog.h termios.h])
AC_HEADER_STDC
AC_HEADER_SYS_WAIT

AC_C_CONST

AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIGNAL
AC_TYPE_UID_T

# XMLTOMAN manpage generation
AC_ARG_ENABLE(xmltoman,
        AC_HELP_STRING([--disable-xmltoman], [Disable rebuilding of man pages with xmltoman]),
[case "${enableval}" in
  yes) xmltoman=yes ;;
  no)  xmltoman=no ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --disable-xmltoman]) ;;
esac],[xmltoman=yes])

if test x$xmltoman = xyes ; then
   AC_CHECK_PROG(have_xmltoman, xmltoman, yes, no)

   if test x$have_xmltoman = xno ; then
     AC_MSG_WARN([*** Not rebuilding man pages as xmltoman is not found ***])
     xmltoman=no
   fi
fi

AM_CONDITIONAL([USE_XMLTOMAN], [test "x$xmltoman" = xyes])

AC_ARG_ENABLE(xcrypt,
        AC_HELP_STRING([--enable-xcrypt], [enable libxcrypt support]),
[case "${enableval}" in
  yes) xcrypt=yes ;;
  no)  xcrypt=no ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-xcrypt]) ;;
esac],[xcrypt=no])

if test x$xcrypt = xyes ; then
   AC_CHECK_LIB(xcrypt, xcrypt,, AC_MSG_ERROR("libxcrypt not found"))

fi

AC_SUBST(HAVE_XCRYPT)
AC_SUBST(XCRYPT_LIBS)

AC_CONFIG_FILES([src/Makefile Makefile man/Makefile])
AC_OUTPUT

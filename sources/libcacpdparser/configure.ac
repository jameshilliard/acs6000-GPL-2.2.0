AC_PREREQ(2.59)
AC_INIT(libcacpdparser, 0.0.1)
AC_CONFIG_SRCDIR([src/cacpdparser.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE()
AC_PROG_LIBTOOL()

AC_PROG_CC

PKG_CHECK_MODULES(DAEMON, [libdaemon])
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(GLIB, [glib-2.0])
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

AC_ARG_ENABLE(devbuild,
   AC_HELP_STRING([--enable-devbuild], [enable development build support]),
   if test "x$enable_tls" = 'xyes'; then
      devbuild_enabled='yes'
   fi,
      devbuild_enabled='no'
)

AC_ARG_ENABLE(eval-command,
	[AS_HELP_STRING([--enable-eval-command], [enable the EVAL and FUNCLIST commands])],
	if test "x$enable_eval_command" = 'xyes'; then
		AC_DEFINE(ENABLE_EVAL_COMMAND, 1, [Enable the EVAL and FUNCLIST commands])
	fi
)

AM_CONDITIONAL(DEVBUILD, test "x$devbuild_enabled" = xyes )

if test -n "$CACPD_SOCKET_PATH" ; then
	AC_DEFINE_UNQUOTED(CACPD_SOCKET_PATH, "$CACPD_SOCKET_PATH", [path to CACPD listening socket])
fi

AC_CONFIG_FILES([Makefile libcacpdparser.pc src/Makefile include/Makefile include/libcacpdparser/Makefile])
AC_OUTPUT

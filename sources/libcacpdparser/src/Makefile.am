lib_LTLIBRARIES = libcacpdparser.la

noinst_PROGRAMS=test_login 

libcacpdparser_la_SOURCES=\
	cacpdparser.c \
	commands_priv.h \
	connection.c \
	connection_priv.h \
	error.c \
	response.c \
	transaction.c \
	transaction_priv.h \
	urlcoding.c

test_login_SOURCES=\
	test_login.c \
	libcacpdparser.la

#response_ut_SOURCES=\
#	response_ut.c

LDADD=@GLIB_LIBS@
test_login_LDADD= libcacpdparser.la @DAEMON_LIBS@ @GLIB_LIBS@ -ldlog -lgsp_auth -lpam -lpam_misc -ldl -lcyc -lcyc++

AM_CFLAGS=@GLIB_CFLAGS@ -Wall -Werror -I$(top_srcdir)/include/

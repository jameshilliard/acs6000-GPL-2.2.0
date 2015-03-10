# $Id: Makefile.am,v 1.2 2006/01/06 23:44:36 oliver Exp $
#
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

AM_CFLAGS = -DSBINDIR=\"@sbindir@\" -fno-strict-aliasing

moduledir = @PAM_MODDIR@
module_LTLIBRARIES = pam_nis.la

pam_nis_la_SOURCES = pam_nis.c log.c log.h common.c common.h
pam_nis_la_LDFLAGS = -module -avoid-version
pam_nis_la_CFLAGS = $(AM_CFLAGS)

sbin_PROGRAMS = pam-nis-helper
noinst_PROGRAMS = pamtest

pam_nis_helper_SOURCES = pam-nis-helper.c common.c common.h log.c log.h 
pam_nis_helper_CFLAGS = $(AM_CFLAGS)

pamtest_SOURCES = pamtest.c

#install-exec-hook:
#	chown root $(DESTDIR)$(sbindir)/pam-nis-helper
#	chmod u+s $(DESTDIR)$(sbindir)/pam-nis-helper

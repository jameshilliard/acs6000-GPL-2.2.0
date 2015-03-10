# $Id: Makefile.am,v 1.1 2005/12/14 01:48:47 oliver Exp $
#
# This file is part of pam_nis.
#
# pam_nis is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# pam_nis is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with pam_nis; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.

man_MANS = pam-nis-helper.8

EXTRA_DIST = $(man_MANS) pam-nis-helper.8.xml.in

if USE_XMLTOMAN

CLEANFILES = $(man_MANS)

pam-nis-helper.8: pam-nis-helper.8.xml.in Makefile
	sed -e 's,@sysconfdir\@,$(sysconfdir),g' -e 's,@sbindir\@,$(sbindir),g' -e 's,@PACKAGE_URL\@,$(PACKAGE_URL),g' --e 's,@PACKAGE_BUGREPORT\@,$(PACKAGE_BUGREPORT),g' $< | xmltoman - > $@

endif

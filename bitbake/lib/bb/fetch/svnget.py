#!/usr/bin/env python
# ex:ts=4:sw=4:sts=4:et
# -*- tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*-
"""
BitBake 'Fetch' implementations

Classes for obtaining upstream sources for the
BitBake build tools.

Copyright (C) 2006 Jiff Shen

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA. 

Based on functions from the base bb module, Copyright 2003 Holger Schurig
"""

import os, re
import bb
from   bb import data
from   bb.fetch import Fetch
from   bb.fetch import FetchError
from   bb.fetch import MissingParameterError

class SvnGet(Fetch):
    """Class to fetch a module or modules from svn repositories"""
    def supports(url, d):
        """Check to see if a given url can be fetched with svn.
           Expects supplied url in list form, as outputted by bb.decodeurl().
        """
        (type, host, path, user, pswd, parm) = bb.decodeurl(data.expand(url, d))
        return type in ['svnget']
    supports = staticmethod(supports)

    def localpath(url, d):
        (type, host, path, user, pswd, parm) = bb.decodeurl(data.expand(url, d))
        if "localpath" in parm:
#           if user overrides local path, use it.
            return parm["localpath"]

        if 'rev' in parm:
            revision = parm['rev']
        else:
            revision = ""

        url = bb.encodeurl([type, host, path, user, pswd, {}])

        return os.path.join(data.getVar("DL_DIR", d), os.path.basename(url))
    localpath = staticmethod(localpath)

    def go(self, d, urls = []):
        """Fetch urls"""
        if not urls:
            urls = self.urls

        localdata = data.createCopy(d)
        data.setVar('OVERRIDES', "svn:%s" % data.getVar('OVERRIDES', localdata), localdata)
        data.update_data(localdata)

        for loc in urls:
            bb.note ("fetch %s" % loc)
            (type, host, path, user, pswd, parm) = bb.decodeurl(data.expand(loc, localdata))

            bb.debug (2, "type %s, host %s, path %s, user %s, paswd %s" % (type, host, path, user, pswd))
            options = []
            if 'rev' in parm:
                revision = parm['rev']
            else:
                revision = ""

            if "proto" in parm:
                proto = parm["proto"]
            else:
                proto = "svn"

            svn_rsh = None
            if proto == "svn+ssh" and "rsh" in parm:
                svn_rsh = parm["rsh"]

            dlfile = self.localpath(loc, localdata)
            dlfile = data.expand(dlfile, localdata)

            dldir = data.getVar('DL_DIR', localdata, 1)
            dldir = data.expand(dldir, localdata)

            md5 = dlfile + '.md5'
            if os.path.exists(md5):
                continue

            infocmd = "svn info %s://%s%s" % (proto, host, path)
            bb.debug (2, "svn info command %s" % infocmd)
            
            hinfo = os.popen (infocmd, "r")
            lines = hinfo.readlines()
            hinfo.close()
            
            regex = re.compile (r"""^Node Kind:\s+file""")
            for line in lines:
                if regex.match (line):
                    break
            else:
                raise FetchError ("not found or not a regular file: " + loc)
            
            bb.debug (1, loc + " is a regular file")

            olddir = os.path.abspath(os.getcwd())
            os.chdir(data.expand(dldir, localdata))

            svncmd = "svn cat %s://%s%s > %s" % (proto, host, path, dlfile)

            # either use the revision or if SRCDATE is now no braces
            if revision:
                svncmd = "svn cat -r %s %s://%s%s > %s" % (revision, proto, host, path, dlfile)

            if svn_rsh:
                svncmd = "svn_RSH=\"%s\" %s" % (svn_rsh, svncmd)

            bb.debug(1, "Running %s" % svncmd)
            myret = os.system(svncmd)
            if myret != 0:
                try:
                    os.remove(dlfile)
                except OSError:
                    pass
                raise FetchError(loc)
            
            os.system ("md5sum %s > %s" % (os.path.basename(dlfile), md5))
            os.chdir(olddir)
        del localdata

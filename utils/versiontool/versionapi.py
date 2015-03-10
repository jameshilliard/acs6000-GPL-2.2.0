#!/usr/bin/env python
#
# This program is a api for project packages scripts.
#
# Author: Tercel Yi
# History: Draft on , 2008-3-7
#
# @Copyright Avocent

import os
import sys
import optparse

def getPackageVersionName(filepath,version_dir):
    import lib.projectversionsdata as projectversionsdata
    revision_number = ''
    try :
        objData = projectversionsdata.ProjectVsersionData(filepath)
        active_version = objData.getActiveVersionName()
        package_list = objData.getOneVersionPackages(active_version)  
        for package_dict in package_list:
            if version_dir== package_dict['dir']:
                revision_number = package_dict['revision_number']
                break
    except :
        revision_number = ''
        
    print revision_number

#===============================================
          

def main():
    usage = "python versionapi.py [ -f <filename>][ -p <sourcepath>] "
    parser = optparse.OptionParser(usage)
    parser.add_option("-f","--file",
                      action="store",
                      type="string",
                      dest="filepath",
                      help="the project file for version")
    parser.add_option("-p","--sourcepath",
                      action="store",
                      type="string",
                      dest="sourcepath",
                      help="the package source path")

    (opts, args) = parser.parse_args(sys.argv)   

    if len(args) < 1: 
        print "Type ' versionapi.py --help' for usage."
        print usage
        sys.exit(1)
    
    if opts.sourcepath != '':
        if opts.filepath == None or opts.filepath == '':
            print "Please type file path."
            print "Type ' versionapi.py --help' for usage."
            print usage
            sys.exit(1)
        
    if opts.filepath != '' and opts.sourcepath != '':
        getPackageVersionName(opts.filepath,opts.sourcepath)

if __name__ == '__main__':
    main()


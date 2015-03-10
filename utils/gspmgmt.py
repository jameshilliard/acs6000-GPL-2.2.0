#!/usr/bin/python

# This program is for GSP management, user can use it to do list and create branches or tags.
#
# Author: Ice Liu
# History: Draft on Jun 18, 2007
#          To support tag and branch base.

import os
import sys
import string
import optparse
import gsputil

Usage = """
\t ./gspmgmt.py <--test> <-r tags/TAG|branches/branchname> <-d tags/TAG|branches/branchname> <-c TAG1:TAG2> -t TAG -b branch -m messages -l modulename/all -q
"""
parser = optparse.OptionParser(Usage)
parser.add_option("-r",
                  "--revision",
                  action="store",
                  type="string",
                  dest="revision",
                  help="the revision to base on tags/TAG or branches/branchname")
parser.add_option("-t",
                  "--tag",
                  action="store",
                  type="string",
                  dest="tag",
                  help="the tag to make")
parser.add_option("-b",
                  "--branch",
                  action="store",
                  type="string",
                  dest="branch",
                  help="the branch to make")
parser.add_option("-d",
		  "--delete",
		  action="store",
		  type="string",
		  dest="delete",
		  help="to delete a tag")
parser.add_option("-c",
		  "--compare",
		  action="store",
		  type="string",
		  dest="compare",
		  help="to diff between 2 tags/branches.")
parser.add_option("-m",
                  "--message",
                  action="store",
                  type="string",
                  dest="message",
                  help="the commit log message")
parser.add_option("-l",
                  "--list",
                  action="store_true",
                  dest="list",
                  help="to list all the TAGs/branches of a project")
parser.add_option("--test",
                  action="store_true",
                  dest="test",
                  help="to only test the commands")
parser.add_option("-q",
		  "--quite",
		  action="store_true",
		  dest="quiet",
		  help="Do not prompt.")

(opts, args) = parser.parse_args(sys.argv)


def toconfirm(prompt):
    if opts.quiet:
	return True
    reply = raw_input("Confirm to "+prompt+"(yes/no):")
    if "yes" == reply or "y" == reply:
	return True

if len(args) < 2:
    print "modulename/all argument not provided"
    print Usage
    sys.exit(1)
modname = args[1]
if not gsputil.isvalid(modname):
    print modname," is not a valid module"
    print Usage
    sys.exit(2)

modules = gsputil.getmods(modname).split()

if opts.list:
    print "To list all the TAGs/branches of project/module", modname, ":"
    for module in modules:
        print "svn list ",gsputil.SVN_LOCAL_URL+module
        print "tags:"
        if opts.test:
            print "svn list "+gsputil.SVN_LOCAL_URL+module+"/tags"
        else:
            os.system("svn list "+gsputil.SVN_LOCAL_URL+module+"/tags")
        print "branches:"
        if opts.test:
            print "svn list "+gsputil.SVN_LOCAL_URL+module+"/branches"
        else:
            os.system("svn list "+gsputil.SVN_LOCAL_URL+module+"/branches")
    sys.exit(0)
elif opts.compare:
    if -1 == opts.compare.find(":") or (not opts.compare.startswith("tags") and not opts.compare.startswith("branches")):
	print "TAGS/branches format error, use T1:T2"
	print Usage
	sys.exit(4)
    tag1,tag2 = opts.compare.split(":")
    for module in modules:
	extraopt = ""
	if "bitbake" == module:
	    extraopt = "/bitbake-1.6.2"
	cmd = "svn diff "+gsputil.SVN_LOCAL_URL+module+"/"+tag1+extraopt+" "+gsputil.SVN_LOCAL_URL+module+"/"+tag2+extraopt
	print cmd
	if not opts.test:
	    os.system(cmd)
    sys.exit(0)
elif opts.delete:
    if not opts.delete.startswith("tags") and not opts.delete.startswith("branches"):
	print "please use tags/TAGNAME or branches/branchname"
	sys.exit(3)
    print "To delete the TAG/branch for the project/module",modname,":"

    if None == opts.message:
        print "message not provided"
        print Usage
        sys.exit(2)

    for module in modules:
	cmd = "svn delete "+gsputil.SVN_URL+module+"/"+opts.delete+" -m\""+opts.message+"\""
	if opts.test:
	    print cmd
	else:
	    if toconfirm("delete Tag/branch "+opts.delete+" of module "+module):
	        os.system(cmd)
    sys.exit(0)
else:
    # If not list, there should be a TAG or branch option.
    if None == opts.tag and None == opts.branch:
        print "TAG/branch option not provided"
        print Usage
        sys.exit(1)
    if None == opts.message:
        print "message not provided"
        print Usage
        sys.exit(2)
    if None != opts.tag:
        tagoption = "/tags/"+opts.tag
    else:
        tagoption = "/branches/"+opts.branch
    srcrev = "trunk"
    rev = opts.revision
    if None != rev:
        if rev.startswith("tags") or rev.startswith("branches"):
            srcrev = opts.revision
        else:
            print "wrong -r(--revision) format, use tags/TAG or branches/branchname"
            print Usage
            sys.exit(3)
    
    for module in modules:
        extraopt = ""    
	if "bitbake" == module:
	    extraopt = "/bitbake-1.6.2"
	    print "svn mkdir "+gsputil.SVN_URL+module+tagoption+" -m \""+opts.message+"\""
	    if not opts.test:
	        if toconfirm("svn mkdir "+gsputil.SVN_URL+module+tagoption+" -m \""+opts.message+"\""):
		    os.system("svn mkdir "+gsputil.SVN_URL+module+tagoption+" -m \""+opts.message+"\"")		
        cmd = "svn copy "+gsputil.SVN_URL+module+"/"+srcrev+extraopt+" "+gsputil.SVN_URL+module+tagoption+extraopt+" -m \""+opts.message+"\""
        print cmd
        if not opts.test:
            if toconfirm("make tag for "+module+"/"+srcrev+extraopt+" to "+module+tagoption+extraopt):
                os.system(cmd)
            else:
                pass


print "Bye."
sys.exit(0)

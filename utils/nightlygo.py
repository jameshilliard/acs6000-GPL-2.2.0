#!/usr/bin/python

import os
import sys
import os.path
import shutil
import string
import time
import osa_util
import osa_build_util
import re


module_name = "caesar_all"
root_dir = os.path.abspath('.')
curdir = ''
MAKE = 'nmake'
conf_file = 'bldconf.win'
REDIRECTION = ''
if None != os.getenv('HOME'):
    REDIRECTION = ' >> '+os.getenv('HOME')+os.sep+'build_result_caesar.txt 2>&1'
if sys.platform == 'linux2':
    curdir = './'
    MAKE = 'make'
    conf_file = 'bldconf.lnx'


# To clean the log file
def clean_log():
    if None != os.getenv('HOME'):
        if os.path.exists(os.getenv('HOME')+os.sep+'build_result_caesar.txt'):
	    os.remove(os.getenv('HOME')+os.sep+'build_result_caesar.txt')
        if os.path.exists(os.getenv('HOME')+os.sep+'osa_building.log'):
            os.remove(os.getenv('HOME')+os.sep+'osa_building.log')
    if os.path.exists(module_name+'.log'):
        os.remove(module_name+'.log')


# If user want to remove old copy before check out, use this option.
rm_lst = [root_dir+os.sep+'source',
          root_dir+os.sep+'thirdparty']
# To clean the outputs and source codes
def clean_all():
    clean_log()
    for to_remove in rm_lst:
	if os.path.exists(to_remove):
	    osa_util.myrmdir_recursive(to_remove)    


# To use a special tag and a date mark for nightly build
t=time.localtime()
tstr = str(t.tm_year)+str(t.tm_mon)+str(t.tm_mday)
# To generate the version.h
def mk_version_h(g):
    o = open("version.h","w")
    o.write("#ifndef __VERSION_H__\n")
    o.write("#define __VERSION_H__\n")
    o.write("/**\n")
    o.write(" * Host type:\n")
    o.write(" * i686: 0\n")
    o.write(" * powerpc: 1\n")
    o.write(" */\n")
    # To define a variable to indicate what current architecture is, default is powerpc on this branch
    hostcode = "1"
    host = os.getenv("HOST")
    if None != host:
	if "i686" == host:
	    hostcode = "0"
    tl = time.localtime()
    o.write("#define HOST_TYPE "+hostcode+"\n")
    o.write("#define MAJOR_VERSION 4\n")
    o.write("#define MINOR_VERSION 1\n")
    o.write("#define RELEASE_VERSION 0\n")
    o.write("#define BUILD_VERSION "+tstr+"\n")
    o.write("#endif\n")
    o.close()
    v = open("version","w")
    version = "MergePoint Application version: 4.1.0."+tstr
    v.write(version+"\n")
    v.close()
        

# If user run this way "python go.py ...", then remove the argument of python
if 'python' in sys.argv:
    sys.argv.remove('python')


# If user run in other direcoty, then change to this directory firstly.
pre_path = os.path.dirname(sys.argv[0])
if '' != pre_path:
    os.chdir(pre_path)
else:
    pass


osa_option = osa_build_util.OSA_Option()
osa_parser = osa_option.options
(opts,args) = osa_parser.parse_args(sys.argv)


#if opts.forceremove:
#    clean_all()


verbose_opt = ""
if opts.verbose:
    verbose_opt = " -V"
    REDIRECTION = ""

if opts.redhat:
    os.system('sh chglobal.sh')

os.system('sh setenv.sh')
clean_log()


if os.path.exists('CVS'+os.sep+'Tag'):
    f = open('CVS'+os.sep+'Tag')
    line = f.readline()
    line = line.replace('\n','')
    # to re-generate the version.h
    patobj = re.compile("(\w+)_(\w+)_(\w+)_(\d+)_(\d+)_(\d+)_(\d+)")
    # release tag or branch
    r_patobj = re.compile("(\w+)_(\w+)_(\w+)_(\d+)_(\d+)_(\d+)")
    matchobjs = patobj.search(line)
    if None == matchobjs:
        r_matchobjs = r_patobj.search(line)
        mk_version_h([])
    else:
        mk_version_h(matchobjs.groups())
else:
    l = ["BLD","MPG","NIGHTLY","1", "0", "1", tstr]
    mk_version_h(l)
    #os.remove("version.h")
    #os.system("cvs up version.h")
    #os.system('echo "Caesar version:0.0.0.0">version')
# copy version.h to common/include
shutil.copy2('version.h','source'+os.sep+'common'+os.sep+'include')

os.system(curdir+'gen_err_struct.py')
print 'Preprocess done, begin to build ...'

os.system(MAKE+' -f '+conf_file+REDIRECTION)
print "Done."


sys.exit(0)


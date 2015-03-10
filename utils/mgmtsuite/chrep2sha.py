#! /usr/bin/env python
#T his program is for changing the repository from https://fre-svn/ to https://sha-svn
# then changing from https/svn://sha-svn/svn/avocent-gsp to https/svn://sha-svn/svn/avocent-sha
#

import os
import os.path
import shutil
import stat

# 
def updatesvninfo (fn):
	input_file = open(fn, "rb")
	output_file = open(fn+".tmp", "wb")
	for s in input_file:
		s = s.replace("svn://sha-scm.corp.avocent.com/avocent-gsp", "https://sha-svn.corp.avocent.com/svn/avocent-gsp")
		s = s.replace("fre-svn", "sha-svn")
		# for windows platform, replace the '\r\n' with '\r', or svn command will be failed
		if '\r\n' == s[-2:]:
			s = s[:-2]+s[-1:]
		output_file.write(s)
	output_file.close()
	input_file.close()
	os.chmod(fn, stat.S_IWRITE)
	os.remove(fn)
	shutil.move(fn+".tmp", fn)

for root, dirs, files in os.walk(os.getcwd()):
	for f in files:
		if   f == "entries":
			#print "root:", root
			#print "dirs:", dirs
			print "files:", os.path.join(root, f)
			updatesvninfo(os.path.join(root, f))



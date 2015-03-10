#!/usr/bin/env python
import os
import sys

syscmd = 'svn update'
stdoutfd = os.popen( syscmd )       
diffs = ''       
for diff in stdoutfd.readlines():          
    diffs += diff 
    diff.strip()
print diffs

import lib.datacommon    
lib.datacommon.doSvnUpdateResultInfo(diffs)            
import versionmain
versionmain.main()

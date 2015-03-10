##
# This program is common function
#
# Author: Tercel Yi
# History: Draft on , 2008-2-25
#
# @Copyright Avocent

import os
import sys
import string
import time
import re

from config import *

g_head_revision= '0'
g_svnupdateresultinfo = ''
def doSvnUpdateResultInfo(info):
    global g_head_revision
    global g_svnupdateresultinfo
    g_svnupdateresultinfo = info
    if info!='' :
        info_list = info.split()
        revision_list =  re.findall('\d+',info_list[len(info_list)-1])
        if revision_list:
            g_head_revision = revision_list[0]
            
def doSvnUpdate():
    syscmd = 'svn update '+DEFAULT_DATA_DIR
    stdoutfd = os.popen( syscmd )       
    diffs = ''       
    for diff in stdoutfd.readlines():          
        diffs += diff 
        diff.strip()
    doSvnUpdateResultInfo(diffs)
    
def getProjectVersionXmlPath(projectxml):
    
    path = DEFAULT_DATA_DIR+'/'
    r= re.compile('\\\\')
    projectxml = r.sub('/',projectxml)
    projectxml_list = projectxml.split('/')
    filename = projectxml_list[len(projectxml_list)-1]
    for i in range(len(filename)-4): 
        path += filename[i] 
     
    path += '_versions'+DEFAULT_DATA_SUFFIX

    return path

def getProjectVersionTxtPath(projectxml):
    path = DEFAULT_DATA_DIR+'/'
    r= re.compile('\\\\')
    projectxml = r.sub('/',projectxml)
    projectxml_list = projectxml.split('/')
    filename = projectxml_list[len(projectxml_list)-1]
    for i in range(len(filename)-4): 
        path += filename[i] 
        
    path += '_versions'+DEFAULT_TXT_SUFFIX
    
    return path

def getProjectVersionData(filepath):
    dict = {'version_list':[], 'version_name_list':[], 'version_active':''}
    try :
        import lib.projectversionsdata as projectversionsdata
        objData = projectversionsdata.ProjectVsersionData(filepath)  
        version_list = objData.getVersions()
        dict['version_list'] = version_list
        dict['version_name_list'] = projectversionsdata.getVersionNameList(version_list)
        dict['version_active'] = objData.getActiveVersionName() 
        objData.Destory()  
    except :
        #print "open ' "+filepath+"' failed"
        pass
    
    return dict

def getOneProjectVersion(version_list, name):

    import lib.projectversionsdata as projectversionsdata
    dict = projectversionsdata.getOneVersion(version_list, name)
    return dict

def getOnePackageVersion(version_list, name):

    import lib.packagedata as packagedata
    dict = packagedata.getOneVersion(version_list, name)
    return dict

def getPackageData(filepath):
    dict = {'version_list':[], 'version_name_list':[], 'version_active':''}
    try :
        import lib.packagedata as packagedata
        objData = packagedata.PackageData(filepath)
        version_list = objData.getVersions()
        dict['version_list'] = version_list
        dict['version_name_list'] = packagedata.getVersionNameList(version_list)
        dict['version_active'] = objData.getActiveVersionName()   
        objData.Destory()
    except :
        #print "open ' "+filepath+"' failed"
        pass
    
    return dict

def checkProjectVersionName(version_name_list,name):
    import lib.projectversionsdata as projectversionsdata
    return projectversionsdata.getVersionNameIsExist(version_name_list,name)
    
def checkPackageName(version_name_list,name):
    import lib.packagedata as packagedata
    return packagedata.getVersionNameIsExist(version_name_list,name)

def addProjectVersionData(filepath,name,note,isactive,package_list,package_version,package_scope):
    import lib.projectversionsdata as projectversionsdata
    objData = projectversionsdata.ProjectVsersionData(filepath)
    objData.addData(name,note,isactive,package_list,package_version,package_scope) 
    objData.Destory()  

def addPackageData(filepath,name,note,isactive):
    import lib.packagedata as packagedata
    objData = packagedata.PackageData(filepath)
    objData.addData(name,note,isactive) 
    objData.Destory()
    
def setProjectVersionData(name,filepath):
    import lib.projectversionsdata as projectversionsdata
    objData = projectversionsdata.ProjectVsersionData(filepath)
    objData.setData(name) 
    objData.Destory()  

def setPackageData(name,filepath):
    import lib.packagedata as packagedata
    objData = packagedata.PackageData(filepath)
    objData.setData(name) 
    objData.Destory()

def getActivePackages(package_list) :
    import lib.packagedata as packagedata
    package_avtive_dict = {}
    for dic in package_list:
        filepath = DEFAULT_PACKAGESDATA_DIR+'/'+dict['dir']+DEFAULT_DATA_SUFFIX

        try :
            objData = packagedata.PackageData(filepath)
            name = objData.getActiveVersionName()  
            objData.Destory() 
       
        except :
            #print "open ' "+filepath+"' failed"
            pass
            
    return 

def getActiveVersionPackages(package_list) :
    version_package_list = []
    for dict in package_list :
        package_dict = {}
        package_dict['name'] = dict['name']
        package_dict['dir'] = dict['dir']
        packagefile = DEFAULT_PACKAGESDATA_DIR+'/'+dict['dir']+DEFAULT_DATA_SUFFIX
        try :
            import lib.packagedata as packagedata
            objData = packagedata.PackageData(packagefile)
            revision = objData.getActiveVersionName()
            objData.Destory()   
            if revision!='' :
                package_dict['revision_number'] = revision
            else:
                package_dict['revision_number'] = g_head_revision
        except :
            package_dict['revision_number'] = g_head_revision
            #print "open ' "+packagefile+"' failed"
            pass
            
        version_package_list.append(package_dict)
            
    return version_package_list

def getOneProjectVersionPackages(version_name,filepath):

    import lib.projectversionsdata as projectversionsdata
    objData = projectversionsdata.ProjectVsersionData(filepath)
    version_package_list = objData.getOneVersionPackages(version_name)
    objData.Destory()  
        
    return version_package_list

def addTxtData(filepath,name,note,package_list,package_version='',package_scope=0):
      
        versionstr = ""

        versionstr += "Version number: "+name+ "\n"
        versionstr += "Time: "+time.asctime() +"\n"
        versionstr += "Comment: " +note +"\n"
        versionstr += "Packages: \n"
        versionstr += "-"*120+"\n"
        
        #package list
        for dict in package_list :
            if package_scope==1 :
                versionstr += dict['dir']+": "+package_version+"\n"
            else :
                versionstr += getPackageTxtInfo(dict,package_version)  
            
        versionstr += "="*120+"\n\n"
        
        fout = open(filepath,"a")
        fout.write(versionstr)
        fout.close()
        
def getPackageTxtInfo(dict,package_version):
    package_str = ''
    packagefile = DEFAULT_PACKAGESDATA_DIR+'/'+dict['dir']+DEFAULT_DATA_SUFFIX
    try :
        import lib.packagedata as packagedata
        objData = packagedata.PackageData(packagefile)
        revision = objData.getActiveVersionName()
        objData.Destory()  
        if revision!='' :
            package_str += dict['dir']+": "+revision+"\n"
        else :
            package_str += dict['dir']+": "+package_version+"\n"
   
    except :
        package_str += dict['dir']+": "+package_version+"\n"
        #print "open ' "+packagefile+"' failed"
        
    return package_str
        
def createProjectVersionsFile(filepath,name='',manager=''):
      
        versionstr = "<?xml version=\"1.0\"?>\n"
        versionstr += "<Project name=\""+name+"\">\n"
        versionstr += "	<Active version_name=\"\"/>\n"
        versionstr += "	<Versions>\n"
        versionstr += "	</Versions>\n"
        versionstr += "</Project>\n"

        fout = open(filepath,"w")
        fout.write(versionstr)
        fout.close()


def createPackageVersionFile(filepath,name='',manager=''):
      
        versionstr = "<?xml version=\"1.0\"?>\n"
        versionstr += "<Package name=\""+name+"\"  manager=\""+manager+"\">\n"
        versionstr += "	<Description>\n"
        versionstr += "	</Description>\n"
        versionstr += "	<Active version_name=\"\"/>\n"
        versionstr += "	<Versions>\n"
        versionstr += "	</Versions>\n"
        versionstr += "</Package>\n"
        
        fout = open(filepath,"w")
        fout.write(versionstr)
        fout.close()
        

        

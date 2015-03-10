##
# This program is a api for project packages scripts.
#
# Author: Tercel Yi
# History: Draft on , 2008-1-30
#
# @Copyright Avocent

import xml.dom.minidom
import dircache
import os
import os.path
import string
import re
import time
import sys

#  

class ProjectData:
    
    def __init__ (self,filepath) :
         
        self.filepath = filepath
        self.dom = None
        self.package_dict = {} 
        self.treelist = []
        self.group_list = []
        self.package_list = []
        self.name = ''
        self.manager = ''
        self.local = ''
        self.remote = ''
        self.svnsync = ''
        self.isok = False
        self.errormsg = ''

        if not os.path.exists(filepath):
            print filepath,"not exitst!"
            return
            #sys.exit(1)    
        try:
            self.dom = xml.dom.minidom.parse(filepath)  
        except :
            print 'Please open valid  xml file!'
            return

        b_getinfo = self.getInfo()
        b_getmodules = self.getModules()
        if b_getinfo and b_getmodules:
            self.isok = True

        self.dom.unlink()
    

    def Destory(self):
        self.dom = None
        self.filepath = ''
        self.package_dict = {} 
        self.treelist = []
        self.group_list = []
        self.package_list = []
        self.name = ''
        self.manager = ''
        self.local = ''
        self.remote = ''
        self.svnsync = ''
        self.isok = None
        
    def showErrorMsg():
        print self.errormsg
               
    def getInfo(self): 
        try:
            ProjectNode = self.dom.getElementsByTagName("Project")[0] 
            self.name = ProjectNode.getAttribute("name")
            self.manager = ProjectNode.getAttribute("manager")
            descriptionNode = self.dom.getElementsByTagName("Description")[0]
            self.description = descriptionNode.childNodes[0].data.strip()
            serverNode = self.dom.getElementsByTagName("Server")[0]
            objLocal =  serverNode.getElementsByTagName("Local")[0]
            self.local =objLocal.childNodes[0].data.strip()
            objRemote = serverNode.getElementsByTagName("Remote")[0]
            self.remote =objRemote.childNodes[0].data.strip()   
            objSvnSync = serverNode.getElementsByTagName("SvnSync")[0]
            self.svnsync =objSvnSync.childNodes[0].data.strip()   
            return True
        except : 
            print 'Parse Project Info Error:'
            return False

    def getModules(self):
        try:
            objModules =  self.dom.getElementsByTagName("Modules")[0]
            objPackages = objModules.getElementsByTagName("Module")

            for objPackage in objPackages:   
                package_dict = {}  
                package_dict['name'] = objPackage.getAttribute("name").strip()
                package_dict['gourp'] = objPackage.getAttribute("gourp").strip()
                package_dict['manager'] = objPackage.getAttribute("manager").strip()
                package_dict['dir'] = objPackage.childNodes[0].data.strip()   
                self.package_list.append(package_dict)
                
            return True
        except:
            print 'Parse Modules Error:' 
            return False   


    def getOneModule(self,name):
        module_dict = {}
        for dict in self.module_list:
            if dict['name']==name :
                module_dict = dict
                break
        return module_dict 
    
    def getPackageNameList(self):
        list = []
        for dict in self.package_list:
            list.append(dict['name'])
        return list
    
    def getOnePackage(self,name):
        package_dict = {}
        for dict in self.package_list:
            if dict['name']==name :
                package_dict = dict
                break
        return package_dict

#----------------------------------------------------------
def getOneModule(module_list,name):
    module_dict = {}
    for dict in module_list:
        if dict['name']==name :
            module_dict = dict
            break
    return module_dict    

def getOnePackage(package_list,name):
    package_dict = {}
    for dict in package_list:
        if dict['name']==name :
            package_dict = dict
            break
    return package_dict


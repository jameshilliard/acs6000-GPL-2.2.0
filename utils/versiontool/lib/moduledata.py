##
# This program is a api for module packages scripts.
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
class ModuleData:
    
    def __init__ (self,filepath) :
        
        if not os.path.exists(filepath):
            print filepath,"not exitst!"
            sys.exit(1)    
        dom =  xml.dom.minidom.parse(filepath)
        self.filepath = filepath
        self.dom = dom
        self.package_dict = {} 
    
    def setPackageDic (self,key,value) :
        self.package_dict[key] = value
  
    def _destory(self):
        self.dom.unlink()
        self.dom = None
        self.filepath = ""
        self.package_dict = {} 
    
    def getActiveVersionName(self):
        VersionsObj = self.dom.getElementsByTagName("Active")[0]
        version_name = VersionsObj.getAttribute("version_name").strip()
        return version_name
		
    def getActiveVersion(self):
        if True:
            VersionsObj = self.dom.getElementsByTagName("Active")[0]
            version_name = VersionsObj.getAttribute("version_name").strip()
            self.getVersion(version_name)
                
    def getVersion(self,version_name):
        if True:
            elementsVersion = self.dom.getElementsByTagName("Version")    
            for eleVersion in elementsVersion:          
                name = eleVersion.getAttribute("name").strip()
                if version_name == name:
                    elementsPackage = eleVersion.getElementsByTagName("Package") 
                    for elePackage in elementsPackage:     
                        package_name = elePackage.getAttribute("name").strip()
                        revision_number = elePackage.getAttribute("revision_number").strip()
                        self.setPackageDic(package_name,revision_number)
                    break 
    
    def getVersions(self):
        version_ary = []
        time_ary = []
        comment_ary = []
        if True:
            elementsVersion = self.dom.getElementsByTagName("Version")  
            for eleVersion in elementsVersion:          
                name = eleVersion.getAttribute("name").strip()
                version_ary.append(name)
                time = eleVersion.getAttribute("time").strip()
                time_ary.append(time)
                eleComment = eleVersion.getElementsByTagName("Comment")[0] 
                comment = eleComment.childNodes[0].data.strip()    
                comment_ary.append(comment)
                
        return version_ary,time_ary,comment_ary
    
    def getVersionsName(self):
        ary = []
        if True:
            elementsVersion = self.dom.getElementsByTagName("Version")    
            for eleVersion in elementsVersion:          
                name = eleVersion.getAttribute("name").strip()
                ary.append(name)
                
        return ary

    def getModulePackageList(self):
        if True:    
            parentObj= self.dom.getElementsByTagName("Packages")[0]        
            elements =  parentObj.getElementsByTagName("Package") 
            for element in elements:     
                package_name = element.getAttribute("name").strip()
                self.package_dict[package_name] = "" 
                
    def getModulePackageAry(self):
        ary = []
        if True:    
            parentObj= self.dom.getElementsByTagName("Packages")[0]        
            elements =  parentObj.getElementsByTagName("Package") 
            for element in elements:     
                name = element.getAttribute("name").strip()
                ary.append(name)
        return ary
    
    def getModuleRevisionNumberByPackage(self,package_name):
        if True:    
            parentObj= self.dom.getElementsByTagName("Packages")[0]        
            elements =  parentObj.getElementsByTagName("Package") 
            for element in elements:     
                name = element.getAttribute("name").strip()
                revision_number = element.getAttribute("revision_number").strip()
                if name == package_name :
                    return revision_number

        return None
        
    def getPackageRevisionNumber(self,package_name):
        for key in self.package_dict:
            if key == package_name:
                return self.package_dict[key]
        return -1
    
    def getNewestVersion(self) :
        element= self.dom.getElementsByTagName("Version")[0]        
        name = element.getAttribute("name").strip()
        return name
    
    def stripEmptyLines(self,originstr):
        newstr = ""
        lines = string.split(originstr, "\n")
        for line in lines:
            if "" != string.strip(line):
                newstr = newstr+"\n"+line
        return newstr
            
    
    def addData(self,name,note,isactive):
        if True:      
            root = self.dom.documentElement 
            if isactive :
                activeObj = root.getElementsByTagName("Active")[0]
                activeObj.setAttribute("version_name",name)
                
            elements = root.getElementsByTagName("Versions")[0]
            tagstart = "<Version"
            tagend = "</Version>"
            
            versionstr = tagstart + " name=" +'"' +name + '"' + " time=" +'"' +  time.asctime() + '"' +">"  + "\n"
            versionstr += "<Comment>" +note +"</Comment>\n"
            
            for key in self.package_dict:      
                versionstr += "<Package name="  +'"' + key + '"'  + "  revision_number=" + '"' + str(self.package_dict[key])  + '"' + "/>" +"\n"
            
            versionstr += tagend
             
            node = (xml.dom.minidom.parseString(versionstr)).documentElement
            
            elements.insertBefore(node, elements.childNodes[0])
            content = root.toprettyxml()
            strippedtxt = self.stripEmptyLines(content)
                
            fout = open(self.filepath,"w")
            fout.write("<?xml version=\"1.0\"?>")
            fout.write(strippedtxt)
            fout.close()
            
    def setData(self,version):
        if True:      
            root = self.dom.documentElement 
            activeObj = root.getElementsByTagName("Active")[0]
            version_name = activeObj.getAttribute("version_name").strip()
            if version_name != version :
                activeObj.setAttribute("version_name",version)
                content = root.toprettyxml()
                strippedtxt = self.stripEmptyLines(content)
                    
                fout = open(self.filepath,"w")
                fout.write("<?xml version=\"1.0\"?>")
                fout.write(strippedtxt)
                fout.close()
    
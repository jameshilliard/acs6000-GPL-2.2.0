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

class PackageData:
    
    def __init__ (self,filepath) :
        if not os.path.exists(filepath):
            #print filepath,"not exitst!"
            sys.exit(1) 
            
        self.dom = None
        self.filepath = ''    
        try:
            self.dom = xml.dom.minidom.parse(filepath)   
            self.filepath = filepath
        except IOError:
            print IOError
            sys.exit(1) 
    
    def Destory(self):
        self.dom.unlink()
        self.dom = None
        self.filepath = '' 
   
    def getActiveVersionName(self):
        VersionsObj = self.dom.getElementsByTagName("Active")[0]
        version_name = VersionsObj.getAttribute("version_name").strip()
        return version_name
    
    def getVersions(self):
        version_list = []
        elementsVersion = self.dom.getElementsByTagName("Version")  
        
        for eleVersion in elementsVersion:   
            version_dict = {}       
            version_dict['name'] = eleVersion.getAttribute("name").strip()
            version_dict['time'] = eleVersion.getAttribute("time").strip()
            eleComment = eleVersion.getElementsByTagName("Comment")[0]
            version_dict['comment'] = getText(eleComment.childNodes).strip()     
            version_list.append(version_dict)
            
        return version_list

  
    def getNewestVersionName(self) :
        element= self.dom.getElementsByTagName("Version")[0]        
        name = element.getAttribute("name").strip()
        return name
    
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

            
            versionstr += tagend
             
            node = (xml.dom.minidom.parseString(versionstr)).documentElement
            
            elements.insertBefore(node, elements.childNodes[0])
            content = root.toprettyxml()
            strippedtxt = stripEmptyLines(content)

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
                strippedtxt = stripEmptyLines(content)
                    
                fout = open(self.filepath,"w")
                fout.write("<?xml version=\"1.0\"?>")
                fout.write(strippedtxt)
                fout.close()
                
#----------------------------------------------------------
def getVersionNameList(version_list):
    version_name_list = []
    for dict in version_list:
        version_name_list.append(dict['name'])
    return version_name_list
    
def getOneVersion(version_list,name):
    version_dict = {}
    for dict in version_list:
        if dict['name']==name :
            version_dict = dict
            break
    return version_dict 

def getVersionNameIsExist(version_list,name):
    for listvalue in version_list:
        if listvalue==name :
            return True
    return False
    
def stripEmptyLines(originstr):
    newstr = ""
    lines = string.split(originstr, "\n")
    for line in lines:
        if "" != string.strip(line):
            newstr = newstr+"\n"+line
    return newstr  
    
def getText(nodelist):
    rc = ""
    for node in nodelist:
        if node.nodeType == node.TEXT_NODE:
            rc = rc + node.data
    return rc  
 

        
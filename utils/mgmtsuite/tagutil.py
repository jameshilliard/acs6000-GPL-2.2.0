##
# This program is a lib for executable scripts.
#
# Author: Ice Liu
# History: Draft on Feb 7, 2006
#          Added inserttag method on Feb 16, 2006
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
def getdepmods(modname):
    listfname = "module_"+modname.lower()+"_list.xml"
    doc = xml.dom.minidom.parse(listfname)
    root = doc.documentElement
    depattr = root.getAttribute("dependency")
    deps = []
    if None != depattr:
        deps = string.split(depattr,"|")
    return deps


#
def getdeptag(modname):
    tag = "HEAD"
    if os.path.exists("Tag"):
        tagfile = open("Tag")
        lines = tagfile.readlines()
        for line in lines:
            if line.lower().startswith(modname.lower()):
                words = string.split(line)
                tag = string.strip(words[1])
        tagfile.close()
    return tag


# To return a ascii format time string of yesterday.
# @param format, e.g. "%Y/%m/%d"
# @return, formated time
def yesterday(format):
    y = time.localtime(time.time()-24*60*60)
    ystr = time.strftime(format,y)
    return ystr


# To return a ascii format time string of tomorrow.
# @param format, e.g. "%Y/%m/%d"
# @return, formated time
def tomorrow(format):
    t = time.localtime(time.time()+24*60*60)
    tstr = time.strftime(format,t)
    return tstr


# To return a ascii format time string of today.
# @param format, e.g. "%Y/%m/%d"
# @return, formated time
def today(format):
    tstr = time.strftime(format,time.localtime())
    return tstr


# To strip the empty line
# @param, string
# @return, striped string
def stripemptylines(originstr):
    newstr = ""
    lines = string.split(originstr, "\n")
    for line in lines:
        if "" != string.strip(line):
            #print line
            newstr = newstr+"\n"+line
    #print newstr
    return newstr


# To verify a tag
# @param, string, module name
# @param, string, tag name
# return, 0 if not match, 1 if match
def verifytag(module, tag):
    alltags = getalltags(module)
    for eachtag in alltags:
        if tag == eachtag:
            return 1
    return 0

# To get the current tag from module_modname_rel.xml with modname.
# @param modname, the given module name.
# @return, the latest tag in xml file should be the current tag.
def getcurtag(modname):
    curtag = ""
    relfname = "module_"+modname.lower()+"_rel.xml"
    if not os.path.exists(relfname):
        print relfname,"not exitst!"
        sys.exit(1)
    doc = xml.dom.minidom.parse(relfname)
    root = doc.documentElement
    historylst = root.getElementsByTagName("History")
    history = historylst[0]
    deliverylst = history.getElementsByTagName("Delivery")
    latestdvy = deliverylst[0]
    curtag = latestdvy.getAttribute("Tag")            
    return curtag


# To get the active tag from module_all_rel.xml with modname.
# @param modname, here it always is "all".
# @return, the active tag in xml file should be the current tag.
def getactivetag(modname):
    activetag = ""
    relfname = "module_"+modname.lower()+"_rel.xml"
    if not os.path.exists(relfname):
        print relfname,"not exitst!"
        sys.exit(1)
    doc = xml.dom.minidom.parse(relfname)
    root = doc.documentElement
    historylst = root.getElementsByTagName("History")
    history = historylst[0]
    deliverylst = history.getElementsByTagName("Delivery")
    for delivery in deliverylst:
        activeattr = delivery.getAttribute("Active")
    if None != activeattr:
        if "ON" == activeattr.upper():
            activetag = delivery.getAttribute("Tag")            
    return activetag


# To get the current module tag from module_all.xml with modname.
# @param modname, the given module name.
# @return, the latest tag in xml file should be the current tag.
def getmodtag(modname):
    curtag = ""
    relfname = "module_all_rel.xml"
    if not os.path.exists(relfname):
        print relfname,"not exitst!"
        sys.exit(1)    
        
    doc = xml.dom.minidom.parse(relfname)
    root = doc.documentElement
    historylst = root.getElementsByTagName("History")
    history = historylst[0]
    deliverylst = history.getElementsByTagName("Delivery")
    
    for delivery in deliverylst:
        attr = delivery.getAttribute("Active")
        if None != attr:
            if "ON" == attr.upper():
                modlst = delivery.getElementsByTagName("Modules")
                modnode = modlst[0]
                mods = modnode.getElementsByTagName("Module")
        for mod in mods:
            if modname.upper() == mod.getAttribute("Name").upper():
                curtag = mod.getAttribute("Tag")
                break
        if "" != curtag:
            break
    return curtag


# To get all source paths of given module name.
# @param modname, the given modules name.
# @return, a list of source path in repository.
def getsrcpaths(modname):
    paths = []
    listfname = "module_"+modname.lower()+"_list.xml"
    if not os.path.exists(listfname):
        print listfname,"not exitst!"
        sys.exit(1)    
    doc = xml.dom.minidom.parse(listfname)
    root = doc.documentElement
    unitslst = root.getElementsByTagName("Units")
    units = unitslst[0]
    unitlst = units.getElementsByTagName("Unit")
    for unit in unitlst:
        path = unit.getAttribute("path")
        paths.append(path)
        
    return paths


# To get all the exists modules defined.
# @param srcpath, the path xml files in
# @return list, of all modules names
def getallexsmods(srcpath):
    lists = dircache.listdir(srcpath)
    exsmods = []
    for path in lists:
        if os.path.isfile(path):
            if path.endswith('_rel.xml'):
                modname = path.replace('module_','')
                modname = modname.replace('_rel.xml','')
                # Not get the three special modules, the project already in the list.
                if "all" != modname:
                    exsmods.append(modname)
    return exsmods


# To get current cvs user.
# @return cvs user
def getcvsusr():
    froot = open('CVS'+os.sep+'Root','r')
    line = froot.readline()
    repository = string.replace(line,'\n','')
    exp = re.compile("^(:.*)?:(.*)@.*")
    objs = exp.search(line)
    if None != objs:
        g = objs.groups()
        cvsusr = g[1]
        return cvsusr
    else:
        print "parse cvs user name err"
        sys.exit(1)


chgnodestr = """
<Changes>
    <Change type="bugfix|internalreview|changerequest" teamtrackerID="" interfacechange="true|false">
                                    Changenote should be here. 
    </Change>             
</Changes>
"""

def inserttag(modname, tag, note, verbose, test, previous, modtags, log):
    
    if test:
        return  ""
    
    fname = "module_"+modname.lower()+"_rel.xml"
    # update it firstly
    #os.system("cvs up -A "+fname)
    
    if not os.path.exists(fname):
        log.write( "Error ! " + fname +"not exitst!" )
        return  "error"
        
    doc = xml.dom.minidom.parse(fname)
    root = doc.documentElement
    historylst = root.getElementsByTagName("History")
    history = historylst[0]
    t = time.asctime()
    str = "<Delivery Tag=\""+tag+"\" Date=\""+t+"\">"
    
    if None == modtags:
        pass
    else:
        str = str+"<Modules>"
        for key in modtags.keys():
            str = str+"<Module Name=\""+key+"\" Tag=\""+modtags[key]+"\" />"
        str = str+"</Modules>"
        
    
    str = str+chgnodestr
    str = str+"<Deliverynote>"+note+"</Deliverynote>"
    str = str+"</Delivery>"
    node = (xml.dom.minidom.parseString(str)).documentElement

    # childs[0] is a text node, childs[1] is the first element node
    firstele = (history.childNodes)[1]
    history.insertBefore(node, firstele)
    content = root.toprettyxml()
    strippedtxt = stripemptylines(content)
  
   
    if not test:
        fout = open(fname,"w")
        fout.write("<?xml version=\"1.0\"?>")
        fout.write(strippedtxt)
        fout.close()
        os.system('cvs ci -m"none" '+fname)

def deletetag(modname, tag, verbose, test, log):
    if test:
        return ""
    
    fname = "module_"+modname.lower()+"_rel.xml"
    # update it firstly
    #os.system("cvs up -A "+fname)
    if not os.path.exists(fname):
        log.write( "Error ! " + fname +"not exitst!" )
        return  "error"
    
    doc = xml.dom.minidom.parse(fname)
    root = doc.documentElement
    historylst = root.getElementsByTagName("History")
    history = historylst[0]
    delvylst = history.getElementsByTagName("Delivery")
    
    for delivery in delvylst:
        recordtag = delivery.getAttribute("Tag")
        if tag == recordtag:
            delivery.setAttribute("Status","deleted")
            t = time.asctime()
            delivery.setAttribute("Deldate",t)
            break
    
    content = root.toprettyxml()
    strippedtxt = stripemptylines(content)
    
   
    if not test:
        fout = open(fname,"w")
        fout.write("<?xml version=\"1.0\"?>")
        fout.write(strippedtxt)
        fout.close()
        os.system('cvs ci -m"none" '+fname)
        
# To get all tags of a module.
# @param modname, the name of module
# @return, a list of all tags  
def getalltags(modname):
    tags = [] 
    fname = "module_"+modname.lower()+"_rel.xml"
     # update it firstly
    os.system("cvs up -A "+fname)
    if not os.path.exists(fname):
        log.write( "Error ! " + fname +"not exitst!" )
        return  "  "
    
    tags.append("HEAD")
    
    doc = xml.dom.minidom.parse(fname)
    root = doc.documentElement
    historylst = root.getElementsByTagName("History")
    history = historylst[0]
    deliverylst = history.getElementsByTagName("Delivery")
    for delivery in deliverylst:
        tag = delivery.getAttribute("Tag")
        if None == delivery.getAttribute("Status"):
            tags.append(tag)
        elif "deleted" != delivery.getAttribute("Status"):
            tags.append(tag)
        else:
            pass
        
    return tags


# To get all source paths of given module name.
# @param modname, the given modules name.
# @return, a list of source path in repository.
def getpathinfo(modname, pathname):
    
    listfname = "module_"+modname.lower()+"_list.xml"
    if not os.path.exists(listfname):
        return  listfname,"not exitst!"
        
    doc = xml.dom.minidom.parse(listfname)
    root = doc.documentElement    
    unitslst = root.getElementsByTagName("Units")
    units = unitslst[0]
    unitlst = units.getElementsByTagName("Unit")
    
    #Module = root.getElementsByTagName("Module")    
    #owner = Module.getAttribute("owner")
    #desclist  =  Module.getElementsByTagName("Description")
    #brief = desclist[0]
    owner = ""
    brief   = ""
    
    for unit in unitlst:
        name = unit.getAttribute("path")
        if name == pathname:
            owner = unit.getAttribute("owner")
            brief   = unit.getAttribute("brief")
            break
        
    pathinfo ="\n" * 3 + "<h2>Owner:  " + owner + "\n" + "Brief:  " + brief + "</h2>" 
    return pathinfo


def getProjectInfo(project):    
    if not os.path.exists(project):
        return  None
    
    if  True:
        dom = xml.dom.minidom.parse(project) 
        serverNode = dom.getElementsByTagName("Server")[0]
        local =  serverNode.getElementsByTagName("Local")[0]
        localserver =local.childNodes[0].data.strip()
        remote = serverNode.getElementsByTagName("Remote")[0]
        remoteserver =remote.childNodes[0].data.strip()
        serverNode = dom.getElementsByTagName("Project")[0]
        prjname = serverNode.getAttribute("name")
        dom.unlink()
        
    return localserver+"/", prjname, remoteserver+"/"

def getSvnSyncPath(project):    
    if not os.path.exists(project):
        return  None
    
    if  True:
        dom = xml.dom.minidom.parse(project)
        serverNode = dom.getElementsByTagName("Server")[0]
	svnsync =  serverNode.getElementsByTagName("SvnSync")[0]
        svnsyncpath =svnsync.childNodes[0].data.strip()
        dom.unlink()
        
    return svnsyncpath
 
 
def getsvnpaths(project):    
     
    if not os.path.exists(project):
        return  None
    
    if True: 
        svnpaths = []    
        dom = xml.dom.minidom.parse(project) 
        modules = dom.getElementsByTagName("Modules")[0]        
        module = modules.getElementsByTagName("Module")     
        for element in module:
           for  node  in element.childNodes:            
               if node.nodeType == node.TEXT_NODE:
                   svnpaths.append( node.data.strip()  )
       
        dom.unlink()
   
   
    paths= []
    for path in svnpaths:
        paths.append(  (path, path.split("/") ) ) 
    
    return paths


def getProjectTags(project):
  
    if not os.path.exists(project):
        return  None
    
    if True:
        tags = []
        tags.append("trunk")
        
        dom = xml.dom.minidom.parse(project)       
        elements = dom.getElementsByTagName("Tag")    
        for element in elements:          
            tag = element.getAttribute("Name")
            tag.strip()
            if None == element.getAttribute("Status"):
                tags.append(tag)
            elif "deleted" != element.getAttribute("Status"):
                tags.append(tag)
            else:
                pass
         
        branches = []
        elements = None
        elements = dom.getElementsByTagName("Branch")    
        for element in elements:          
            branch = element.getAttribute("Name")
            branch.strip()
            if None == element.getAttribute("Status"):
                branches.append(branch)
            elif "deleted" != element.getAttribute("Status"):
                branches.append(branch)
            else:
                pass                
        dom.unlink()   
    
    return tags, branches
    
    
def delsvntag(project, name, tagOrbranch):
  
    if not os.path.exists(project):
        return  None
  
    if True:
        tags = []     
        dom = xml.dom.minidom.parse(project)       
        root = dom.documentElement 
        if  tagOrbranch == "/tags/":
            elements = root.getElementsByTagName("Tag")    
        else:
            elements = root.getElementsByTagName("Branch")    
            
        for element in elements:          
            tag = element.getAttribute("Name")
            tag.strip()
            if name == tag:
                element.setAttribute("Status","deleted")
                t = time.asctime()
                element.setAttribute("End",t)
                break            
            
        content = root.toprettyxml()
        strippedtxt = stripemptylines(content)
            
        fout = open(project,"w")
        fout.write("<?xml version=\"1.0\"?>")
        fout.write(strippedtxt)
        fout.close()
        dom.unlink()       
    
def addsvntag(project, name, tagOrbranch, note, mods, tags ):
  
    if not os.path.exists(project):
        return  None
    
    if True:
        dom = xml.dom.minidom.parse(project)       
        root = dom.documentElement 
        if  tagOrbranch == "/tags/":
            elements = root.getElementsByTagName("Tags")[0]
            tagstart ="<Tag "
            tagend = "</Tag>"
        else:
            elements = root.getElementsByTagName("Branches")[0]
            tagstart ="<Branch "
            tagend = "</Branch>"
       
        str = tagstart + "Begin=" +'"' +  time.asctime() + '"' + " Name=" +'"' +name + '"' +">"  + "\n"
        str += "<Comment>" +note +"</Comment>"
        
        for  index in  range( len(mods) ):        
            str += "<Module name="  +'"' + mods[ index ] + '"'  + "  RefTag=" + '"' + tags[ index ]  + '"' + "> </Module>" +"\n"
        str += tagend
         
        node = (xml.dom.minidom.parseString(str)).documentElement
        
        elements.insertBefore(node, elements.childNodes[0])
        content = root.toprettyxml()
        strippedtxt = stripemptylines(content)
            
        fout = open(project,"w")
        fout.write("<?xml version=\"1.0\"?>")
        fout.write(strippedtxt)
        fout.close()
                   
        dom.unlink()   
        
    
def getTagNames( svnpath, log=None):                
        
        #try: 
        if True:
            svncmd = "svn list -v "
            syscmd =  svncmd + svnpath 
            stdoutfd = os.popen( syscmd )   
            
            tagmap = {}              
            for line in stdoutfd.readlines():     
                taginfo = line.split()
                revision = taginfo[0]               
                name = taginfo[ len(taginfo)-1 ]
                tag= name[ :name.find("/")]
                tagmap [ revision ] = tag
                
            keys = tagmap.keys()
            keys.sort()
            keys.reverse()
            return   [ tagmap[key ] for key in keys  ]    
            
        #except:
        #    return None   

def getTagAndBranch( svnpath, log=None):                
        
        if True: 
            tagmap        = {}      
            branchmap  = {}       
            allmap         = {}
            tags             =  [] 
            branches     =  []            
            svncmd = "svn list -v "
            # get tags
            syscmd =  svncmd + svnpath + "/tags"
            stdoutfd = os.popen( syscmd )               
            for line in stdoutfd.readlines():     
                taginfo = line.split()
                revision = taginfo[0]               
                name = taginfo[ len(taginfo)-1 ]
                tag= name[ :name.find("/")]
                tagmap [ revision ] = tag
                allmap [ revision ] = tag
                
            # get branches
            syscmd =  svncmd + svnpath + "/branches"
            stdoutfd = os.popen( syscmd )               
            for line in stdoutfd.readlines():     
                taginfo = line.split()
                revision = taginfo[0]               
                name = taginfo[ len(taginfo)-1 ]
                tag= name[ :name.find("/")]
                branchmap [ revision ] = tag
                allmap [ revision ] = tag                
           
            keys = allmap.keys()
            keys.sort()
            keys.reverse()
            alltags = [ allmap[key ] for key in keys  ]   
            alltags.insert(0, "trunk")

            keys = tagmap.keys()
            keys.sort()
            keys.reverse()
            tags = [ tagmap[key ] for key in keys  ]   
            tags.insert(0, "trunk")

            keys = branchmap.keys()
            keys.sort()
            keys.reverse()
            branches = [ branchmap[key ] for key in keys  ]
 
            return    alltags, tags, branches
            
     
    
if __name__ == '__main__':
  
    project = "project_mergepoint.xml"
    print getTagNames("svn://sha-scm/avocent-test/utils/tags", None)
     
     

#!/usr/bin/env python
#----------------------------------------------------------------------------
# This program is version panel.

# Author: Tercel Yi
# History: Draft on , 2008-1-30
#
# Copyright:    (c) Avocent
#----------------------------------------------------------------------------

import wx
import wx.combo
import os
import sys
import string
import re

from lib.config import *
import lib.datacommon as datacommon

 
#----------------------------------------------------------------------


class VersionPanel(wx.Panel):
    def __init__(self, mainFrame, parent):
        wx.Panel.__init__(self, parent, -1) 
        
        self.mainFrame     = mainFrame   
        self.package_list = self.mainFrame.objProjectData.package_list
        self.treePydata = self.mainFrame.treePydata
        self.filepath = ''
        self.namelabel = ''
        self.nametext = ''
        self.log = mainFrame.log   
        self.controls_data_dict= {'version_list':[], 'version_name_list':[], 'version_active':''}
        
        self.version_name_list = []
        self.version_active = ''
        
        self.initPanel()
        
    def __del__(self):
        pass
            
    def initPanel(self):
      
        if self.treePydata['type']=='root':
            self.namelabel = 'Name:'
            self.nametext = 'name'
            self.initProjectData()
        elif self.treePydata['type']=='list':
            self.namelabel = 'Revision:'
            self.nametext = 'revision  number'
            self.initPackageData() #node

        self.createControls()
        
    def initProjectData(self):
        self.filepath = datacommon.getProjectVersionXmlPath(self.mainFrame.projectxml)
        self.controls_data_dict = datacommon.getProjectVersionData(self.filepath)
                 
    def initPackageData(self):
        self.filepath = DEFAULT_PACKAGESDATA_DIR+'/'+self.treePydata['dir']+DEFAULT_DATA_SUFFIX
        self.controls_data_dict = datacommon.getPackageData(self.filepath)
            
    def createControls(self):
        # Create controls
            
        #edit box
        edit_ObjStaticText_version = wx.StaticText(self,-1,self.namelabel)
        version_list= self.controls_data_dict['version_name_list']
        version_active = self.controls_data_dict['version_active']
        if self.treePydata['type']=='list':
            version_list.insert(0,DEAFULT_NULL)
            if version_active=='' :
                version_active = DEAFULT_NULL
        self.cbversion_list = wx.ComboBox(self, choices=version_list)          
        #mysize = (80,-1)  
        #self.cbversion_list.SetMinSize(mysize) 
        self.cbversion_list.SetValue(version_active)
        self.Bind(wx.EVT_COMBOBOX, self.EvtComboBoxVersion, self.cbversion_list)   
        
        self.btn_set = wx.Button(self, label='Set Active', name='btn_set')
        self.Bind(wx.EVT_BUTTON, self.Click, self.btn_set) 
        
        editBox = wx.StaticBoxSizer(wx.StaticBox(self, -1, " Manage: "), wx.HORIZONTAL)
        editBox.AddMany([
            (edit_ObjStaticText_version, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5),
            (self.cbversion_list, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5),
            (self.btn_set, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5),
            ])
        
        #add box
        self.add_ObjTextCtrl_version = wx.TextCtrl(self, -1, '',size=(160, -1))  
        self.add_ObjCheckBox_isavctive = wx.CheckBox(self, -1, " Active")
        self.add_ObjTextCtrl_comment = wx.TextCtrl(self, -1,size=(300, 100),style= wx.TE_MULTILINE | wx.SUNKEN_BORDER )
        
        addBox_01 = wx.BoxSizer(wx.HORIZONTAL) 
        addBox_01.AddMany([
            (self.add_ObjTextCtrl_version,0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL),
            ((5,0)),
            (self.add_ObjCheckBox_isavctive,0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL),
            ])
            
        self.btn_add = wx.Button(self, label='Add', name='btn_add',size=(80, -1))
        self.Bind(wx.EVT_BUTTON, self.Click, self.btn_add)
 
        add_ObjFlexGridSizer =  wx.FlexGridSizer(cols=2, hgap=10, vgap=10)
               
        add_ObjFlexGridSizer.Add( wx.StaticText(self,-1,self.namelabel))
        add_ObjFlexGridSizer.Add( addBox_01)
        
        if self.treePydata['type']=='root':

            packageActionList = [ 'Default', 'Force']
            self.add_ObjRadioBox_packagesversion = wx.RadioBox(
                self, -1, "", wx.DefaultPosition, wx.DefaultSize,
                packageActionList, 2, wx.RA_SPECIFY_COLS
                )
            self.Bind(wx.EVT_RADIOBOX, self.EvtRadioBox, self.add_ObjRadioBox_packagesversion)   
            self.add_ObjTextCtrl_packagesversion = wx.TextCtrl(self, -1, datacommon.g_head_revision,size=(160, -1)) 
            self.add_ObjTextCtrl_packagesversion.SetHelpText("package version number")
            addBox_02 = wx.BoxSizer(wx.HORIZONTAL) 
            addBox_02.AddMany([
                (self.add_ObjTextCtrl_packagesversion),
                ((5,0)),
                (self.add_ObjRadioBox_packagesversion,0, wx.TOP,-10),
                ])
            
            add_ObjFlexGridSizer.Add( wx.StaticText(self,-1,"Packages Revision:"))
            add_ObjFlexGridSizer.Add( addBox_02)
        
        add_ObjFlexGridSizer.Add( wx.StaticText(self,-1,"Comment:"))
        add_ObjFlexGridSizer.Add( self.add_ObjTextCtrl_comment,15, wx.EXPAND|wx.ALL)
        
        add_ObjFlexGridSizer.Add( (20, 0))
        add_ObjFlexGridSizer.Add( self.btn_add)
        
        addBox = wx.StaticBoxSizer(wx.StaticBox(self, -1, " Add: "), wx.HORIZONTAL)
        addBox.Add(add_ObjFlexGridSizer, 2, wx.EXPAND|wx.ALL, 10)
                
        box = wx.BoxSizer(wx.VERTICAL)            
        box.Add(editBox, 0, wx.EXPAND|wx.ALL, 10)
        box.Add(addBox, 2, wx.EXPAND|wx.ALL, 10)
        
        self.SetSizer(box)
        self.SetAutoLayout(True)
        

    def treeModChanged(self):
        pass

    def EvtCheckBox(self, evt):        
        pass

        
    def Click(self, event):
        self.log.Clear()
        btn = event.GetEventObject()
        cmd = btn.GetName()
        wx.BeginBusyCursor()        
        if  cmd == 'btn_set':
            self.doset()
        elif cmd == 'btn_add':
            self.doadd() 
        elif cmd == 'btn_refresh':
            self.dorefresh()       
        else:
            sys.exit(1)
            
        wx.EndBusyCursor()

            
    # When the user selects something, we go here.
    def EvtComboBoxVersion(self, evt):
        selected = self.cbversion_list.GetValue() 
        self.version_active =  selected  
        self.showVersionInfo(selected)    
        
    def EvtRadioBox(self, evt):
        self.log.Clear()
        package_scope = evt.GetInt()
        if package_scope==0 :
            self.log.WriteText("Defaule mode: If there's active flag for any package, the active version will be used, or, the given version will be used.")    
        elif package_scope==1 :
            self.log.WriteText("Force mode: Use the given version for all packages, whatever set active flag or not.")    
        
    def errorMessage(self, msg):
       dlg = wx.MessageDialog(self, msg, 'Warning', wx.OK | wx.ICON_INFORMATION)
       dlg.ShowModal()
       dlg.Destroy()
    
    def dorefresh(self):
        #wx.BusyInfo("Refresh subversion tag, please wait ... ")
        self.mainFrame.LoadDemo(self.treePydata['treenav'])
        
    def doadd(self):
        name = self.add_ObjTextCtrl_version.GetValue()    
        note = self.add_ObjTextCtrl_comment.GetValue()    
        isactive = self.add_ObjCheckBox_isavctive.IsChecked() 
          
        isExist = False
        msg_btn = ""
        msg_syscmd = ""
        
        if name.strip() == '' :
            self.errorMessage("Please type "+self.nametext+"!")
            return 
    
        if note.strip() == '' :
            self.errorMessage("Please type comment!")
            return 

        #create dir/file    
        if self.filepath != '' :
            if not os.path.exists(self.filepath):
                if self.treePydata['type']=='root':
                    datacommon.createProjectVersionsFile(self.filepath,self.treePydata['name'],self.treePydata['manager'])
                elif self.treePydata['type']=='list':
                    self.createDir()
                    datacommon.createPackageVersionFile(self.filepath,self.treePydata['name'],self.treePydata['manager'])
                    
                self.doSvnCreateFile()
    
            else :
                pass
                
    
        #add  version in xmlfile            
        if self.treePydata['type']=='root':
            package_version = self.add_ObjTextCtrl_packagesversion.GetValue().strip() 
            package_scope = self.add_ObjRadioBox_packagesversion.GetSelection()
            if package_version=='' or package_version=='0'  or len(re.findall('\D+',package_version))>0 or  string.atoi(package_version)>string.atoi(datacommon.g_head_revision):
                self.errorMessage("Please type valid package revision number!")
                return
            
            isExist = datacommon.checkProjectVersionName(self.controls_data_dict['version_name_list'],name)
            if not isExist :
                datacommon.addProjectVersionData(self.filepath,name,note,isactive,self.package_list,package_version,package_scope)
                txtpath = datacommon.getProjectVersionTxtPath(self.mainFrame.projectxml)
                datacommon.addTxtData(txtpath,name,note,self.package_list,package_version,package_scope)
                self.controls_data_dict['version_name_list'].insert(0,name)
                self.doSvnAddVersion(name,isactive)
                
            else:
                msg_btn = "Add "+self.nametext+" "+name+"  Failed,  It's already exist ."
                self.errorMessage(msg_btn)
                return 

        elif self.treePydata['type']=='list':
            if name=='0'  or len(re.findall('\D+',name))>0  or  string.atoi(name)>string.atoi(datacommon.g_head_revision):
                self.errorMessage("Please typ valide "+self.nametext+"!")
                return 
            isExist = datacommon.checkPackageName(self.controls_data_dict['version_name_list'],name)
            if not isExist :
                datacommon.addPackageData(self.filepath,name,note,isactive)  
                self.controls_data_dict['version_name_list'].insert(0,name)
                self.doSvnAddVersion(name,isactive)
                
            else:
                msg_btn = "Add "+self.nametext+" "+name+"  Failed,  It's already exist ."
                self.errorMessage(msg_btn)
                return 
        
        self.dorefresh()   
            
    def doset(self):   
        version = self.cbversion_list.GetValue()   
        if version == DEAFULT_NULL:
            version = ''
        msg_btn = ''
        msg_syscmd = '' 
        if self.treePydata['type']=='root':
            datacommon.setProjectVersionData(version,self.filepath)
        elif self.treePydata['type']=='list':
            datacommon.setPackageData(version,self.filepath)
            
        msg_btn = "Set Active "+self.nametext+" "+version
        syscmd = "svn ci   "+ self.filepath +"   "+" -m " + "\""+  msg_btn +"\""             
        os.system(syscmd)
        msg_btn += " Successful!"
        msg_syscmd = "\n"+"-" * 120 + " \n"+"[svn command] ->  " + syscmd +" \n"

        self.log.WriteText(msg_btn+msg_syscmd)  
        
    def getRefPath(self,  mod, bBranch ):
        pass 
        
    def showVersionInfo(self,selected):
        version_dict = {}
        if self.treePydata['type']=='root':
            version_dict = datacommon.getOneProjectVersion(self.controls_data_dict['version_list'],selected)
        elif self.treePydata['type']=='list':
            version_dict = datacommon.getOnePackageVersion(self.controls_data_dict['version_list'],selected)
        
            
        msg = ""
        if not (self.treePydata['type']=='list' and selected==DEAFULT_NULL):
            msg += "name: "+selected+"\n"
            msg += "time: "+version_dict['time']+"\n"
            msg += "comment: "+version_dict['comment']+"\n"
        
        self.log.Clear()
        self.log.WriteText(msg) 
    
    def createDir(self) :
            dir_list = self.filepath.split('/')
            filepath = '.'
            for i in range(len(dir_list)-1) :
                filepath += '/'+dir_list[i]
                if not os.path.exists(filepath):
                    os.makedirs(filepath)
                    self.doSvnCreateFolder(filepath)
                        
    def doSvnCreateFolder(self,filepath):
        syscmd1 =  "svn add "+filepath
        os.system( syscmd1 )   
        syscmd2 =  "svn commit "+filepath+ " -m \"add folder\""
        os.system( syscmd2 )   
        msg_addfile = "Create folder: "+filepath+" \n"
        msg_addfile += "[svn command] ->  "+syscmd1+" \n"
        msg_addfile += "[svn command] ->  "+syscmd2+" \n"
        msg_addfile += "-" * 120 + " \n"
        self.log.WriteText(msg_addfile)
        
    def doSvnCreateFile(self):
        syscmd1 =  "svn add "+self.filepath
        os.system( syscmd1 )   
        syscmd2 =  "svn commit "+self.filepath+ " -m \"add file\""
        os.system( syscmd2 )
        msg_addfile = "Create file: "+self.filepath+" \n"
        msg_addfile += "[svn command] ->  "+syscmd1+" \n"
        msg_addfile += "[svn command] ->  "+syscmd2+" \n"
        msg_addfile += "-" * 120 + " \n"
        
        self.log.WriteText(msg_addfile)
        
    def doSvnAddVersion(self,name,isactive=False):
        msg_btn = "Add version "+name
        if isactive :
            msg_btn += " and Set actively "
            
        syscmd = "svn ci   "+ self.filepath +"   "+" -m " + "\""+  msg_btn +"\""             
        os.system(syscmd) 
        msg_btn += " Successful!"
        msg_syscmd = " \n"+"[svn command] ->  " + syscmd +" \n"

        self.log.WriteText(msg_btn+msg_syscmd) 
    
#----------------------------------------------------------------------

def runVersionPanel(frame, nb):
    win = VersionPanel(frame,  nb)
    return win

#----------------------------------------------------------------------



#---------------------------------------------------------------------------
        
if __name__ == '__main__':
                  
    app = wx.PySimpleApp()
    frame = myFrame(None, -1, "Size Test")

    frame.Show()

    app.MainLoop()


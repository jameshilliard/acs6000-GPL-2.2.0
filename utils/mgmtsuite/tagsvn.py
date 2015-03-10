import wx
import wx.combo
import os
import sys
import re

import tagutil 
 
#----------------------------------------------------------------------


class TestPanel(wx.Panel):
    def __init__(self, mainFrame, parent):

        self.bRefBranch = False
        
        self.mainFrame     = mainFrame
        self.projectxml      = mainFrame.projectxml
        self.projectname   = mainFrame.projectname
        self.mod               =  mainFrame.projectname
        self.server              = mainFrame.server        
        self.remote            = mainFrame.remote
        self.log                  = mainFrame.log   
        self.tags                       = mainFrame.projectTags
        self.branches               = mainFrame.projectBranches                                  
        self.paths              = mainFrame.paths
        # Create controls
        mystyle = wx.EXPAND|wx.ALL
        wx.Panel.__init__(self, parent, -1)       
        self.cballtags = wx.ComboBox(self, choices=self.tags,   style=mystyle| wx.CB_READONLY)          
        mysize = (360,-1)  
        self.cballtags.SetMinSize(mysize) 
        self.Bind(wx.EVT_COMBOBOX, self.EvtComboBox, self.cballtags)            
        self.cbbranches = wx.ComboBox(self, choices=self.branches , style= mystyle| wx.CB_READONLY)
        self.Bind(wx.EVT_COMBOBOX, self.EvtComboBox, self.cbbranches)    
        
        self.tagname = wx.TextCtrl(self, -1, "" , style=wx.SUNKEN_BORDER)    
        self.revisionname = wx.TextCtrl(self, -1, "" , style=wx.SUNKEN_BORDER)                                 
        self.tagcomment = wx.TextCtrl(self, -1,style= wx.TE_MULTILINE | wx.SUNKEN_BORDER )
       
         #button                   
        self.btn_tag = wx.Button(self, label='Tag', name='tag')
        self.Bind(wx.EVT_BUTTON, self.Click, self.btn_tag)        
        self.btn_del = wx.Button(self, label='Delete', name='del')
        self.Bind(wx.EVT_BUTTON, self.Click, self.btn_del)
        self.btn_refresh = wx.Button(self, label='Refresh', name='refresh')
        self.Bind(wx.EVT_BUTTON, self.Click, self.btn_refresh)
        self.btn_check = wx.Button(self, label='Check', name='check')
        self.Bind(wx.EVT_BUTTON, self.Click, self.btn_check)
        #self.btn_quit = wx.Button(self, label='Quit', name='quit')
        #self.Bind(wx.EVT_BUTTON, self.Click, self.btn_quit)
       
        #options 
        self.opt_verbose = wx.CheckBox(self, -1, "Verbose")
        self.Bind(wx.EVT_CHECKBOX, self.EvtCheckBox, self.opt_verbose)
        self.opt_verbose.SetValue(True)
        self.opt_test = wx.CheckBox(self, -1, "Test")
        self.Bind(wx.EVT_CHECKBOX, self.EvtCheckBox, self.opt_test)
        self.opt_branch   = wx.CheckBox(self, -1, "Branch", name="branch")
        self.Bind(wx.EVT_CHECKBOX, self.EvtCheckBox, self.opt_branch)
        
        # Setup the layout
        sb = wx.StaticBox(self, -1, "Options")
        optionBox = wx.StaticBoxSizer(sb, wx.VERTICAL)
        optionBox.Add(self.opt_verbose,  1, wx.ALL, 3)
        optionBox.Add(self.opt_test,  1, wx.ALL, 3)
        optionBox.Add(self.opt_branch,   1, wx.ALL, 3)        
        #optionBox.Add(self.ref_branch  ,   1, wx.ALL, 3)
         
                               
        cmdBox = wx.BoxSizer(wx.VERTICAL)
        cmdBox.Add(self.btn_tag,  1, mystyle, 4)
        cmdBox.Add(self.btn_del,  1, mystyle, 4)
        #cmdBox.Add(self.btn_quit,   1, mystyle, 4)   
        cmdBox.Add(self.btn_refresh,   1, mystyle, 4)  
        cmdBox.Add(self.btn_check,   1, mystyle, 4)         
         
        fgs = wx.FlexGridSizer(cols=3, hgap=10, vgap=30)
        fgs.Add( wx.StaticText(self,-1,"Refer Tags:"), 3)
        fgs.Add( (10,10), 1)
        fgs.Add( self.cballtags, 15,  mystyle)
        
        fgs.Add(wx.StaticText(self, -1, "Refer Branches:"), 3 )
        fgs.Add( (10,10) , 1)
        fgs.Add( self.cbbranches, 15,  mystyle)
        
        fgs.Add(wx.StaticText(self, -1, "Revision:"), 3 )
        fgs.Add( (10,10) , 1)
        fgs.Add( self.revisionname, 15,  mystyle)
        
        fgs.Add(wx.StaticText(self, -1, "Tag Name:"), 3)
        fgs.Add((10,10), 1)
        fgs.Add(self.tagname,15,  mystyle)
        
        fgs.Add(wx.StaticText(self, -1, "Tag Comment:"), 3)
        fgs.Add((10,10), 1)
        fgs.Add(self.tagcomment, 15, mystyle)
         
         
        right_box = wx.BoxSizer(wx.VERTICAL)
        right_box.Add(optionBox,  1, wx.EXPAND|wx.ALL, 3)
        right_box.Add(cmdBox, 1,wx.EXPAND|wx.ALL, 5) 
                
        box = wx.BoxSizer()               
        box.Add(fgs, 3, wx.EXPAND|wx.ALL, 20)
        box.Add(right_box, 1, wx.EXPAND|wx.ALL, 20)
        self.SetSizer(box)

    def treeModChanged(self, mainFrame, mod, tags, branches):
        self.mod               =  mod
        self.projectxml      = mainFrame.projectxml
        self.projectname   = mainFrame.projectname
        self.server              = mainFrame.server        
        self.remote            = mainFrame.remote
        self.log                  = mainFrame.log   
        self.paths              = mainFrame.paths

        self.tags = tags
        self.branches = branches
     
        self.cballtags.Clear()
        self.cballtags.SetValue("")
        self.tagname.Clear()
        self.tagcomment.Clear()
        
        for tag in tags:            
            self.cballtags.Append(tag)
        
        self.cbbranches.Clear()
        self.cbbranches.SetValue("")
        for branch in branches:
            self.cbbranches.Append(branch)            
            
 
    def EvtCheckBox(self, evt):        
        chkbox = evt.GetEventObject()
        if chkbox.GetName() == "branch":
            if  evt.IsChecked():             
                self.btn_tag.SetLabel("Branch")
            else:
                self.btn_tag.SetLabel("Tag")

        
    def Click(self, event):
        btn = event.GetEventObject()
        tag = self.tagname.GetValue()
        cmd = btn.GetName()
        wx.BeginBusyCursor()        
        if  cmd == 'tag':
            self.dotag()
        elif cmd == 'del':
            self.deltag()       
        elif cmd == 'refresh':
            self.refresh()     
        elif cmd == 'check':
            self.doCheck()       
        else:
            sys.exit(1)
            
        wx.EndBusyCursor()

    def getCheckOneMod(self, mod,refer_type,refer_name):
        svnpath =  self.server+mod+refer_type
        syscmd =  "svn list   "+ svnpath 
        stdoutfd = os.popen( syscmd )   
        
        diffs = ''       
        for diff in stdoutfd.readlines():          
            diffs += diff 
            diff.strip()
        
        msg = ''
        if diffs.find(refer_name) == -1 :
            msg += mod+"\n"
            wx.LogMessage(msg) 
            
        return msg
            
    def doCheck(self):                 
        self.log.Clear()
        
        bBranch = self.opt_branch.IsChecked()
        refer_name = ''
        refer_type = ''
        if bBranch:         
            refer_name = self.cbbranches.GetValue()           
            refer_type  = "/branches/"
            if refer_name== "":
                self.errorMessage('Please select  Branch Name !')
                return
        else:            
            refer_name = self.cballtags.GetValue()  
            refer_type  = "/tags/"
            if refer_name== "":
                self.errorMessage('Please select Tag Name !')
                return
        msg_begin= "Search " +refer_name+" :\n"+"-" * 120  + "\n"
        wx.LogMessage(msg_begin) 
        msg_content = ''
        #wx.BeginBusyCursor()        
        if self.mod == self.projectname:  
            modmax = len(self.paths)
            dlg = wx.ProgressDialog("gsp tag tool",
                               "Busying check , please waiting ...",
                               maximum = modmax,
                               parent=self,
                               style = #wx.PD_CAN_ABORT
                                  wx.PD_APP_MODAL
                                #| wx.PD_ELAPSED_TIME
                                #| wx.PD_ESTIMATED_TIME
                                | wx.PD_REMAINING_TIME
                                )
            curmod = 1                      
            for  mod, pathlist in self.paths:                 
                msg_content += self.getCheckOneMod(mod,refer_type,refer_name)
                if curmod < modmax:
                    dlg.Update(curmod)
                    curmod += 1  
            dlg.Destroy()    
        else:
            msg_content += self.getCheckOneMod(self.mod,refer_type,refer_name)
        
        msg_end = "-" * 120  + "\n"
        if msg_content.strip() !='' :
            msg_end += "Couldn't find!"
        else :
            msg_end += "Successful!"
            
        wx.LogMessage(msg_end) 
        #wx.EndBusyCursor()
                        
    # When the user selects something, we go here.
    def EvtComboBox(self, evt):
        cb = evt.GetEventObject()
         
        if cb == self.cballtags:
            self.bRefBranch = False
            self.cbbranches.Clear()
            self.cbbranches.SetValue("")
            for branch in self.branches:
                self.cbbranches.Append(branch)              
        else:
            self.bRefBranch = True
            self.cballtags.Clear()
            self.cballtags.SetValue("")        
            for tag in self.tags:            
                self.cballtags.Append(tag)                        
        
    def errorMessage(self, msg):
       dlg = wx.MessageDialog(self, msg, 'Warning', wx.OK | wx.ICON_INFORMATION)
       dlg.ShowModal()
       dlg.Destroy()
    
    def refresh(self):
        #wx.BusyInfo("Refresh subversion tag, please wait ... ")
        self.mainFrame.RecreateSvnTree(True)
        
    def deltag(self):
        bVerbse  = self.opt_verbose.IsChecked()
        bBranch = self.opt_branch.IsChecked()
        bTest      = self.opt_test.IsChecked()        
        tagmsg   = self.tagcomment.GetValue()
        
        if  self.bRefBranch:
            tagOrBranch  = "/branches/"
            tagName  =  self.cbbranches.GetValue()  
        else:
            tagOrBranch  = "/tags/"
            tagName =  self.cballtags.GetValue()
        
        if tagName== ""  or tagName== "trunk" or tagName == "None":
            self.errorMessage('Please select the correct tag name to be delete !')
            return   
         
        dest = tagOrBranch+  tagName   
        
        def  delonemod(self, path): 
            source = path + dest
            syscmd = "svn delete   "+ source +"   "+" -m " + "\""+  tagmsg +"\""             
          
            if  bVerbse :
               self.log.write("-" * 120 + " \n" )             
               self.log.write("[svn command] ->  " + syscmd +" \n")    
            
            if not bTest :                     
                os.system(syscmd)               
      
        if self.mod == self.projectname: 
             modmax = len(self.paths)
             dlg = wx.ProgressDialog("gsp tag tool",
                               "deleting tag/branch , please waiting ...",
                               maximum = modmax,
                               parent=self,
                               style = #wx.PD_CAN_ABORT
                                  wx.PD_APP_MODAL
                                #| wx.PD_ELAPSED_TIME
                                #| wx.PD_ESTIMATED_TIME
                                | wx.PD_REMAINING_TIME
                                )
             curmod = 1                       
             for p, pathlist in self.paths:                 
                 serverpath =  self.remote+ p          
                 delonemod(self, serverpath)
                 if curmod < modmax:
                    dlg.Update(curmod)
                    curmod += 1  
             dlg.Destroy()

             syscmd = "svn commit " + self.projectxml + " -m " + "\"deleting tag in project xml file\" "
             if  bVerbse :
                 self.log.write("-" * 120 + " \n")         
                 self.log.write("[svn command] ->  " + syscmd + "\n")

             if not bTest :                 
                 tagutil.delsvntag(self.projectxml, tagName, tagOrBranch)
                 os.system(syscmd)                
                   
        else:                    
             svnpath =   self.remote+ self.mod 
             delonemod(self, svnpath)  

        if not bTest:
            if  self.bRefBranch:           
                    self.cbbranches.Delete( self.cbbranches.GetSelection()  )
                    self.cbbranches.SetValue("")
            else:            
                    self.cballtags.Delete( self.cballtags.GetSelection()  )
                    self.cballtags.SetValue("")                             
        return        
            
            
        
    def  dotag(self):        
        tagName = self.tagname.GetValue()
        tagMsg   = self.tagcomment.GetValue()  
        revision_name = self.revisionname.GetValue().strip()   
        
        bVerbse  = self.opt_verbose.IsChecked()
        bBranch = self.opt_branch.IsChecked()
        bTest      = self.opt_test.IsChecked()
        
        if len(re.findall('\D+',revision_name))>0 :
            self.errorMessage("Please type valide version number!")
            return 
        
        if  self.bRefBranch:
            refer = self.cbbranches.GetValue()                        
            source_path = "/branches/" + refer
        else:
            refer = self.cballtags.GetValue()
            if refer == "trunk":
                source_path = "/trunk"
            else:
                source_path = "/tags/" + refer
        
        if  refer == "None" or refer == "" :
            useTreeRefTag = True
        else:
            useTreeRefTag = False    

        if bBranch:                     
            dest_path  = "/branches/" + tagName
            tagOrBranch = "/branches/"
        else:            
            dest_path  = "/tags/" + tagName
            tagOrBranch = "/tags/"
            
        if tagName== "":
           self.errorMessage('Please Input The New Tag Or Branch Name !')
           return   
        if  self.mod!= self.projectname:
            if refer== "" or refer =="None":
                self.errorMessage('Please Select The Correct Tag To Refer !')
                return   
            
        def  tagonemod(self, path ):            
            source = path +source_path                
            dest =  path+ dest_path
            revison_str = ''
            if revision_name!='' :
                revison_str = ' -r '+revision_name+' '
                  
            syscmd = "svn copy  "+revison_str+" "+ source +"     "+dest+"      -m " + "\""+  tagMsg +"\""             
            
            if  bVerbse :
               self.log.write("-" * 120 + " \n")         
               self.log.write("[svn command] ->  " + syscmd + "\n")    
            
            if not bTest :                     
                os.system(syscmd)                

     
        if self.mod == self.projectname:                 
            reftags = []
            submods = []   
            modmax = len(self.paths)
            dlg = wx.ProgressDialog("gsp tag tool",
                               "Do tag/branch , please waiting ...",
                               maximum = modmax,
                               parent=self,
                               style = #wx.PD_CAN_ABORT
                                  wx.PD_APP_MODAL
                                #| wx.PD_ELAPSED_TIME
                                #| wx.PD_ESTIMATED_TIME
                                | wx.PD_REMAINING_TIME
                                )
            curmod = 1
            
            for p, pathlist in self.paths: 
                svnpath = self.remote+ p                
                if useTreeRefTag: 
                    ( source_path, reftag) = self.getRefPath( pathlist[len(pathlist) - 1], bBranch )
                else:
                     reftag = refer    
                submods.append(p)
                reftags.append(reftag)
                tagonemod(self, svnpath)
                if curmod < modmax:
                    dlg.Update(curmod)
                    curmod += 1  
            
            dlg.Destroy()
            
       
                
            syscmd = "svn commit " + self.projectxml + " -m " + "\"Adding tag in project xml file\""
            if  bVerbse :
               self.log.write("-" * 120 + " \n")         
               self.log.write("[svn command] ->  " + syscmd + "\n")
            
            if not bTest :                
                tagutil.addsvntag(self.projectxml, tagName, tagOrBranch, tagMsg, submods, reftags )       
                os.system( syscmd)                
            
            
        else:
             svnpath =   self.remote+ self.mod             
             tagonemod(self, svnpath)                         
   
        if not bTest:
            if  bBranch:
                    if self.cbbranches.GetCount() == 0:
                        self.cbbranches.Append(tagName)
                    else:
                        self.cbbranches.Insert(tagName, 1)
            else:            
                    self.cballtags.Insert(tagName, 1)

            #self.mainFrame.RecreateSvnTree(True)                
        return        
        
    def getRefPath(self,  mod, bBranch ):
        (tagorBranch, refer) = self.mainFrame.svnTreeTagSelected[ mod ]
         
        if  tagorBranch == "branches":           
            source_path = "/branches/" + refer
        else:            
            if refer == "trunk":
                source_path = "/trunk"
            else:
                source_path = "/tags/" + refer       
            
        return source_path, refer 
    
#----------------------------------------------------------------------

def runTest(frame, nb):
    win = TestPanel(frame,  nb)
    return win

#----------------------------------------------------------------------


#---------------------------------------------------------------------------
        
if __name__ == 'main':
                  
    app = wx.PySimpleApp()
    frame = myFrame(None, -1, "Size Test")

    frame.Show()

    app.MainLoop()


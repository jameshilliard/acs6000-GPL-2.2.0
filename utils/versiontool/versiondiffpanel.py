#!/usr/bin/env python
#----------------------------------------------------------------------------
# This program is diff panel.

# Author: Tercel Yi
# History: Draft on , 2008-1-30
#
# Copyright:    (c) Avocent
#----------------------------------------------------------------------------

import wx
import wx.combo
import os
import sys

from lib.config import *
import lib.datacommon as datacommon
  
#---------------------------------------------------------------------------
# A class to be used to display source code in the demo.  Try using the
# wxSTC in the StyledTextCtrl_2 sample first, fall back to wxTextCtrl
# if there is an error, such as the stc module not being present.
#

try:
    ##raise ImportError     # for testing the alternate implementation
    from wx import stc
    from lib.tagStyledTextCtrl import PythonSTC

    class DemoCodeEditor(PythonSTC):
        def __init__(self, parent):
            PythonSTC.__init__(self, parent, -1, style=wx.BORDER_NONE)
            self.SetUpEditor()

        # Some methods to make it compatible with how the wxTextCtrl is used
        def SetValue(self, value):
            if wx.USE_UNICODE:
                value = value.decode('iso8859_1')
            self.SetText(value)
            self.EmptyUndoBuffer()
            self.SetSavePoint()

        def IsModified(self):
            return self.GetModify()

        def Clear(self):
            self.ClearAll()

        def SetInsertionPoint(self, pos):
            self.SetCurrentPos(pos)
            self.SetAnchor(pos)

        def ShowPosition(self, pos):
            line = self.LineFromPosition(pos)
            #self.EnsureVisible(line)
            self.GotoLine(line)

        def GetLastPosition(self):
            return self.GetLength()

        def GetPositionFromLine(self, line):
            return self.PositionFromLine(line)

        def GetRange(self, start, end):
            return self.GetTextRange(start, end)

        def GetSelection(self):
            return self.GetAnchor(), self.GetCurrentPos()

        def SetSelection(self, start, end):
            self.SetSelectionStart(start)
            self.SetSelectionEnd(end)

        def SelectLine(self, line):
            start = self.PositionFromLine(line)
            end = self.GetLineEndPosition(line)
            self.SetSelection(start, end)
            
        def SetUpEditor(self):
            """
            This method carries out the work of setting up the demo editor.            
            It's seperate so as not to clutter up the init code.
            """
            import keyword
            
            self.SetLexer(stc.STC_LEX_PYTHON)
            self.SetKeyWords(0, " ".join(keyword.kwlist))
    
            # Enable folding
            self.SetProperty("fold", "1" ) 

            # Highlight tab/space mixing (shouldn't be any)
            self.SetProperty("tab.timmy.whinge.level", "1")

            # Set left and right margins
            self.SetMargins(2,2)
    
            # Set up the numbers in the margin for margin #1
            self.SetMarginType(1, wx.stc.STC_MARGIN_NUMBER)
            # Reasonable value for, say, 4-5 digits using a mono font (40 pix)
            self.SetMarginWidth(1, 40)
    
            # Indentation and tab stuff
            self.SetIndent(4)               # Proscribed indent size for wx
            self.SetIndentationGuides(True) # Show indent guides
            self.SetBackSpaceUnIndents(True)# Backspace unindents rather than delete 1 space
            self.SetTabIndents(True)        # Tab key indents
            self.SetTabWidth(4)             # Proscribed tab size for wx
            self.SetUseTabs(False)          # Use spaces rather than tabs, or
                                            # TabTimmy will complain!    
            # White space
            self.SetViewWhiteSpace(False)   # Don't view white space
    
            # EOL: Since we are loading/saving ourselves, and the
            # strings will always have \n's in them, set the STC to
            # edit them that way.            
            self.SetEOLMode(wx.stc.STC_EOL_LF)
            self.SetViewEOL(False)
            
            # No right-edge mode indicator
            self.SetEdgeMode(stc.STC_EDGE_NONE)
    
            # Setup a margin to hold fold markers
            self.SetMarginType(2, stc.STC_MARGIN_SYMBOL)
            self.SetMarginMask(2, stc.STC_MASK_FOLDERS)
            self.SetMarginSensitive(2, True)
            self.SetMarginWidth(2, 12)
    
            # and now set up the fold markers
            self.MarkerDefine(stc.STC_MARKNUM_FOLDEREND,     stc.STC_MARK_BOXPLUSCONNECTED,  "white", "black")
            self.MarkerDefine(stc.STC_MARKNUM_FOLDEROPENMID, stc.STC_MARK_BOXMINUSCONNECTED, "white", "black")
            self.MarkerDefine(stc.STC_MARKNUM_FOLDERMIDTAIL, stc.STC_MARK_TCORNER,  "white", "black")
            self.MarkerDefine(stc.STC_MARKNUM_FOLDERTAIL,    stc.STC_MARK_LCORNER,  "white", "black")
            self.MarkerDefine(stc.STC_MARKNUM_FOLDERSUB,     stc.STC_MARK_VLINE,    "white", "black")
            self.MarkerDefine(stc.STC_MARKNUM_FOLDER,        stc.STC_MARK_BOXPLUS,  "white", "black")
            self.MarkerDefine(stc.STC_MARKNUM_FOLDEROPEN,    stc.STC_MARK_BOXMINUS, "white", "black")
    
            # Global default style
            if wx.Platform == '__WXMSW__':
                self.StyleSetSpec(stc.STC_STYLE_DEFAULT, 
                                  'fore:#000000,back:#FFFFFF,face:Courier New,size:9')
            elif wx.Platform == '__WXMAC__':
                # TODO: if this looks fine on Linux too, remove the Mac-specific case 
                # and use this whenever OS != MSW.
                self.StyleSetSpec(stc.STC_STYLE_DEFAULT, 
                                  'fore:#000000,back:#FFFFFF,face:Courier')
            else:
                self.StyleSetSpec(stc.STC_STYLE_DEFAULT, 
                                  'fore:#000000,back:#FFFFFF,face:Courier,size:9')
    
            # Clear styles and revert to default.
            self.StyleClearAll()

            # Following style specs only indicate differences from default.
            # The rest remains unchanged.

            # Line numbers in margin
            self.StyleSetSpec(wx.stc.STC_STYLE_LINENUMBER,'fore:#000000,back:#99A9C2')    
            # Highlighted brace
            self.StyleSetSpec(wx.stc.STC_STYLE_BRACELIGHT,'fore:#00009D,back:#FFFF00')
            # Unmatched brace
            self.StyleSetSpec(wx.stc.STC_STYLE_BRACEBAD,'fore:#00009D,back:#FF0000')
            # Indentation guide
            self.StyleSetSpec(wx.stc.STC_STYLE_INDENTGUIDE, "fore:#CDCDCD")
    
            # Python styles
            self.StyleSetSpec(wx.stc.STC_P_DEFAULT, 'fore:#000000')
            # Comments
            self.StyleSetSpec(wx.stc.STC_P_COMMENTLINE,  'fore:#008000,back:#F0FFF0')
            self.StyleSetSpec(wx.stc.STC_P_COMMENTBLOCK, 'fore:#008000,back:#F0FFF0')
            # Numbers
            self.StyleSetSpec(wx.stc.STC_P_NUMBER, 'fore:#008080')
            # Strings and characters
            self.StyleSetSpec(wx.stc.STC_P_STRING, 'fore:#800080')
            self.StyleSetSpec(wx.stc.STC_P_CHARACTER, 'fore:#800080')
            # Keywords
            self.StyleSetSpec(wx.stc.STC_P_WORD, 'fore:#000080,bold')
            # Triple quotes
            self.StyleSetSpec(wx.stc.STC_P_TRIPLE, 'fore:#800080,back:#FFFFEA')
            self.StyleSetSpec(wx.stc.STC_P_TRIPLEDOUBLE, 'fore:#800080,back:#FFFFEA')
            # Class names
            self.StyleSetSpec(wx.stc.STC_P_CLASSNAME, 'fore:#0000FF,bold')
            # Function names
            self.StyleSetSpec(wx.stc.STC_P_DEFNAME, 'fore:#008080,bold')
            # Operators
            self.StyleSetSpec(wx.stc.STC_P_OPERATOR, 'fore:#800000,bold')
            # Identifiers. I leave this as not bold because everything seems
            # to be an identifier if it doesn't match the above criterae
            self.StyleSetSpec(wx.stc.STC_P_IDENTIFIER, 'fore:#000000')

            # Caret color
            self.SetCaretForeground("BLUE")
            # Selection background
            self.SetSelBackground(1, '#66CCFF')

            self.SetSelBackground(True, wx.SystemSettings_GetColour(wx.SYS_COLOUR_HIGHLIGHT))
            self.SetSelForeground(True, wx.SystemSettings_GetColour(wx.SYS_COLOUR_HIGHLIGHTTEXT))

        def RegisterModifiedEvent(self, eventHandler):
            self.Bind(wx.stc.EVT_STC_CHANGE, eventHandler)


except ImportError:
    class DemoCodeEditor(wx.TextCtrl):
        def __init__(self, parent):
            wx.TextCtrl.__init__(self, parent, -1, style =
                                 wx.TE_MULTILINE | wx.HSCROLL | wx.TE_RICH2 | wx.TE_NOHIDESEL)

        def RegisterModifiedEvent(self, eventHandler):
            self.Bind(wx.EVT_TEXT, eventHandler)

        def SetReadOnly(self, flag):
            self.SetEditable(not flag)
            # NOTE: STC already has this method
    
        def GetText(self):
            return self.GetValue()

        def GetPositionFromLine(self, line):
            return self.XYToPosition(0,line)

        def GotoLine(self, line):
            pos = self.GetPositionFromLine(line)
            self.SetInsertionPoint(pos)
            self.ShowPosition(pos)

        def SelectLine(self, line):
            start = self.GetPositionFromLine(line)
            end = start + self.GetLineLength(line)
            self.SetSelection(start, end)

#----------------------------------------------------------------------       
# Constants for module versions

modOriginal = 0
modModified = 1
modDefault = modOriginal 
#----------------------------------------------------------------------
class DiffPanel(wx.Panel):
    """Panel for the 'Demo Code' tab"""
    def __init__(self, parent, mainFrame, mod):
        wx.Panel.__init__(self, parent, size=(1,1))        
     
        self.mainFrame = mainFrame
        self.log = mainFrame.log
        self.package_list = self.mainFrame.objProjectData.package_list
        self.remote = self.mainFrame.objProjectData.remote
        self.local = self.mainFrame.objProjectData.local
        self.treePydata = self.mainFrame.treePydata
        self.filepath = ''
        self.controls_data_dict=  {'version_list':[], 'version_name_list':[], 'version_active':''}
        self.version_list = []
        self.version_name_list = []
        self.version_active = ''
            
        if 'wxMSW' in wx.PlatformInfo:
            self.Hide()   
        
        if self.treePydata['type']=='root':
            self.initProjectData()
        elif self.treePydata['type']=='list':
            self.initPackageData() 

        self.createControls()
        
    def __del__(self):
        pass
    
    def errorMessage(self, msg):
       dlg = wx.MessageDialog(self, msg, 'Warning', wx.OK | wx.ICON_INFORMATION)
       dlg.ShowModal()
       dlg.Destroy()
        
    def initProjectData(self):
        self.filepath = datacommon.getProjectVersionXmlPath(self.mainFrame.projectxml)
        self.controls_data_dict = datacommon.getProjectVersionData(self.filepath)
     
    def initPackageData(self):
        self.filepath = DEFAULT_PACKAGESDATA_DIR+'/'+self.treePydata['dir']+DEFAULT_DATA_SUFFIX
        self.controls_data_dict = datacommon.getPackageData(self.filepath)
        

    def createControls(self):  
        
        mysize = (200,-1)  
        cb1_list = []
        if self.treePydata['type']=='root':
            cb1_list = [DEAFULT_HEAD]

        cb1_list.extend(self.controls_data_dict['version_name_list'])
        
        self.cb1 = wx.ComboBox(self, choices=cb1_list)
        if self.treePydata['type']=='root':
            self.cb1.SetValue(DEAFULT_HEAD)
        elif self.treePydata['type']=='list':
            if len(cb1_list)>0 :
                self.cb1.SetValue(cb1_list[0])
                
        #self.cb1.SetMinSize(mysize) 
        self.cb2 = wx.ComboBox(self, choices =self.controls_data_dict['version_name_list'])
        self.cb2.SetValue(self.controls_data_dict['version_active'])
        #self.cb2.SetMinSize(mysize) 
        
        self.Bind(wx.EVT_COMBOBOX, self.EvtComboBoxV1, self.cb1) 
        self.Bind(wx.EVT_COMBOBOX, self.EvtComboBoxV2, self.cb2) 
        
        self.editor = DemoCodeEditor(self)
        
        self.btnDiff = wx.Button(self, -1, "Diff")
        self.btnSave = wx.Button(self, -1, "Save")
 
        self.btnSave.Bind(wx.EVT_BUTTON, self.OnSave)
        self.btnDiff.Bind(wx.EVT_BUTTON, self.OnDiff)  
        
            
        self.controlBox = wx.BoxSizer(wx.HORIZONTAL)
        self.controlBox.Add((46,0))
        self.controlBox.Add(wx.StaticText(self, -1, "Select:"),0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL,5)
        self.controlBox.Add(self.cb1,0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL,5)   
        self.controlBox.Add(wx.StaticText(self, -1, "v"),0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL,5)
        self.controlBox.Add(self.cb2,0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL,5)  
        self.controlBox.Add(self.btnDiff,0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL,5)
        self.controlBox.Add(self.btnSave,0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL,5)


        self.box = wx.BoxSizer(wx.VERTICAL)
        self.box.Add(self.controlBox, 0, wx.EXPAND|wx.ALL, 6)
        self.box.Add(wx.StaticLine(self), 0, wx.EXPAND)
        self.box.Add(self.editor, 1, wx.EXPAND)
        
        self.box.Fit(self)
        self.SetSizer(self.box)

    def treeModChange(self, mainFrame, mod, tagAndBranch, tags ):
        self.editor.Clear()

    def EvtComboBoxV1(self, evt):
        selected = self.cb1.GetValue() 
        if selected!=DEAFULT_HEAD :
            self.showVersionInfo(selected) 
        else :
            self.log.Clear()
            
    def EvtComboBoxV2(self, evt):
        selected = self.cb2.GetValue() 
        self.showVersionInfo(selected) 
    
    def showVersionInfo(self,selected):
        version_dict = {}
        if self.treePydata['type']=='root':
            version_dict = datacommon.getOneProjectVersion(self.controls_data_dict['version_list'],selected)  
        elif self.treePydata['type']=='list':
            version_dict = datacommon.getOnePackageVersion(self.controls_data_dict['version_list'],selected)
            
        self.log.Clear()
        msg = ""
        msg += "name: "+selected+"\n"
        msg += "time: "+version_dict['time']+"\n"
        msg += "comment: "+version_dict['comment']+"\n"
        self.log.WriteText(msg) 
                    
            
    def OnDiff(self, event):       
        wx.BeginBusyCursor()     
               
        if self.treePydata['type']=='root':
            self.doProjectDiff()
        elif self.treePydata['type']=='list':
            self.doPackageDiff()
            
        wx.EndBusyCursor()
        
    def OnSave(self, event):
        
        dlg = wx.FileDialog(
            self, message="Save diffs file as ...", defaultDir=os.getcwd(), 
            defaultFile="", style=wx.SAVE )

        dlg.SetFilterIndex(2)

        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
        else:
            return 

        if os.path.exists(path):
            dlg = wx.MessageDialog(self, 'Are you sure to replace the existing file?',
                               'A Message Box',
                               wx.ICON_QUESTION
                               |wx.YES_NO | wx.NO_DEFAULT | wx.CANCEL 
                               )
            
            if dlg.ShowModal() != 5103:
                return 
        # Save
        f = open(path, "wt")
        diffs = self.editor.GetText()
        if wx.USE_UNICODE:
                diffs = diffs.encode('utf-8')
        try:
            f.write(diffs)
        finally:
            f.close()            
            wx.LogMessage(" %s  saved!" % path )
       
    def doProjectDiff(self):

        version_name1 = self.cb1.GetValue()  
        version_name2 = self.cb2.GetValue()  
        if version_name1 == DEAFULT_HEAD :
            version_package_list1 = datacommon.getActiveVersionPackages(self.package_list)
        else :
            version_package_list1 = datacommon.getOneProjectVersionPackages(version_name1,self.filepath)
        
        version_package_list2 = datacommon.getOneProjectVersionPackages(version_name2,self.filepath)
        
        self.isEqual = True
        
        def getEqual(b):
            isEqual = b
            return isEqual
        
        def getV2Str(dir,revision_number) :
            for package_dict in version_package_list2:
                if package_dict['dir'] == dir :
                    if package_dict['revision_number'] == revision_number :
                        return " == ["+package_dict['revision_number']+"]"
                    else :
                        self.isEqual = False
                        return " <> ["+package_dict['revision_number']+"]"
            return ""
        
        def checkInV1(dir) :
            rtn = False
            for package_dict in version_package_list1:
                if package_dict['dir'] == dir :
                    rtn = True
                    break
                
            return rtn
        
        str = ''
        str += "\n"
        for package_dict in version_package_list1:
            v2_str = getV2Str(package_dict['dir'],package_dict['revision_number'])
            isadd_str = "  "
            
            if v2_str== "" :
                isadd_str = "+ "
                self.isEqual = False
                
            str += isadd_str+package_dict['dir'] +" : [" + package_dict['revision_number']+"]"+v2_str+"\n"  

        for package_dict in version_package_list2:
            if not checkInV1(package_dict['dir']) :
                self.isEqual = False
                str += "- "+package_dict['dir'] +" : [" + package_dict['revision_number']+"]\n"
        
        str_result = ' <> '
        if  self.isEqual :
            str_result = ' == '
            
        str_head =  "# "+version_name1+str_result+version_name2+" in "+self.treePydata['treenav']+".\n" 
        self.editor.Clear()
        self.editor.AppendText(str_head+str)
        
    def doPackageDiff(self):

        v1 = self.cb1.GetValue()  
        v2 = self.cb2.GetValue()  
        
        if v1.strip() == '' or v2=='':
            self.errorMessage("Please select! ")
            return 
        if v1>=v2:
            self.errorMessage("Please check the former is less-than the latter! ")
            return

        str = ''
        str += "# "+v1+" differ from "+v2+" in "+self.treePydata['treenav']+".\n" 
        str += "\n"
        
        svnpath =  self.local+"/"+self.treePydata['dir']
        syscmd =  "svn diff -r "+v1+":"+v2+" "+ svnpath 
        stdoutfd = os.popen( syscmd )   
        
        diffs = ''          
        files = "File Changed List: \n"  
        files += "-" * 80 + "\n"  
        for diff in stdoutfd.readlines():          
            diffs += diff 
            diff.strip()
            if  diff.startswith("Index:"):
                file= diff[7:].strip()                 
                files += file + "\n"
            
        self.log.Clear()
        wx.LogMessage( "[svn command] ->  %s" % syscmd ) 
        
        self.editor.Clear()
        self.editor.AppendText(files)
        self.editor.AppendText("#" * 80 + "\n\n"  )
        self.editor.AppendText(diffs)
      
               
    # Loads a demo from a DemoModules object
    def LoadDemo(self, demoModules):
        self.demoModules = demoModules
        if (modDefault == modModified) and demoModules.Exists(modModified):
            demoModules.SetActive(modModified)
        else:
            demoModules.SetActive(modOriginal)
        #self.radioButtons[demoModules.GetActiveID()].Enable(True)
        self.ActiveModuleChanged()


    def ActiveModuleChanged(self):
        self.LoadDemoSource( self.demoModules.GetSource() )
        self.UpdateControlState()
        self.mainFrame.Freeze()        
        self.ReloadDemo()
        self.mainFrame.Thaw()

        
    def LoadDemoSource(self, source):
        self.editor.Clear()
        #self.editor.SetValue(source)
        self.JumpToLine(0)
        self.btnSave.Enable(False)


    def JumpToLine(self, line, highlight=False):
        self.editor.GotoLine(line)
        self.editor.SetFocus()
        if highlight:
            self.editor.SelectLine(line)
        
       
    def UpdateControlState(self):
        self.btnSave.Enable(True)
        self.btnDiff.Enable(True)


    def ReloadDemo(self):
        if self.demoModules.name != __name__:
            self.mainFrame.RunModule()

    def OnRestore(self, event): # Handles the "Delete Modified" button
        modifiedFilename = GetModifiedFilename(self.demoModules.name)
        self.demoModules.Delete(modModified)
        os.unlink(modifiedFilename) # Delete the modified copy
        busy = wx.BusyInfo("Reloading demo module...")
        
        self.ActiveModuleChanged()

        self.mainFrame.SetTreeModified(False)



#---------------------------------------------------------------------------
        
if __name__ == '__main__':
                  
    app = wx.PySimpleApp()
    frame = myFrame(None, -1, "Size Test")

    frame.Show()

    app.MainLoop()


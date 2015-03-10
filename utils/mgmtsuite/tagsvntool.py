#!/usr/bin/env python
#----------------------------------------------------------------------------
# Name:         tagsvntool.py
# Purpose:      svn tag tool 
#
# Author:      Time Shi (time.shi@avocent.com)
#
# Copyright:    (c) 2007 Avocent
#----------------------------------------------------------------------------

import sys, os, time, traceback, types
import wx                  
import wx.aui
import wx.html
#from wx.lib.mixins.treemixin import ExpansionState

import tagimages
import tagutil
import webtransfer
# For debugging
##wx.Trap();
##print "wx.VERSION_STRING = %s (%s)" % (wx.VERSION_STRING, wx.USE_UNICODE and 'unicode' or 'ansi')
##print "pid:", os.getpid()
##raw_input("Press Enter...")


#---------------------------------------------------------------------------
SVN_MODE =  True
USE_CUSTOMTREECTRL = True
ALLOW_AUI_FLOATING = False
DEFAULT_PERSPECTIVE = "Default Perspective"

#---------------------------------------------------------------------------

_demoPngs = ["overview", "recent", "frame", "dialog", "moredialog", "core",
             "book", "customcontrol", "morecontrols", "layout", "process", "clipboard",
             "images", "miscellaneous"]


#---------------------------------------------------------------------------
# Show how to derive a custom wxLog class

class MyLog(wx.PyLog):
    def __init__(self, textCtrl, logTime=0):
        wx.PyLog.__init__(self)
        self.tc = textCtrl
        self.logTime = logTime
 
    def DoLogString(self, message, timeStamp):
        #print message, timeStamp
        #if self.logTime:
        #    message = time.strftime("%X", time.localtime(timeStamp)) + \
        #              ": " + message
        if self.tc:
            self.tc.AppendText(message + '\n')


class MyTP(wx.PyTipProvider):
    def GetTip(self):
        return "This is my tip"

#---------------------------------------------------------------------------
# A class to be used to simply display a message in the demo pane
# rather than running the sample itself.

class MessagePanel(wx.Panel):
    def __init__(self, parent, message, caption='', flags=0):
        wx.Panel.__init__(self, parent)

        # Make widgets
        if flags:
            artid = None
            if flags & wx.ICON_EXCLAMATION:
                artid = wx.ART_WARNING            
            elif flags & wx.ICON_ERROR:
                artid = wx.ART_ERROR
            elif flags & wx.ICON_QUESTION:
                artid = wx.ART_QUESTION
            elif flags & wx.ICON_INFORMATION:
                artid = wx.ART_INFORMATION

            if artid is not None:
                bmp = wx.ArtProvider.GetBitmap(artid, wx.ART_MESSAGE_BOX, (32,32))
                icon = wx.StaticBitmap(self, -1, bmp)
            else:
                icon = (32,32) # make a spacer instead

        if caption:
            caption = wx.StaticText(self, -1, caption)
            caption.SetFont(wx.Font(28, wx.SWISS, wx.NORMAL, wx.BOLD))

        message = wx.StaticText(self, -1, message)

        # add to sizers for layout
        tbox = wx.BoxSizer(wx.VERTICAL)
        if caption:
            tbox.Add(caption)
            tbox.Add((10,10))
        tbox.Add(message)
        
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((10,10), 1)
        hbox.Add(icon)
        hbox.Add((10,10))
        hbox.Add(tbox)
        hbox.Add((10,10), 1)

        box = wx.BoxSizer(wx.VERTICAL)
        box.Add((10,10), 1)
        box.Add(hbox, 0, wx.EXPAND)
        box.Add((10,10), 2)

        self.SetSizer(box)
        self.Fit()
        
    
#---------------------------------------------------------------------------
# A class to be used to display source code in the demo.  Try using the
# wxSTC in the StyledTextCtrl_2 sample first, fall back to wxTextCtrl
# if there is an error, such as the stc module not being present.
#

try:
    ##raise ImportError     # for testing the alternate implementation
    from wx import stc
    from tagStyledTextCtrl import PythonSTC

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

       

#---------------------------------------------------------------------------
# Constants for module versions

modOriginal = 0
modModified = 1
modDefault = modOriginal
#---------------------------------------------------------------------------
class DemoCodePanel(wx.Panel):
    """Panel for the 'Demo Code' tab"""
    def __init__(self, parent, mainFrame, mod):
        wx.Panel.__init__(self, parent, size=(1,1))        
     
        self.mod  = mod
        self.projectxml       = mainFrame.projectxml
        self.projectname    = mainFrame.projectname
        self.server              = mainFrame.server        
        self.remote            = mainFrame.remote        
        self.paths                = mainFrame.paths
            
        if 'wxMSW' in wx.PlatformInfo:
            self.Hide()
        self.mainFrame = mainFrame
        self.editor = DemoCodeEditor(self)
        
        self.btnDiff = wx.Button(self, -1, "Diff")
        self.btnSave = wx.Button(self, -1, "Save")
 
        self.btnSave.Bind(wx.EVT_BUTTON, self.OnSave)
        self.btnDiff.Bind(wx.EVT_BUTTON, self.OnDiff)
    
        tags = []
        tags.extend(mainFrame.projectTags)
        tags.extend(mainFrame.projectBranches)
        self.tags = mainFrame.projectTags
        mysize = (200,-1)  
        self.cb1 = wx.ComboBox(self, choices=tags)
        self.cb1.SetMinSize(mysize) 
        self.cb2 = wx.ComboBox(self, choices =tags)
        self.cb2.SetMinSize(mysize) 
        self.controlBox = wx.BoxSizer(wx.HORIZONTAL)
        self.controlBox.Add(wx.StaticText(self, -1, "tag1:"), 1,  5)
        self.controlBox.Add(self.cb1, 6, 5)   
        self.controlBox.Add(wx.StaticText(self, -1, "tag2:"), 1,  5)
        self.controlBox.Add(self.cb2, 6,  5)  
        self.controlBox.Add(self.btnDiff, 3, wx.RIGHT, 5)
        self.controlBox.Add(self.btnSave, 3 )

        self.box = wx.BoxSizer(wx.VERTICAL)
        self.box.Add(self.controlBox, 0, wx.EXPAND)
        self.box.Add(wx.StaticLine(self), 0, wx.EXPAND)
        self.box.Add(self.editor, 1, wx.EXPAND)
        
        self.box.Fit(self)
        self.SetSizer(self.box)

    def treeModChange(self, mainFrame, mod, tagAndBranch, tags ):
        self.mod  = mod
        self.projectxml       = mainFrame.projectxml
        self.projectname    = mainFrame.projectname
        self.server              = mainFrame.server        
        self.remote            = mainFrame.remote        
        self.paths                = mainFrame.paths
        
        self.cb1.Clear()
        self.cb2.Clear()
        self.editor.Clear()
        self.mod  = mod
        self.tags  = tags
        for tag in tagAndBranch:            
            self.cb1.Append(tag)
            self.cb2.Append(tag)
            
    def getDiffOneMod(self, mod):
   
        tag1 = self.cb1.GetValue()  
        if tag1 == "trunk":
            svnpath1 = self.server + mod + "/trunk"
        else:
            if tag1 in self.tags:
                tagOrBranch= "/tags/"
            else:
                tagOrBranch= "/branches/"            
            svnpath1 = self.server +mod + tagOrBranch + tag1
            
        tag2 = self.cb2.GetValue()
        if tag2 == "trunk":
            svnpath2 = self.server + mod + "/trunk"
        else:
            if tag2 in self.tags:
                tagOrBranch= "/tags/"
            else:
                tagOrBranch= "/branches/"            
            svnpath2 = self.server + mod + tagOrBranch + tag2
            
        svncmd = "svn diff   "
        syscmd =  svncmd + svnpath1 + "   "  + svnpath2
        stdoutfd = os.popen( syscmd )   
       
        wx.LogMessage("-" * 120  )                  
        wx.LogMessage( "[svn command] ->  %s" % syscmd )
        
        files =""        
        files = "[   "  + mod + "  ]   File Changed List: \n"  
        files += "-" * 80 + "\n"
        
        diffs = ""       
        for diff in stdoutfd.readlines():          
            diffs += diff 
            diff.strip()
            if  diff.startswith("Index:"):
                file= diff[7:].strip()                 
                files += file + "\n"

        files += "-" * 80  + "\n"
        
        return diffs, files
            
            
    def OnDiff(self, event):                 
        tag1 = self.cb1.GetValue()
        tag2 = self.cb2.GetValue()
        if tag1=="" or tag2=="":
            dlg = wx.MessageDialog(self, 'Please select tag name!',
                               'A Message Box',
                               wx.ICON_INFORMATION|wx.OK 
                               )
            
            dlg.ShowModal()
            return 
            
        diffs = ""
        files = ""
        #wx.BeginBusyCursor()        
        if self.mod == self.projectname:  
            modmax = len(self.paths)
            dlg = wx.ProgressDialog("gsp tag tool",
                               "Busying diff , please waiting ...",
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
                (diff, file) = self.getDiffOneMod(mod)
                diffs += diff
                if diff !=  "":
                    files += file
                
                if curmod < modmax:
                    dlg.Update(curmod)
                    curmod += 1  
            dlg.Destroy()    
        else:
            (diffs, files) = self.getDiffOneMod(self.mod)

         
        self.editor.SetValue(files)
        self.editor.AppendText(diffs)
        #wx.EndBusyCursor()
        
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

def opj(path):
    """Convert paths to the platform-specific separator"""
    st = apply(os.path.join, tuple(path.split('/')))
    # HACK: on Linux, a leading / gets lost...
    if path.startswith('/'):
        st = '/' + st
    return st


def GetDataDir():
    """
    Return the standard location on this platform for application data
    """
    sp = wx.StandardPaths.Get()
    return sp.GetUserDataDir()


def GetModifiedDirectory():
    """
    Returns the directory where modified versions of the demo files
    are stored
    """
    return os.path.join(GetDataDir(), "modified")


def GetModifiedFilename(name):
    """
    Returns the filename of the modified version of the specified demo
    """
    if not name.endswith(".py"):
        name = name + ".py"
    return os.path.join(GetModifiedDirectory(), name)


def GetOriginalFilename(name):
    """
    Returns the filename of the original version of the specified demo
    """
    if not name.endswith(".py"):
        name = name + ".py"
    return name


def DoesModifiedExist(name):
    """Returns whether the specified demo has a modified copy"""
    if os.path.exists(GetModifiedFilename(name)):
        return True
    else:
        return False


def GetConfig():
    if not os.path.exists(GetDataDir()):
        os.makedirs(GetDataDir())

    config = wx.FileConfig(
        localFilename=os.path.join(GetDataDir(), "options"))
    return config


def SearchDemo(name, keyword):
    """ Returns whether a demo contains the search keyword or not. """
#    fid = open(GetOriginalFilename(name), "rt")
#    fullText = fid.read()
#    fid.close()
#    if fullText.find(keyword) >= 0:
#        return True
#
#    return False    

#---------------------------------------------------------------------------

class ModuleDictWrapper:
    """Emulates a module with a dynamically compiled __dict__"""
    def __init__(self, dict):
        self.dict = dict
        
    def __getattr__(self, name):
        if name in self.dict:
            return self.dict[name]
        else:
            raise AttributeError

class DemoModules:
    """
    Dynamically manages the original/modified versions of a demo
    module
    """
    def __init__(self, name):
        self.modActive = -1
        self.name = name
        
        #              (dict , source ,  filename , description   , error information )        
        #              (  0  ,   1    ,     2     ,      3        ,          4        )        
        self.modules = [[None,  ""    ,    ""     , "<original>"  ,        None],
                        [None,  ""    ,    ""     , "<modified>"  ,        None]]
        
        # load original module
        self.LoadFromFile(modOriginal, GetOriginalFilename(name))
        self.SetActive(modOriginal)

        # load modified module (if one exists)
        if DoesModifiedExist(name):
           self.LoadFromFile(modModified, GetModifiedFilename(name))


    def LoadFromFile(self, modID, filename):
        self.modules[modID][2] = filename
        file = open(filename, "rt")
        self.LoadFromSource(modID, file.read())
        file.close()


    def LoadFromSource(self, modID, source):
        self.modules[modID][1] = source
        self.LoadDict(modID)


    def LoadDict(self, modID):
        if self.name != __name__:
            source = self.modules[modID][1]
            #description = self.modules[modID][3]
            description = self.modules[modID][2]

            try:
                self.modules[modID][0] = {}
                code = compile(source, description, "exec")        
                exec code in self.modules[modID][0]
            except:
                self.modules[modID][4] = DemoError(sys.exc_info())
                self.modules[modID][0] = None
            else:
                self.modules[modID][4] = None


    def SetActive(self, modID):
        if modID != modOriginal and modID != modModified:
            raise LookupError
        else:
            self.modActive = modID


    def GetActive(self):
        dict = self.modules[self.modActive][0]
        if dict is None:
            return None
        else:
            return ModuleDictWrapper(dict)


    def GetActiveID(self):
        return self.modActive

    
    def GetSource(self, modID = None):
        if modID is None:
            modID = self.modActive
        return self.modules[modID][1]


    def GetFilename(self, modID = None):
        if modID is None:
            modID = self.modActive
        return self.modules[self.modActive][2]


    def GetErrorInfo(self, modID = None):
        if modID is None:
            modID = self.modActive
        return self.modules[self.modActive][4]


    def Exists(self, modID):
        return self.modules[modID][1] != ""


    def UpdateFile(self, modID = None):
        """Updates the file from which a module was loaded
        with (possibly updated) source"""
        if modID is None:
            modID = self.modActive

        source = self.modules[modID][1]
        filename = self.modules[modID][2]

        try:        
            file = open(filename, "wt")
            file.write(source)
        finally:
            file.close()


    def Delete(self, modID):
        if self.modActive == modID:
            self.SetActive(0)

        self.modules[modID][0] = None
        self.modules[modID][1] = ""
        self.modules[modID][2] = ""


#---------------------------------------------------------------------------

class DemoError:
    """Wraps and stores information about the current exception"""
    def __init__(self, exc_info):
        import copy
        
        excType, excValue = exc_info[:2]
        # traceback list entries: (filename, line number, function name, text)
        self.traceback = traceback.extract_tb(exc_info[2])

        # --Based on traceback.py::format_exception_only()--
        if type(excType) == types.ClassType:
            self.exception_type = excType.__name__
        else:
            self.exception_type = excType

        # If it's a syntax error, extra information needs
        # to be added to the traceback
        if excType is SyntaxError:
            try:
                msg, (filename, lineno, self.offset, line) = excValue
            except:
                pass
            else:
                if not filename:
                    filename = "<string>"
                line = line.strip()
                self.traceback.append( (filename, lineno, "", line) )
                excValue = msg
        try:
            self.exception_details = str(excValue)
        except:
            self.exception_details = "<unprintable %s object>" & type(excValue).__name__

        del exc_info
        
    def __str__(self):
        ret = "Type %s \n \
        Traceback: %s \n \
        Details  : %s" % ( str(self.exception_type), str(self.traceback), self.exception_details )
        return ret

#---------------------------------------------------------------------------

class DemoErrorPanel(wx.Panel):
    """Panel put into the demo tab when the demo fails to run due  to errors"""

    def __init__(self, parent, codePanel, demoError, log):
        wx.Panel.__init__(self, parent, -1)#, style=wx.NO_FULL_REPAINT_ON_RESIZE)
        self.codePanel = codePanel
        self.nb = parent
        self.log = log

        self.box = wx.BoxSizer(wx.VERTICAL)

        # Main Label
        self.box.Add(wx.StaticText(self, -1, "An error has occurred while trying to run the demo")
                     , 0, wx.ALIGN_CENTER | wx.TOP, 10)

        # Exception Information
        boxInfo      = wx.StaticBox(self, -1, "Exception Info" )
        boxInfoSizer = wx.StaticBoxSizer(boxInfo, wx.VERTICAL ) # Used to center the grid within the box
        boxInfoGrid  = wx.FlexGridSizer(0, 2, 0, 0)
        textFlags    = wx.ALIGN_RIGHT | wx.LEFT | wx.RIGHT | wx.TOP
        boxInfoGrid.Add(wx.StaticText(self, -1, "Type: "), 0, textFlags, 5 )
        boxInfoGrid.Add(wx.StaticText(self, -1, str(demoError.exception_type)) , 0, textFlags, 5 )
        boxInfoGrid.Add(wx.StaticText(self, -1, "Details: ") , 0, textFlags, 5 )
        boxInfoGrid.Add(wx.StaticText(self, -1, demoError.exception_details) , 0, textFlags, 5 )
        boxInfoSizer.Add(boxInfoGrid, 0, wx.ALIGN_CENTRE | wx.ALL, 5 )
        self.box.Add(boxInfoSizer, 0, wx.ALIGN_CENTER | wx.ALL, 5)
       
        # Set up the traceback list
        # This one automatically resizes last column to take up remaining space
        from tagerror import TestListCtrl
        self.list = TestListCtrl(self, -1, style=wx.LC_REPORT  | wx.SUNKEN_BORDER)
        self.list.Bind(wx.EVT_LEFT_DCLICK, self.OnDoubleClick)
        self.list.Bind(wx.EVT_LIST_ITEM_SELECTED, self.OnItemSelected)
        self.list.InsertColumn(0, "Filename")
        self.list.InsertColumn(1, "Line", wx.LIST_FORMAT_RIGHT)
        self.list.InsertColumn(2, "Function")
        self.list.InsertColumn(3, "Code")
        self.InsertTraceback(self.list, demoError.traceback)
        self.list.SetColumnWidth(0, wx.LIST_AUTOSIZE)
        self.list.SetColumnWidth(2, wx.LIST_AUTOSIZE)
        self.box.Add(wx.StaticText(self, -1, "Traceback:")
                     , 0, wx.ALIGN_CENTER | wx.TOP, 5)
        self.box.Add(self.list, 1, wx.GROW | wx.ALIGN_CENTER | wx.ALL, 5)
        self.box.Add(wx.StaticText(self, -1, "Entries from the demo module are shown in blue\n"
                                           + "Double-click on them to go to the offending line")
                     , 0, wx.ALIGN_CENTER | wx.BOTTOM, 5)

        self.box.Fit(self)
        self.SetSizer(self.box)


    def InsertTraceback(self, list, traceback):
        #Add the traceback data
        for x in range(len(traceback)):
            data = traceback[x]
            list.InsertStringItem(x, os.path.basename(data[0])) # Filename
            list.SetStringItem(x, 1, str(data[1]))              # Line
            list.SetStringItem(x, 2, str(data[2]))              # Function
            list.SetStringItem(x, 3, str(data[3]))              # Code
            
            # Check whether this entry is from the demo module
            if data[0] == "<original>" or data[0] == "<modified>": # FIXME: make more generalised
                self.list.SetItemData(x, int(data[1]))   # Store line number for easy access
                # Give it a blue colour
                item = self.list.GetItem(x)
                item.SetTextColour(wx.BLUE)
                self.list.SetItem(item)
            else:
                self.list.SetItemData(x, -1)        # Editor can't jump into this one's code
       

    def OnItemSelected(self, event):
        # This occurs before OnDoubleClick and can be used to set the
        # currentItem. OnDoubleClick doesn't get a wxListEvent....
        self.currentItem = event.m_itemIndex
        event.Skip()

        
    def OnDoubleClick(self, event):
        # If double-clicking on a demo's entry, jump to the line number
        line = self.list.GetItemData(self.currentItem)
        if line != -1:
            self.nb.SetSelection(1) # Switch to the code viewer tab
            wx.CallAfter(self.codePanel.JumpToLine, line-1, True)
        event.Skip()
        

#---------------------------------------------------------------------------

class DemoTaskBarIcon(wx.TaskBarIcon):
    TBMENU_RESTORE = wx.NewId()
    TBMENU_CLOSE   = wx.NewId()
    TBMENU_CHANGE  = wx.NewId()
    TBMENU_REMOVE  = wx.NewId()
    
    def __init__(self, frame):
        wx.TaskBarIcon.__init__(self)
        self.frame = frame

        # Set the image
        icon = self.MakeIcon(images.getWXPdemoImage())
        self.SetIcon(icon, "wxPython Demo")
        self.imgidx = 1
        
        # bind some events
        self.Bind(wx.EVT_TASKBAR_LEFT_DCLICK, self.OnTaskBarActivate)
        self.Bind(wx.EVT_MENU, self.OnTaskBarActivate, id=self.TBMENU_RESTORE)
        self.Bind(wx.EVT_MENU, self.OnTaskBarClose, id=self.TBMENU_CLOSE)
        self.Bind(wx.EVT_MENU, self.OnTaskBarChange, id=self.TBMENU_CHANGE)
        self.Bind(wx.EVT_MENU, self.OnTaskBarRemove, id=self.TBMENU_REMOVE)


    def CreatePopupMenu(self):
        """
        This method is called by the base class when it needs to popup
        the menu for the default EVT_RIGHT_DOWN event.  Just create
        the menu how you want it and return it from this function,
        the base class takes care of the rest.
        """
        menu = wx.Menu()
        menu.Append(self.TBMENU_RESTORE, "Restore wxPython Demo")
        menu.Append(self.TBMENU_CLOSE,   "Close wxPython Demo")
        menu.AppendSeparator()
        menu.Append(self.TBMENU_CHANGE, "Change the TB Icon")
        menu.Append(self.TBMENU_REMOVE, "Remove the TB Icon")
        return menu


    def MakeIcon(self, img):
        """
        The various platforms have different requirements for the
        icon size...
        """
        if "wxMSW" in wx.PlatformInfo:
            img = img.Scale(16, 16)
        elif "wxGTK" in wx.PlatformInfo:
            img = img.Scale(22, 22)
        # wxMac can be any size upto 128x128, so leave the source img alone....
        icon = wx.IconFromBitmap(img.ConvertToBitmap() )
        return icon
    

    def OnTaskBarActivate(self, evt):
        if self.frame.IsIconized():
            self.frame.Iconize(False)
        if not self.frame.IsShown():
            self.frame.Show(True)
        self.frame.Raise()


    def OnTaskBarClose(self, evt):
        self.frame.Close()


    def OnTaskBarChange(self, evt):
        names = [ "WXPdemo", "Mondrian", "Pencil", "Carrot" ]                  
        name = names[self.imgidx]
        
        getFunc = getattr(images, "get%sImage" % name)
        self.imgidx += 1
        if self.imgidx >= len(names):
            self.imgidx = 0
            
        icon = self.MakeIcon(getFunc())
        self.SetIcon(icon, "This is a new icon: " + name)


    def OnTaskBarRemove(self, evt):
        self.RemoveIcon()
 
#---------------------------------------------------------------------------
class wxPythonDemo(wx.Frame):
    overviewText = "all"

    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, -1, title, size = (970, 720),
                          style=wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE)

        self.SetMinSize((1000,720))                
        self.mgr = wx.aui.AuiManager()
        self.mgr.SetManagedWindow(self)

        if len(sys.argv) > 1:
            arg = sys.argv[1]
            self.projectxml = arg
        else:
            #print "Please input project xml file. Synopsis: tagsvntool.py  <*.xml> "
            self.projectxml = "project_mergepoint.xml"
            
        self.loaded = False
        self.cwd = os.getcwd()
        self.curOverview = ""
        self.demoPage = None
        self.codePage = None
        self.shell = None
        self.firstTime = True
        self.finddlg = None
  

        icon = tagimages.getWXPdemoIcon()
        self.SetIcon(icon)

        try:
            self.tbicon = DemoTaskBarIcon(self)
        except:
            self.tbicon = None
            
        self.otherWin = None
        self.Bind(wx.EVT_IDLE, self.OnIdle)
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)
        self.Bind(wx.EVT_ICONIZE, self.OnIconfiy)
        self.Bind(wx.EVT_MAXIMIZE, self.OnMaximize)

        self.Centre(wx.BOTH)
        self.CreateStatusBar(3, wx.ST_SIZEGRIP)
        

        self.dying = False
        self.skipLoad = False
        
        def EmptyHandler(evt): pass

        self.ReadConfigurationFile()
        
        # Create a Notebook
        self.nb = wx.Notebook(self, -1, style=wx.CLIP_CHILDREN)
        self.nb.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGED, self.OnPageChanged)
        imgList = wx.ImageList(16, 16)
        for png in ["overview", "code", "demo"]:
            bmp = tagimages.catalog[png].getBitmap()
            imgList.Add(bmp)
        self.nb.AssignImageList(imgList)

        self.BuildMenuBar()
        
        self.finddata = wx.FindReplaceData()
        self.finddata.SetFlags(wx.FR_DOWN)

        # Create a TreeCtrl
        leftPanel = wx.Panel(self, style=wx.TAB_TRAVERSAL|wx.CLIP_CHILDREN)

        # Set up a log window
        self.log = wx.TextCtrl(self, -1,style = wx.TE_MULTILINE|wx.HSCROLL)
        if wx.Platform == "__WXMAC__":
            self.log.MacCheckSpelling(False)
        # But instead of the above we want to show how to use our own wx.Log class
        wx.Log_SetActiveTarget(MyLog(self.log))

        self.svnTreeTagSelected = {}
        self.svnTreeMap = {}
        self.svnTreeModMap = {} 
         
        self.treeMap = {}
        self.searchItems = {}
       
 
        self.tree = wxPythonDemoTree(leftPanel)
    
        self.filter = wx.SearchCtrl(leftPanel, style=wx.TE_PROCESS_ENTER)
        #self.filter.Bind(wx.EVT_TEXT, self.RecreateSvnTree)
        self.filter.Bind(wx.EVT_TEXT_ENTER, self.OnSearch)
        
        self.RecreateSvnTree(True)        
        self.tree.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged)
         
        self.LoadDemo(self.projectname)
        self.loaded = True            
     
        # add the windows to the splitter and split it.
        leftBox = wx.BoxSizer(wx.VERTICAL)
        leftBox.Add(self.tree, 1, wx.EXPAND)
        leftBox.Add(wx.StaticText(leftPanel, label = "Search :"), 0, wx.TOP|wx.LEFT, 5)
        leftBox.Add(self.filter, 0, wx.EXPAND|wx.ALL, 5)
        leftPanel.SetSizer(leftBox)

        # select initial items
        self.nb.SetSelection(0)
        self.tree.SelectItem(self.root)

            
        # Use the aui manager to set up everything
        self.mgr.AddPane(self.nb, wx.aui.AuiPaneInfo().CenterPane().Name("Notebook"))
        self.mgr.AddPane(leftPanel,
                         wx.aui.AuiPaneInfo().
                         Left().Layer(2).BestSize((240, -1)).
                         MinSize((300, -1)).
                         Floatable(ALLOW_AUI_FLOATING).FloatingSize((240, 700)).                        
                         CloseButton(False).
                         Name("DemoTree"))
        self.mgr.AddPane(self.log,
                         wx.aui.AuiPaneInfo().
                         Bottom().BestSize((-1, 750)).
                         MinSize((-1,280)).
                         Floatable(ALLOW_AUI_FLOATING).FloatingSize((600, 260)).
                         Caption("Log Messages").
                         CloseButton(False).
                         Name("LogWindow"))
            
        self.auiConfigurations[DEFAULT_PERSPECTIVE] = self.mgr.SavePerspective()
        self.mgr.Update()

        self.mgr.SetFlags(self.mgr.GetFlags() ^ wx.aui.AUI_MGR_TRANSPARENT_DRAG)
        

    def errorMessage(self, msg):
	dlg = wx.MessageDialog(self, msg, 'Warning', wx.OK | wx.ICON_INFORMATION)
	dlg.ShowModal()
	dlg.Destroy()

    def ReadConfigurationFile(self):

        self.auiConfigurations = {}
        #self.expansionState = [0, 1]

        config = GetConfig()
        val = config.Read('ExpansionState')
        if val:
            #self.expansionState = eval(val)
            pass

        val = config.Read('AUIPerspectives')
        if val:
            self.auiConfigurations = eval(val)
        
    def OnLoadProject(self, event):
        wildcard = "svn project setting file (*.xml)|*.xml|"     \
           "All files (*.*)|*.*"
        dlg = wx.FileDialog(self, message="Load a project ...", defaultDir=os.getcwd(), 
            defaultFile="", wildcard=wildcard,style=wx.OPEN )

        #dlg.SetFilterIndex(2)

        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
        else:
            return 

        self.projectxml = path
        wx.BeginBusyCursor()
        #wx.BusyInfo("Refresh subversion tag, please wait ... ")
        self.RecreateSvnTree(True)
        self.codePage.editor.Clear()
        self.codePage.cb1.SetValue("")
        self.codePage.cb2.SetValue("")
        self.log.Clear()
        self.demoPage.cballtags.SetValue("")
        self.demoPage.cbbranches.SetValue("")
        self.demoPage.tagname.Clear()
        self.demoPage.tagcomment.Clear()
        
        wx.EndBusyCursor()

    def OnRefresh(self, event):
        wx.BeginBusyCursor()
        #wx.BusyInfo("Refresh subversion tag, please wait ... ")
        os.system("svn update  " + self.projectxml )    
        self.RecreateSvnTree(True)
        wx.EndBusyCursor()

    def OnPageChanged(self, event):
        self.codePage.editor.Clear()
        self.codePage.cb1.SetValue("")
        self.codePage.cb2.SetValue("")
        self.log.Clear()
        self.demoPage.cballtags.SetValue("")
        self.demoPage.cbbranches.SetValue("")
        self.demoPage.tagname.Clear()
        self.demoPage.tagcomment.Clear()

        event.Skip()
        
    def OnSvnSync(self, event):
        wx.BeginBusyCursor()
        svnsyncpath = tagutil.getSvnSyncPath(self.projectxml )
	if svnsyncpath!=None and svnsyncpath!='' :
		#busy = wx.BusyInfo("running...")
		syncresult = webtransfer.getWebContent(svnsyncpath,1024).strip()

		if syncresult == '::::>' :	
			#busy = wx.BusyInfo("another running...")
			self.errorMessage('another running...')
		elif syncresult.find( '===>|')>-1 :
			self.RecreateSvnTree(True)
		else :
			#wx.BusyInfo("malfunction occur")
			self.errorMessage('malfunction occur!')
			
        wx.EndBusyCursor()


    def BuildMenuBar(self):

        # Make a File menu
        self.mainmenu = wx.MenuBar()
        menu = wx.Menu()
        #item = menu.Append(-1, '&Redirect Output', 'Redirect print statements to a window', wx.ITEM_CHECK)
        #self.Bind(wx.EVT_MENU, self.OnToggleRedirect, item)
 
        item = menu.Append(-1, '&Load\tCtrl-L', 'Load a Project', wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU, self.OnLoadProject, item)

        item = menu.Append(-1, '&Refresh\tCtrl-R', 'Refresh all tag', wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU, self.OnRefresh, item)
        
        item = menu.Append(-1, '&SVN Sync\tCtrl-S', 'Svn Sync', wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU, self.OnSvnSync, item)
         
        exitItem = wx.MenuItem(menu, -1, 'E&xit\tCtrl-Q', 'we will exist!')
        exitItem.SetBitmap(tagimages.catalog['exit'].getBitmap())
        menu.AppendItem(exitItem)
        self.Bind(wx.EVT_MENU, self.OnFileExit, exitItem)
        wx.App.SetMacExitMenuItemId(exitItem.GetId())
        self.mainmenu.Append(menu, '&Project')

        # Make a Demo menu
        #menu = wx.Menu()
        #for indx, item in enumerate(_treeList[:-1]):
           # menuItem = wx.MenuItem(menu, -1, item[0])
            #submenu = wx.Menu()
            #for childItem in item[1]:
            #    mi = submenu.Append(-1, childItem)
            #    self.Bind(wx.EVT_MENU, self.OnDemoMenu, mi)
        #    menuItem.SetBitmap(tagimages.catalog[_demoPngs[indx+1]].getBitmap())
           # menuItem.SetBitmap(tagimages.catalog[_demoPngs[10]].getBitmap())
           # menuItem.SetSubMenu(submenu)
           # menu.AppendItem(menuItem)
        #self.mainmenu.Append(menu, '&Modules')

        # Make an Option menu
        # If we've turned off floatable panels then this menu is not needed
        if ALLOW_AUI_FLOATING:
            menu = wx.Menu()
            auiPerspectives = self.auiConfigurations.keys()
            auiPerspectives.sort()
            perspectivesMenu = wx.Menu()
            item = wx.MenuItem(perspectivesMenu, -1, DEFAULT_PERSPECTIVE, "Load startup default perspective", wx.ITEM_RADIO)
            self.Bind(wx.EVT_MENU, self.OnAUIPerspectives, item)
            perspectivesMenu.AppendItem(item)
            for indx, key in enumerate(auiPerspectives):
                if key == DEFAULT_PERSPECTIVE:
                    continue
                item = wx.MenuItem(perspectivesMenu, -1, key, "Load user perspective %d"%indx, wx.ITEM_RADIO)
                perspectivesMenu.AppendItem(item)
                self.Bind(wx.EVT_MENU, self.OnAUIPerspectives, item)

            menu.AppendMenu(wx.ID_ANY, "&AUI Perspectives", perspectivesMenu)
            self.perspectives_menu = perspectivesMenu

            item = wx.MenuItem(menu, -1, 'Save Perspective', 'Save AUI perspective')
            #item.SetBitmap(images.catalog['saveperspective'].getBitmap())
            menu.AppendItem(item)
            self.Bind(wx.EVT_MENU, self.OnSavePerspective, item)

            item = wx.MenuItem(menu, -1, 'Delete Perspective', 'Delete AUI perspective')
            #item.SetBitmap(images.catalog['deleteperspective'].getBitmap())
            menu.AppendItem(item)
            self.Bind(wx.EVT_MENU, self.OnDeletePerspective, item)

            menu.AppendSeparator()

            item = wx.MenuItem(menu, -1, 'Restore Tree Expansion', 'Restore the initial tree expansion state')
            #item.SetBitmap(images.catalog['expansion'].getBitmap())
            menu.AppendItem(item)
            self.Bind(wx.EVT_MENU, self.OnTreeExpansion, item)

            self.mainmenu.Append(menu, '&Options')
        

        self.SetMenuBar(self.mainmenu)

        if False:
            # This is another way to set Accelerators, in addition to
            # using the '\t<key>' syntax in the menu items.
            aTable = wx.AcceleratorTable([(wx.ACCEL_ALT,  ord('X'), exitItem.GetId()),
                                          (wx.ACCEL_CTRL, ord('H'), helpItem.GetId()),
                                          (wx.ACCEL_CTRL, ord('F'), findItem.GetId()),
                                          (wx.ACCEL_NORMAL, wx.WXK_F3, findnextItem.GetId()),
                                          (wx.ACCEL_NORMAL, wx.WXK_F9, shellItem.GetId()),
                                          ])
            self.SetAcceleratorTable(aTable)
            

    def error(self, msg):
       dlg = wx.MessageDialog(self, msg, 'Warning', wx.OK | wx.ICON_INFORMATION)
       dlg.ShowModal()
       dlg.Destroy()
       
#---------------------------------------------    
    def RecreateSvnTree(self, bRefresh=False, evt=None):     
       
        item = self.tree.GetSelection()
        if  item:
            current = self.tree.GetItemText(item)
        else:
            current = self.projectxml
        selectItem = None
        
        #expansionState = self.tree.GetExpansionState()
        self.tree.DeleteAllItems()
        self.tree.Freeze()

        
        filter = self.filter.GetValue()               
        treeFont = self.tree.GetFont()
        catFont = self.tree.GetFont()
        
        if 'wxMSW' not in wx.PlatformInfo or wx.GetApp().GetComCtl32Version() >= 600:
            treeFont.SetPointSize(treeFont.GetPointSize()+2)
            treeFont.SetWeight(wx.BOLD)
            catFont.SetWeight(wx.BOLD)
        
        if bRefresh:
            self.svnTreeTagSelected.clear()
            self.svnTreeMap.clear()
            self.svnTreeModMap.clear()
            
            self.paths = tagutil.getsvnpaths(self.projectxml)         
            if  not self.paths:
                self.error("Project xml file format error! Please check it.")  
                self.tree.Thaw()
                self.Close()
                                                        
            (self.server, self.projectname, self.remote) = tagutil.getProjectInfo(self.projectxml )            
            (self.projectTags, self.projectBranches) =  tagutil.getProjectTags( self.projectxml )        
            tagmap = {}
            tagmap["tags"] = self.projectTags
            tagmap["branches" ] = self.projectBranches            
            tagmap["all_sort_by_time"] =  self.projectTags  + self.projectBranches
            self.svnTreeMap[ self.projectname ] =  tagmap
            self.svnTreeModMap[ self.projectname ] = self.projectname
            if self.demoPage: #reload
                self.demoPage.treeModChanged(self, self.projectname, tagmap["tags"], tagmap["branches"]  )
            if self.codePage:
                self.codePage.treeModChange(self,  self.projectname, tagmap["all_sort_by_time"], tagmap["tags"] )

            modmax = len(self.paths)
            dlg = wx.ProgressDialog("gsp tag tool",
                               "Loading svn tags",
                               maximum = modmax,
                               parent=self,
                               style = #wx.PD_CAN_ABORT
                                  wx.PD_APP_MODAL
                                #| wx.PD_ELAPSED_TIME
                                #| wx.PD_ESTIMATED_TIME
                                | wx.PD_REMAINING_TIME
                                )
            curmod = 1
        
        self.SetStatusText("Module Name: %s " % self.projectname,  0 )
        self.SetStatusText("Remote Server: %s " % self.remote,  1 )
        self.SetStatusText("Local Server: %s" % self.server,  2 )
        
        self.root = self.tree.AddRoot(self.projectname )      
        self.tree.SetItemImage(self.root, 13)     
        self.tree.SetItemFont(self.root, treeFont)     
        rootPydata = {}    
        rootPydata['type'] = 'root'
        self.tree.SetItemPyData(self.root, rootPydata)                    
       
        treemap = {}
        # sample:  path=/com/org.openembedded.dev  pathlist = [com , org.openembedded.dev ] 
        for path, pathlist in self.paths:             
            if filter:                 
                pathlist = [item for item in pathlist if filter.lower() in item.lower()]                  
            count = len(pathlist)            
            if   count != 0:
                level =1                  
                for mod in pathlist:                
                    key = str( pathlist[:level] )
                    if  not treemap.has_key(key):                     
                         if level ==1:
                             parent = self.root
                         else:     
                            parent= treemap[ str( pathlist[:level-1] ) ]
                         
                         # leaf node => real module    
                         nodePydata = {}
                         if level == count:
                             if  not self.svnTreeMap.has_key(mod) or bRefresh:                                 
                                 (cbList, tags, branches)  = tagutil.getTagAndBranch(self.server + path)                                 
                                 tagmap = {}
                                 tagmap["tags"] = tags                             
                                 tagmap["branches" ] = branches
                                 tagmap["all_sort_by_time"] = cbList
                                 self.svnTreeMap[mod] =  tagmap
                                 self.svnTreeModMap[ mod ] = path

                                 if curmod < modmax:
                                     dlg.Update(curmod)
                                 curmod += 1
                                  
                                 
                                 
                             else:
                                 tagmap = self.svnTreeMap[mod]
                                 cbList = tagmap["all_sort_by_time"]
                                                               
                             mystyle = wx.ALIGN_MASK | wx.CB_READONLY | wx.CB_DROPDOWN
                             cb =  wx.ComboBox(self.tree, choices = cbList, value="trunk", style=mystyle, name = mod )
                             self.svnTreeTagSelected[ mod ] = ("tags", "trunk")
                             self.Bind(wx.EVT_COMBOBOX, self.EvtComboBox, cb ) 
                             node = self.tree.AppendItem(parent , mod, image=9,  wnd= cb  )
                             nodePydata['type'] = 'list'
                             self.tree.SetItemPyData(node, nodePydata)
                             if   current == mod:
                                 selectItem = node
                              
                         else:
                             node = self.tree.AppendItem(parent , mod, image=9)       
                             nodePydata['type'] = 'group'
                             self.tree.SetItemPyData(node, nodePydata)                  
                         treemap[ key ] = node                    
                    level  += 1

        if bRefresh:
            dlg.Destroy()   
        self.tree.ExpandAll()        
        self.tree.Thaw()
        


    def EvtComboBox(self, evt):
        cb = evt.GetEventObject()
        name =  cb.GetName()
        tag     = cb.GetValue()
        tagorBranch = "tags"
        tagmap = self.svnTreeMap[ name]
        if  tag in tagmap["branches"]:
            tagorBranch = "branches"
        self.svnTreeTagSelected[ name ] = (tagorBranch, tag)
       

    def OnSearchMenu(self, event):

        # Catch the search type (name or content)
        searchMenu = self.filter.GetMenu().GetMenuItems()
        fullSearch = searchMenu[1].IsChecked()
        
        if fullSearch:
            self.OnSearch()
        else:
            self.RecreateTree()
            

    def OnSearch(self, event=None):

        value = self.filter.GetValue()
        if not value:
            self.RecreateSvnTree()
            return

        wx.BeginBusyCursor()  
        self.RecreateSvnTree()            
        wx.EndBusyCursor()


    def SetTreeModified(self, modified):
        item = self.tree.GetSelection()
        if modified:
            image = len(_demoPngs)
        else:
            image = self.tree.GetItemPyData(item)
        self.tree.SetItemImage(item, image)
        
        
    def WriteText(self, text):
        if text[-1:] == '\n':
            text = text[:-1]
        wx.LogMessage(text)

    def write(self, txt):
        self.WriteText(txt)

#---------------------------------------------
    def OnItemExpanded(self, event):
        item = event.GetItem()
#        wx.LogMessage("OnItemExpanded: %s" % self.tree.GetItemText(item))
        event.Skip()

    #---------------------------------------------
    def OnItemCollapsed(self, event):
        item = event.GetItem()
#        wx.LogMessage("OnItemCollapsed: %s" % self.tree.GetItemText(item))
        event.Skip()

    #---------------------------------------------
    def OnTreeLeftDown(self, event):
        # reset the overview text if the tree item is clicked on again
        #pt = event.GetPosition();
        #item, flags = self.tree.Hit(pt)
        #if item == self.tree.GetSelection():
        #    self.SetOverview(self.tree.GetItemText(item)+" Overview", self.curOverview)
        event.Skip()

    #---------------------------------------------
    def OnSelChanged(self, event):
        if self.dying or not self.loaded or self.skipLoad:
            return
        item = event.GetItem()
        treePydata = self.tree.GetPyData(item)
        modName = self.tree.GetItemText(item)
        if treePydata['type']=='list' or treePydata['type']=='root':
            tagmap = self.svnTreeMap[ modName ]            
            self.demoPage.treeModChanged(self,  self.svnTreeModMap[modName], tagmap["tags"], tagmap["branches"]  )
            self.codePage.treeModChange(self,  self.svnTreeModMap[modName], tagmap["all_sort_by_time"], tagmap["tags"] )
            self.SetStatusText("Module Name: %s" % self.svnTreeModMap[ modName ], 0 )
            self.log.Clear()
            
        return         
                                        
        
    #---------------------------------------------
    def LoadDemo(self, modName):
        try:
            wx.BeginBusyCursor()
            self.Freeze()
            
            os.chdir( self.cwd )
            self.ShutdownDemoModule()
            
            self.mod = modName
            demoName = 'tagsvn'    
            
            if os.path.exists( GetOriginalFilename( demoName ) ):
#                wx.LogMessage( "Loading demo %s.py..." % demoName )
                self.demoModules = DemoModules( demoName   )
                self.LoadDemoSource(modName)
            else:                
                self.SetOverview( "wxPython", mainOverview )
                self.codePage = None
                self.UpdateNotebook( 1 )

        finally:
            wx.EndBusyCursor()
            self.Thaw()

    #---------------------------------------------
    def LoadDemoSource(self, modName):
        self.codePage = None
        self.codePage = DemoCodePanel(self.nb, self, modName)
        self.codePage.LoadDemo(self.demoModules)
  
        
    #---------------------------------------------
    def RunModule(self):
        """Runs the active module"""

        module = self.demoModules.GetActive()
        self.ShutdownDemoModule()
        overviewText = ""
        
        # o The RunTest() for all samples must now return a window that can
        #   be palced in a tab in the main notebook.
        # o If an error occurs (or has occurred before) an error tab is created.
        
        if module is not None:
            try:
                self.demoPage = module.runTest(self, self.nb)
            except:
                self.demoPage = DemoErrorPanel(self.nb, self.codePage,
                                               DemoError(sys.exc_info()), self)

            bg = self.nb.GetThemeBackgroundColour()
            if bg:
                self.demoPage.SetBackgroundColour(bg)

            assert self.demoPage is not None, "runTest must return a window!"
            
        else:
            # There was a previous error in compiling or exec-ing
            self.demoPage = DemoErrorPanel(self.nb, self.codePage,
                                           self.demoModules.GetErrorInfo(), self)
            
        self.SetOverview("Overview", self.curOverview)

        if self.firstTime:
            # change to the demo page the first time a module is run
            self.UpdateNotebook(1)
            self.firstTime = False
        else:
            # otherwise just stay on the same tab in case the user has changed to another one
            self.UpdateNotebook()
            pass

    #---------------------------------------------
    def ShutdownDemoModule(self):
        if self.demoPage:
            # inform the window that it's time to quit if it cares
            if hasattr(self.demoPage, "ShutdownDemo"):
                self.demoPage.ShutdownDemo()
            wx.YieldIfNeeded() # in case the page has pending events
            self.demoPage = None
            
    #---------------------------------------------
    def UpdateNotebook(self, select = -1):
        nb = self.nb
        debug = False
        self.Freeze()
        
        def UpdatePage(page, pageText):
            pageExists = False
            pagePos = -1
            for i in range(nb.GetPageCount()):
                if nb.GetPageText(i) == pageText:
                    pageExists = True
                    pagePos = i
                    break
                
            if page:
                if not pageExists:
                    # Add a new page
                    nb.AddPage(page, pageText, imageId=nb.GetPageCount())
                    if debug: wx.LogMessage("DBG: ADDED %s" % pageText)
                else:
                    if nb.GetPage(pagePos) != page:
                        # Reload an existing page
                        nb.DeletePage(pagePos)
                        nb.InsertPage(pagePos, page, pageText, imageId=pagePos)
                        if debug: wx.LogMessage("DBG: RELOADED %s" % pageText)
                    else:
                        # Excellent! No redraw/flicker
                        if debug: wx.LogMessage("DBG: SAVED from reloading %s" % pageText)
            elif pageExists:
                # Delete a page
                nb.DeletePage(pagePos)
                if debug: wx.LogMessage("DBG: DELETED %s" % pageText)
            else:
                if debug: wx.LogMessage("DBG: STILL GONE - %s" % pageText)
            
                
        if select == -1:
            select = nb.GetSelection()

        UpdatePage(self.codePage, "Diff")
        UpdatePage(self.demoPage, "Tag ")

        if select >= 0 and select < nb.GetPageCount():
            nb.SetSelection(select)

        self.Thaw()
        
    #---------------------------------------------
    def SetOverview(self, name, text):
        pass
        #self.curOverview = text
        #lead = text[:6]
        #if lead != '<html>' and lead != '<HTML>':
         #   text = '<br>'.join(text.split('\n'))
        #if wx.USE_UNICODE:
        #   text = text.decode('iso8859_1')  
        #self.ovr.SetPage(text)
        #self.nb.SetPageText(0, name)

    #---------------------------------------------
    # Menu methods
    def OnFileExit(self, *event):
        self.Close()

    def OnToggleRedirect(self, event):
        app = wx.GetApp()
        if event.Checked():
            app.RedirectStdio()
            print "Print statements and other standard output will now be directed to this window."
        else:
            app.RestoreStdio()
            print "Print statements and other standard output will now be sent to the usual location."


    def OnAUIPerspectives(self, event):
        perspective = self.perspectives_menu.GetLabel(event.GetId())
        self.mgr.LoadPerspective(self.auiConfigurations[perspective])
        self.mgr.Update()


    def OnSavePerspective(self, event):
        dlg = wx.TextEntryDialog(self, "Enter a name for the new perspective:", "AUI Configuration")
        
        dlg.SetValue(("Perspective %d")%(len(self.auiConfigurations)+1))
        if dlg.ShowModal() != wx.ID_OK:
            return

        perspectiveName = dlg.GetValue()
        menuItems = self.perspectives_menu.GetMenuItems()
        for item in menuItems:
            if item.GetLabel() == perspectiveName:
                wx.MessageBox("The selected perspective name:\n\n%s\n\nAlready exists."%perspectiveName,
                              "Error", style=wx.ICON_ERROR)
                return
                
        item = wx.MenuItem(self.perspectives_menu, -1, dlg.GetValue(),
                           "Load user perspective %d"%(len(self.auiConfigurations)+1),
                           wx.ITEM_RADIO)
        self.Bind(wx.EVT_MENU, self.OnAUIPerspectives, item)                
        self.perspectives_menu.AppendItem(item)
        item.Check(True)
        self.auiConfigurations.update({dlg.GetValue(): self.mgr.SavePerspective()})


    def OnDeletePerspective(self, event):
        menuItems = self.perspectives_menu.GetMenuItems()[1:]
        lst = []
        loadDefault = False
        
        for item in menuItems:
            lst.append(item.GetLabel())
            
        dlg = wx.MultiChoiceDialog(self, 
                                   "Please select the perspectives\nyou would like to delete:",
                                   "Delete AUI Perspectives", lst)

        if dlg.ShowModal() == wx.ID_OK:
            selections = dlg.GetSelections()
            strings = [lst[x] for x in selections]
            for sel in strings:
                self.auiConfigurations.pop(sel)
                item = menuItems[lst.index(sel)]
                if item.IsChecked():
                    loadDefault = True
                    self.perspectives_menu.GetMenuItems()[0].Check(True)
                self.perspectives_menu.DeleteItem(item)
                lst.remove(sel)

        if loadDefault:
            self.mgr.LoadPerspective(self.auiConfigurations[DEFAULT_PERSPECTIVE])
            self.mgr.Update()


    def OnTreeExpansion(self, event):
        #self.tree.SetExpansionState(self.expansionState)
        pass
        
 
    def OnHelpAbout(self, event):
        from About import MyAboutBox
        about = MyAboutBox(self)
        about.ShowModal()
        about.Destroy()

    def OnHelpFind(self, event):
        if self.finddlg != None:
            return
        
        self.nb.SetSelection(1)
        self.finddlg = wx.FindReplaceDialog(self, self.finddata, "Find",
                        wx.FR_NOMATCHCASE | wx.FR_NOWHOLEWORD)
        self.finddlg.Show(True)


    def OnUpdateFindItems(self, evt):
        evt.Enable(self.finddlg == None)


    def OnFind(self, event):
        editor = self.codePage.editor
        self.nb.SetSelection(1)
        end = editor.GetLastPosition()
        textstring = editor.GetRange(0, end).lower()
        findstring = self.finddata.GetFindString().lower()
        backward = not (self.finddata.GetFlags() & wx.FR_DOWN)
        if backward:
            start = editor.GetSelection()[0]
            loc = textstring.rfind(findstring, 0, start)
        else:
            start = editor.GetSelection()[1]
            loc = textstring.find(findstring, start)
        if loc == -1 and start != 0:
            # string not found, start at beginning
            if backward:
                start = end
                loc = textstring.rfind(findstring, 0, start)
            else:
                start = 0
                loc = textstring.find(findstring, start)
        if loc == -1:
            dlg = wx.MessageDialog(self, 'Find String Not Found',
                          'Find String Not Found in Demo File',
                          wx.OK | wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()
        if self.finddlg:
            if loc == -1:
                self.finddlg.SetFocus()
                return
            else:
                self.finddlg.Destroy()
                self.finddlg = None
        editor.ShowPosition(loc)
        editor.SetSelection(loc, loc + len(findstring))



    def OnFindNext(self, event):
        if self.finddata.GetFindString():
            self.OnFind(event)
        else:
            self.OnHelpFind(event)

    def OnFindClose(self, event):
        event.GetDialog().Destroy()
        self.finddlg = None


    def OnOpenShellWindow(self, evt):
        if self.shell:
            # if it already exists then just make sure it's visible
            s = self.shell
            if s.IsIconized():
                s.Iconize(False)
            s.Raise()
        else:
            # Make a PyShell window
            from wx import py
            namespace = { 'wx'    : wx,
                          'app'   : wx.GetApp(),
                          'frame' : self,
                          }
            self.shell = py.shell.ShellFrame(None, locals=namespace)
            self.shell.SetSize((640,480))
            self.shell.Show()

            # Hook the close event of the main frame window so that we
            # close the shell at the same time if it still exists            
            def CloseShell(evt):
                if self.shell:
                    self.shell.Close()
                evt.Skip()
            self.Bind(wx.EVT_CLOSE, CloseShell)


    def OnOpenWidgetInspector(self, evt):
        # Activate the widget inspection tool
        from wx.lib.inspection import InspectionTool
        if InspectionTool().initialized:
            InspectionTool().Init()

        # Find a widget to be selected in the tree.  Use either the
        # one under the cursor, if any, or this frame.
        wnd = wx.FindWindowAtPointer()
        if not wnd:
            wnd = self
        InspectionTool().Show(wnd, True)

        
    #---------------------------------------------
    def OnCloseWindow(self, event):
        self.dying = True
        self.demoPage = None
        self.codePage = None
        self.mainmenu = None
        if self.tbicon is not None:
            self.tbicon.Destroy()

        config = GetConfig()
        #config.Write('ExpansionState', str(self.tree.GetExpansionState()))
        config.Write('AUIPerspectives', str(self.auiConfigurations))
        config.Flush()

        self.Destroy()


    #---------------------------------------------
    def OnIdle(self, event):
        if self.otherWin:
            self.otherWin.Raise()
            self.demoPage = self.otherWin
            self.otherWin = None


    #---------------------------------------------
    def ShowTip(self):
        return 
        config = GetConfig()
        showTipText = config.Read("tips")
        if showTipText:
            showTip, index = eval(showTipText)
        else:
            showTip, index = (1, 0)
            
        if showTip:
            tp = wx.CreateFileTipProvider(opj("data/tips.txt"), index)
            ##tp = MyTP(0)
            showTip = wx.ShowTip(self, tp)
            index = tp.GetCurrentTip()
            config.Write("tips", str( (showTip, index) ))
            config.Flush()

    #---------------------------------------------
    def OnDemoMenu(self, event):
        try:
            selectedDemo = self.treeMap[self.mainmenu.GetLabel(event.GetId())]
        except:
            selectedDemo = None
        if selectedDemo:
            self.tree.SelectItem(selectedDemo)
            self.tree.EnsureVisible(selectedDemo)



    #---------------------------------------------
    def OnIconfiy(self, evt):
        #wx.LogMessage("OnIconfiy: %s" % evt.Iconized())
        evt.Skip()

    #---------------------------------------------
    def OnMaximize(self, evt):
        #wx.LogMessage("OnMaximize")
        evt.Skip()

    #---------------------------------------------
    def OnActivate(self, evt):
        #wx.LogMessage("OnActivate: %s" % evt.GetActive())
        evt.Skip()

    #---------------------------------------------
    def OnAppActivate(self, evt):
        #wx.LogMessage("OnAppActivate: %s" % evt.GetActive())
        evt.Skip()

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

class MySplashScreen(wx.SplashScreen):
    def __init__(self):
        bmp = wx.Image(opj("taglogo.gif")).ConvertToBitmap()
        wx.SplashScreen.__init__(self, bmp,
                                 wx.SPLASH_CENTRE_ON_SCREEN | wx.SPLASH_TIMEOUT,
                                 1000, None, -1)
        self.Bind(wx.EVT_CLOSE, self.OnClose)
        self.fc = wx.FutureCall(2000, self.ShowMain)


    def OnClose(self, evt):
        # Make sure the default handler runs too so this window gets
        # destroyed
        evt.Skip()
        self.Hide()
        
        # if the timer is still running then go ahead and show the
        # main frame now
        if self.fc.IsRunning():
            self.fc.Stop()
            self.ShowMain()


    def ShowMain(self):
        frame = wxPythonDemo(None, "Svn Tag Tool")
        frame.Show()
        if self.fc.IsRunning():
            self.Raise()
        wx.CallAfter(frame.ShowTip)




#---------------------------------------------------------------------------

if USE_CUSTOMTREECTRL:
    import wx.lib.customtreectrl as CT
    TreeBaseClass = CT.CustomTreeCtrl
else:
    TreeBaseClass = wx.TreeCtrl
    

class wxPythonDemoTree(TreeBaseClass):
    def __init__(self, parent):
        TreeBaseClass.__init__(self, parent, style=wx.TR_DEFAULT_STYLE|
                               wx.TR_HAS_VARIABLE_ROW_HEIGHT)
        self.BuildTreeImageList()
        #if USE_CUSTOMTREECTRL:
           # self.SetSpacing(10)
            #self.SetWindowStyle(self.GetWindowStyle() & ~wx.TR_LINES_AT_ROOT)

    def AppendItem(self, parent, text, image=-1, wnd=None):
        if USE_CUSTOMTREECTRL:
            item = TreeBaseClass.AppendItem(self, parent, text, image=image, wnd=wnd)
        else:
            item = TreeBaseClass.AppendItem(self, parent, text, image=image)
        return item
            
    def BuildTreeImageList(self):
        imgList = wx.ImageList(16, 16)
        for png in _demoPngs:
            imgList.Add(tagimages.catalog[png].getBitmap())
            
        # add the image for modified demos.
        imgList.Add(tagimages.catalog["custom"].getBitmap())

        self.AssignImageList(imgList)
        

    def GetItemIdentity(self, item):
        return self.GetPyData(item)


#---------------------------------------------------------------------------

class MyApp(wx.App):
    def OnInit(self):
        """
        Create and show the splash screen.  It will then create and show
        the main frame when it is time to do so.
        """

        wx.SystemOptions.SetOptionInt("mac.window-plain-transition", 1)
        self.SetAppName("GSP-tag")
        
        #splash = MySplashScreen()
        #splash.Show()
        frame = wxPythonDemo(None, "GSP Tag Tool")
        frame.Show()

        return True



#---------------------------------------------------------------------------

def main():
    try:
        demoPath = os.path.dirname(__file__)
        os.chdir(demoPath)
    except:
        pass
    app = MyApp(False)
    app.MainLoop()

#---------------------------------------------------------------------------


mainOverview = """<html><body>
<h2>Project Overview </h2>

 

"""


#----------------------------------------------------------------------------
#----------------------------------------------------------------------------

if __name__ == '__main__':
    __name__ = 'Main'
    main()

#----------------------------------------------------------------------------





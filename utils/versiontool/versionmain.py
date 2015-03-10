#!/usr/bin/env python
#----------------------------------------------------------------------------
# Name:         mpversiontool.py
# Purpose:      svn version tool 
#
# Author: Tercel Yi
#
# Copyright:    (c) 2008 Avocent
#----------------------------------------------------------------------------

import sys, os, time, traceback, types
import wx                  
import wx.aui
import wx.html

from lib.config import *
import lib.tagimages as tagimages
import lib.projectdata as projectdata
import lib.datacommon as datacommon
import lib.webtransfer as webtransfer
import versiondiffpanel
import errorpanel


# For debugging
##wx.Trap();
##print "wx.VERSION_STRING = %s (%s)" % (wx.VERSION_STRING, wx.USE_UNICODE and 'unicode' or 'ansi')
##print "pid:", os.getpid()
##raw_input("Press Enter...")


#---------------------------------------------------------------------------
# Constants for module versions
SVN_MODE =  True
USE_CUSTOMTREECTRL = True
ALLOW_AUI_FLOATING = False
DEFAULT_PERSPECTIVE = "Default Perspective"
cfg_default_file_path = DEFAULT_PROJECT


modOriginal = 0
modModified = 1
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
        name =name + ".py"
    return os.path.join(GetModifiedDirectory(), name)


def GetOriginalFilename(name):
    """
    Returns the filename of the original version of the specified demo
    """
    if not name.endswith(".py"):
        name =name + ".py"
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
            self.projectxml = cfg_default_file_path
            
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
        self.filter.ShowCancelButton(True)
        self.filter.Bind(wx.EVT_TEXT, self.RecreateTree)
        self.filter.Bind(wx.EVT_SEARCHCTRL_CANCEL_BTN,
                         lambda e: self.filter.SetValue(''))
        self.filter.Bind(wx.EVT_TEXT_ENTER, self.OnSearch)
        
        self.RecreateTree(True)        
        self.tree.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged)
 
        self.LoadDemo()        
     
        # add the windows to the splitter and split it.
        leftBox = wx.BoxSizer(wx.VERTICAL)
        leftBox.Add(self.tree, 1, wx.EXPAND)
        leftBox.Add(wx.StaticText(leftPanel, label = "Search :"), 0, wx.TOP|wx.LEFT, 5)
        leftBox.Add(self.filter, 0, wx.EXPAND|wx.ALL, 5)
        leftPanel.SetSizer(leftBox)

        # select initial items
        #self.nb.SetSelection(0)
 
        # Use the aui manager to set up everything
        self.mgr.AddPane(self.nb, wx.aui.AuiPaneInfo().CenterPane().Name("Notebook"))
        self.mgr.AddPane(leftPanel,
                         wx.aui.AuiPaneInfo().
                         Left().Layer(2).BestSize((240, -1)).
                         MinSize((200, -1)).
                         Floatable(ALLOW_AUI_FLOATING).FloatingSize((240, 700)).                        
                         CloseButton(False).
                         Name("DemoTree"))
        self.mgr.AddPane(self.log,
                         wx.aui.AuiPaneInfo().
                         Bottom().BestSize((-1, 750)).
                         MinSize((-1,50)).
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
        self.RecreateTree(True)
        
        self.tree.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged)
        self.LoadDemo()
        
        #self.codePage.editor.Clear()
        #self.codePage.cb1.SetValue("")
        #self.codePage.cb2.SetValue("")
        #self.log.Clear()
        #self.demoPage.cbversion_list.SetValue("")
        
        wx.EndBusyCursor()

    def OnRefresh(self, event):
        wx.BeginBusyCursor()
        #wx.BusyInfo("Refresh subversion tag, please wait ... ")
        datacommon.doSvnUpdate()
        self.RecreateTree(True)
        wx.EndBusyCursor()

    def OnPageChanged(self, event):
        #self.codePage.editor.Clear()
        self.log.Clear()
        #self.demoPage.cballtags.SetValue("")
        #self.demoPage.cbbranches.SetValue("")
        #self.demoPage.tagname.Clear()
        #self.demoPage.comment.SetValue('')

        event.Skip()
        
    def OnSvnSync(self, event):
        wx.BeginBusyCursor()
        svnsyncpath = self.objProjectData.svnsync
        if svnsyncpath!=None and svnsyncpath!='' :
            #busy = wx.BusyInfo("running...")
            syncresult = webtransfer.getWebContent(svnsyncpath,1024).strip()

            if syncresult == '::::>' :	
                #busy = wx.BusyInfo("another running...")
                self.errorMessage('another running...')
            elif syncresult.find( '===>|')>-1 :
                datacommon.doSvnUpdate()
                self.RecreateTree(True)
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
    def RecreateTree(self, bRefresh=True, evt=None):     
 
        self.current = None
        item = self.tree.GetSelection()
        if item:
            self.current = self.treePydata['treenav']
        
        self.treePydata = {'type':'','name':'','manager':'','dir':'','treenav':''}
        
        #expansionState = self.tree.GetExpansionState()
        self.tree.DeleteAllItems()
        self.tree.Freeze()
               
        treeFont = self.tree.GetFont()
        catFont = self.tree.GetFont()
        
        if 'wxMSW' not in wx.PlatformInfo or wx.GetApp().GetComCtl32Version() >= 600:
            treeFont.SetPointSize(treeFont.GetPointSize()+2)
            treeFont.SetWeight(wx.BOLD)
            catFont.SetWeight(wx.BOLD)
        
        
        self.svnTreeTagSelected.clear()
        self.svnTreeMap.clear()
        self.svnTreeModMap.clear()
        
        if bRefresh:                                
            self.objProjectData = projectdata.ProjectData(self.projectxml)
            
        if self.objProjectData.isok :
            self.loaded = True
        else :
            self.error("Please open valid project xml file!")
            
        self.SetStatusText(" Name: %s " % self.objProjectData.name,  0 )
        self.SetStatusText("Remote Server: %s " % self.objProjectData.remote,  1 )
        self.SetStatusText("Local Server: %s" % self.objProjectData.local,  2 )
        
        self.root = self.tree.AddRoot(self.objProjectData.name )      
        self.tree.SetItemImage(self.root, 13)
        self.tree.SetItemFont(self.root, treeFont)
        rootPydata = {}    
        rootPydata['type'] = 'root'
        rootPydata['name'] = self.objProjectData.name
        rootPydata['manager'] = self.objProjectData.manager
        rootPydata['dir'] = self.projectxml
        rootPydata['treenav'] = self.objProjectData.name
        
        self.tree.SetItemPyData(self.root, rootPydata) 
        self.treePydata = rootPydata 
        
        self.createTreeNodeByGroup()                
        
        self.tree.Thaw()

        self.searchItems = {}
        

    def createTreeNodeByGroup(self) :
        selectItem = None
        firstChild = None

        filter = self.filter.GetValue() 
        treemap = {}

        #sample:  path=/com/org.openembedded.dev  pathlist = ['com' , 'org.openembedded.dev']
        
        for package_dict in self.objProjectData.package_list:      
            pathlist =  package_dict['dir'].split("/")
            
            if filter:                 
                pathlist = [item for item in pathlist if filter.lower() in item.lower()]      
                            
            count = len(pathlist)            
            if   count != 0:
                level =1         
                groupnav = ''        
                for mod in pathlist:                
                    key = str( pathlist[:level] )  #sample: key= " ['com' , 'org.openembedded.dev'] "
                    if  not treemap.has_key(key):                     
                        if level ==1:
                            parent = self.root
                        else:     
                            parent= treemap[ str( pathlist[:level-1] ) ]
                            
                        # leaf node => real module    
                        nodePydata = {}
                        if level == count:
                             
                            node = self.tree.AppendItem(parent , mod, image=11)

                            nodePydata['type'] = 'list'
                            nodePydata['name'] = package_dict['name']
                            nodePydata['manager'] = package_dict['manager']
                            nodePydata['treenav'] = self.objProjectData.name+'/'+package_dict['dir']
                            nodePydata['dir'] = package_dict['dir']

                            if self.current == nodePydata['treenav']:
                                selectItem = node
                              
                        else:
                            node = self.tree.AppendItem(parent , mod, image=6)    
                            groupnav += '/'+mod
                            nodePydata['type'] = 'group'
                            nodePydata['name'] = ''
                            nodePydata['manager'] = ''
                            nodePydata['treenav'] = self.objProjectData.name+groupnav
                            nodePydata['dir'] = ''
                            
                        self.tree.SetItemPyData(node, nodePydata)
                            
                        if not firstChild: 
                            firstChild = node
                                                  
                        treemap[ key ] = node   
                                          
                    level  += 1
                    
        self.tree.Expand(self.root)
        if firstChild:
            self.tree.Expand(firstChild)
        if filter:
            self.tree.ExpandAll()
        if selectItem:
            self.skipLoad = True
            self.tree.SelectItem(selectItem)
            self.skipLoad = False


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
            self.RecreateTree(False)
            return

        wx.BeginBusyCursor()  
        self.RecreateTree()            
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
        #itemText = self.tree.GetItemText(item)
        self.treePydata = self.tree.GetPyData(item)
        self.LoadDemo(self.treePydata['treenav'])
        self.SetStatusText(" Name: %s " % self.treePydata['treenav'],  0 )
        self.showLogTreeInfo()       
                         
    #---------------------------------------------
    def showLogTreeInfo(self):
        msg = ''
        if self.treePydata['type']=='root':
            msg += 'Name: '+self.objProjectData.name+'\n'
            msg += 'Manager: '+self.objProjectData.manager+'\n'
            msg += 'Local server: '+self.objProjectData.local+'\n'
            msg += 'Remote server: '+self.objProjectData.remote+'\n'
            msg += 'Description: '+self.objProjectData.description+'\n'
        elif self.treePydata['type']=='group':
            msg += 'Manager: '+self.treePydata['manager']+'\n'
        elif self.treePydata['type']=='list':
            msg += 'Manager: '+self.treePydata['manager']+'\n'
            msg += 'Source path: '+self.treePydata['dir']+'\n'
        
        self.log.Clear()    
        self.log.WriteText(msg)
        
    #---------------------------------------------     
    def LoadDemo(self, treenav=''):
        try:
            wx.BeginBusyCursor()
            self.Freeze()
            
            os.chdir( self.cwd )
            self.ShutdownDemoModule()
            
            demoName = 'versionmanagepanel'    

            if os.path.exists( GetOriginalFilename( demoName ) ):
                #wx.LogMessage( "Loading demo %s.py..." % demoName )
                
                if self.treePydata['type'] != 'group' and self.loaded: 
                    self.demoModules = DemoModules( demoName   )
                    self.LoadDemoSource(treenav)
                else :
                    self.SetOverview("wxPython", mainOverview)
                    self.codePage = None
                    self.UpdateNotebook(0)
            else:              
                self.SetOverview("wxPython", mainOverview)
                self.codePage = None
                self.UpdateNotebook(0)

        finally:
            wx.EndBusyCursor()
            self.Thaw()

    #---------------------------------------------
    def LoadDemoSource(self, modName=''):
        self.codePage = None
        self.codePage = versiondiffpanel.DiffPanel(self.nb, self, modName)
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
                self.demoPage = module.runVersionPanel(self, self.nb) #run version panel
            except:
                self.demoPage = errorpanel.ErrorPanel(self.nb, self.codePage,
                                               DemoError(sys.exc_info()), self)

            bg = self.nb.GetThemeBackgroundColour()
            if bg:
                self.demoPage.SetBackgroundColour(bg)

            assert self.demoPage is not None, "runTest must return a window!"
            
        else:
            # There was a previous error in compiling or exec-ing
            self.demoPage = errorpanel.ErrorPanel(self.nb, self.codePage,
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
        UpdatePage(self.demoPage, "Version ")

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
        self.SetAppName("GSP-Version")
        
        #splash = MySplashScreen()
        #splash.Show()
        frame = wxPythonDemo(None, "GSP Version Tool")
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





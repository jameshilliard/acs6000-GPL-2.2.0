#!/usr/bin/env python
# ex:ts=4:sw=4:sts=4:et
# -*- tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*-
##########################################################################
# Copyright (C) 2006-2007 Jiff Shen <jiff.shen@avocent.com>
# Copyright (C) 2006-2007 Avocent Corp
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA 02111-1307 USA.

##########################################################################
# Import and setup global variables
##########################################################################

try:
    set
except NameError:
    from sets import Set as set
import sys, os, re, commands, types
import gobject, gtk, gtk.glade
from bb import data, parse, build, fatal, cache, providers as Providers
from ConfigParser import ConfigParser

__version__ = "0.1.0"
__credits__ = """Bitbake IDE Version %s (C) 2006 Jiff Shen <jiff.shen@avocent.com>.""" % __version__

cmds = {}
leave_mainloop = False
last_exception = None
cooker = None
bb_cache = None
parsed = False
initdata = None

( 
    TASK_NAME_COL,
    TASK_DEPENDS_COL,
    TASK_STAMP_COL
) = range (3)

(FLAG_NAME_COL, FLAG_VALUE_COL) = range (2)

class BBFile:
    """ Provide functions and data for bb file"""

    def __init__(self, fn):
        self._fn = fn
        self.notify = {}
        self.d = bb_cache.loadDataFull(fn, cooker)
        data.update_data( self.d )


    def getVars (self, excludes = ("func", "matchesenv")):
        vars = {} 
        
        if excludes == None:
            excludes = ()

        for var in self.d.keys():
            skip = False
            flags = data.getVarFlags (var, self.d)

            for exclude in excludes:
                if exclude in flags:
                    skip = True
                    break
            if skip: continue

            val = data.getVar(var, self.d, 1)
            if type(val) is not types.StringType:
                continue
            oval = data.getVar(var, self.d, 0)
            vars[var] = [oval, val, flags]

        # vars = [ value, expanded value, flags ]
        return vars

    def  filterVars (self, patterns, vars, remove = False, reverse = False):
        if (vars == None):
            return {}
        
        _vars = {}

        for pattern in patterns:
            if pattern[1] == 1:
                regex = re.compile (pattern[0], 0)
            else:
                regex = 0

            for var in vars.keys():
                if (regex and regex.match(var)) or (var == pattern[0]):
                    _vars[var] = vars[var]
                    if (remove or reverse):
                        del vars[var]

        if (reverse):
            return vars
        else:
            return _vars
        
##########################################################################
# Class BBConfigParser
##########################################################################
class BBConfigParser(ConfigParser):
    def optionxform (self, string):
        return string

##########################################################################
# Class BitbakeIDE
##########################################################################

class BitbakeIDE:
    """ GUI for Navigate OpenEmbedded Metadata Repository"""

    groups = [ 
            ['Header' , [['DESCRIPTION', 0], ['SECTION', 0], ['LICENSE', 0],
                ['PRIORITY', 0], ['MAINTAINER', 0] ]],
            [ 'Common' , [['T', 0], [ 'S', 0], ['WORKDIR', 0], ['FILESDIR', 0],
                ['FILESPATH', 0] ]],
            ['Default' , [['^.*$', 1]]]
            ]
    
#            [ 'Package' , [ '^P$', '^PF$', '^PN$','^PV$', '^PR$', 
#                '^PACKAGES$', '^R?DEPENDS$', '^RRECOMMENDS$', '^SRC_URI$', 
#                '^A$', '^FILES(_.*)?$' ]],
    gladexml = os.path.dirname(__file__)+'/gsp.glade'
    tree2pop = None
    bbfile = None

    def __init__(self):
        ide = gtk.glade.XML(self.gladexml, 'window1')
        ide.signal_autoconnect(self)

        self._load_group ()
        self.ide = ide
        self._init_treeview1()
        self._init_treeview2()
        self._init_treeview3()
        self._init_treeview4()

    def _load_group (self):
        config = BBConfigParser ()
        try:
            config.read (os.environ.get("HOME") + "/.bbide")
        except IOError:
            return
        
        if not config.has_section('Default'):
            return

        if not config.has_option ('Default', 'KEY_GRP'):
            return

        sections = []
        groups = config.get('Default', 'KEY_GRP')
        groups = groups.split () 
        for section in groups:
            options = []
            for option in config.options ('KEY_GRP_' + section):
                options.append ([option, config.getint ('KEY_GRP_' + section,
                    option)])
#                print "append option %s\n" % ([option, config.getint (section, option)])

            sections.append ([section, options[:]])
#            print "append section %s\n" % ([section, options])

        self.groups = sections

    def _init_treeview1 (self):

        self.pkg_store = gtk.TreeStore (gobject.TYPE_STRING, 
                gobject.TYPE_STRING, 
                gobject.TYPE_STRING, 
                gobject.TYPE_INT,
                gobject.TYPE_STRING 
                )
        
        tree = self.ide.get_widget('treeview1')
        self.pkg_tree = tree
       
        selection = tree.get_selection()
        selection.set_mode (gtk.SELECTION_SINGLE)
        selection.connect('changed', self.treeview1_selection_changed_cb)

        tree.set_model (self.pkg_store)
        tree.set_reorderable(False)
        
        titles = ('Package', 'Latest', 'Preferred', 'Status', 'File')
        vis = (True, True, True, False, False)
        i = 0
        while (i < len (titles)):
            tvc = gtk.TreeViewColumn(titles[i], 
                                            gtk.CellRendererText(),
                                            text = i )
            tvc.set_resizable(True)
            tvc.set_clickable(True)
            tvc.set_expand(False)
            tvc.set_visible(vis[i])
            
            tree.append_column(tvc)
            i += 1

    def _init_treeview2 (self):

#        self.pkg_store = gtk.TreeStore (str, str, str, int)
        self.var_store = gtk.TreeStore (gobject.TYPE_STRING, 
                gobject.TYPE_STRING) 
        
        tree = self.ide.get_widget('treeview2')
        self.var_tree = tree
        
        selection = tree.get_selection()
        selection.set_mode (gtk.SELECTION_SINGLE)
        selection.connect('changed', self.treeview2_selection_changed_cb)

        tree.set_model (self.var_store)
        tree.set_reorderable(True)
        
        titles = ('Key', 'Definition')
        vis = (True, True)
        i = 0
        while (i < len (titles)):
            tvc = gtk.TreeViewColumn(titles[i], 
                                            gtk.CellRendererText(),
                                            text = i )
            tvc.set_resizable(True)
            tvc.set_clickable(True)
            tvc.set_expand(False)
            tvc.set_visible(vis[i])
            
            tree.append_column(tvc)
            i += 1
    
    def _init_treeview3 (self):
        self.flag_store = gtk.TreeStore (gobject.TYPE_STRING, 
                gobject.TYPE_STRING)

        self.flag_tree = self.ide.get_widget('treeview3')
        self.flag_tree.set_model (self.flag_store)
        self.flag_tree.set_reorderable (True)
        
        column = gtk.TreeViewColumn ('Flag', gtk.CellRendererText(),
                text=FLAG_NAME_COL)
        self.flag_tree.append_column (column)
        column = gtk.TreeViewColumn ('Value', gtk.CellRendererText(),
                text=FLAG_VALUE_COL)

        cells = column.get_cell_renderers ()
        for cell in cells:
            cell.set_property ('xalign', 0.01)

        self.flag_tree.append_column (column)

    def _init_treeview4 (self):
        self.task_store = gtk.TreeStore (gobject.TYPE_STRING, 
                gobject.TYPE_STRING,
                gobject.TYPE_BOOLEAN) 
        
        tree = self.ide.get_widget('treeview4')
        self.task_tree = tree
        
        tree.set_model (self.task_store)
        tree.set_reorderable(False)
        
        tvc = gtk.TreeViewColumn('Task', gtk.CellRendererText(), 
                text=TASK_NAME_COL)
        tree.append_column(tvc)

        tvc = gtk.TreeViewColumn('Depends', gtk.CellRendererText(), 
                text=TASK_DEPENDS_COL)
        tree.append_column(tvc)

        renderer = gtk.CellRendererToggle()
        renderer.set_data ("column", TASK_STAMP_COL)
        renderer.set_property ("xalign", 0.01)
        renderer.set_property ("activatable", True)
        renderer.connect ('toggled', self.treeview4_stamp_toggled_cb, self.task_store)

        tvc = gtk.TreeViewColumn('Stamp', renderer, active=TASK_STAMP_COL)
        tree.append_column(tvc)
    
    def _checkParsed( self ):
        if not parsed:
            self.parse(None)
                     
    def main (self):
        gtk.main ()

    def parse( self, params ):
        """(Re-)parse .bb files and calculate the dependency graph"""
        cooker.status = cooker.ParsingStatus()
        ignore = data.getVar("ASSUME_PROVIDED", cooker.configuration.data, 
                1) or ""
        cooker.status.ignored_dependencies = set( ignore.split() )
        cooker.handleCollections( data.getVar("BBFILE_COLLECTIONS", 
            cooker.configuration.data, 1) )

        cooker.collect_bbfiles( cooker.myProgressCallback )
        cooker.buildDepgraph()
        global parsed
        parsed = True

    def on_window1_delete_event(self, widget, event, userdata=None):
        gtk.main_quit()
        return False
                
    def on_button1_clicked (self, widget, userdata = None):
        self._checkParsed ()
        
        regex = re.compile (r"""^(.*\/).*_(.*)\.bb$""", 0)
        pkg_list = cooker.status.pkg_pn.keys()
        pkg_list.sort()
        for item in pkg_list:
            preferred = data.getVar( "PREFERRED_PROVIDER_%s" % item, 
                    cooker.configuration.data, 1 )
            if not preferred: preferred = item

            try:
                lv, lf, pv, pf = Providers.findBestProvider(preferred, 
                        cooker.configuration.data, cooker.status, 
                        cooker.build_cache_fail)
            except KeyError:
                lv, lf, pv, pf = (None,)*4
            
            if (lv == pv): pv = ""
            
            riter = self.pkg_store.append (None, [item, lv, pv, 0, pf] )
            pv_list = []
            for fn in cooker.status.pkg_pn[item]:
                ver = regex.sub (r'\2', fn)
                pv_list.append ((ver, fn))
             
            if (len (pv_list) <= 1):
                continue

            pv_list.sort()
            for (ver, fn) in pv_list:
                self.pkg_store.append (riter, ["", ver,"", 1, fn])

    def treeview1_selection_changed_cb (self, selection):
        model1, iter = selection.get_selected()
        if not iter: return False

        fn = model1.get_value(iter,4)

        self.var_store.clear()
        
        self.bbfile = BBFile (fn)
        pkg_vars = self.bbfile.getVars() 

        keys = pkg_vars.keys ()
        keys.sort ()
        
        fh = open (fn, 'r')
        tv1 = self.ide.get_widget('textview1')
        tv1.get_buffer().set_text (fh.read(), -1)
        fh.close()

        vbox3 = self.ide.get_widget ('vbox3')
        for child in vbox3.get_children ():
            child.destroy()

        for group in self.groups:
            vars = self.bbfile.filterVars (group[1], pkg_vars, True)
#            print vars

            if (len (vars) > 0):
                frame = gtk.Frame (group[0])
                iter = self.var_store.append (None, [group[0], ""])

                rows = len (vars)
                table = gtk.Table (rows, 2, False)

                row = 0
                keys = vars.keys ()
                keys.sort ()
                for var in keys:
                    self.var_store.append (iter, [var , vars[var][1]])

                    label = gtk.Label (var)
                    label.set_property ('xalign', 0.0)
                    label.set_property ('yalign', 0.0)
                    table.attach (label, 0, 1, row, row+1, gtk.FILL, 
                            gtk.EXPAND | gtk.FILL, 5, 2)
                    label = gtk.Label (vars[var][1])
                    label.set_property ('xalign', 0.0)
                    label.set_property ('yalign', 0.0)
                    label.set_line_wrap (True)
                    table.attach (label, 1, 2, row, row+1, 
                            gtk.EXPAND | gtk.FILL, gtk.EXPAND | gtk.FILL, 5, 5)
                    row += 1

                frame.add (table)
                vbox3.pack_start (frame, False, True, 5)
        vbox3.show_all ()

        self.task_tree.emit ('visibility-notify-event', gtk.gdk.Event (gtk.gdk.VISIBILITY_NOTIFY) )

    def treeview2_selection_changed_cb (self, selection):
        model1, iter = selection.get_selected()
        if not iter: return False
        if model1.iter_has_child (iter): return False

        var = model1.get_value (iter, 0 )

        self.flag_store.clear()

        flags = data.getVarFlags (var, self.bbfile.d)
        if not flags: return True

        for flag in flags.keys():
            self.flag_store.append (None, [flag, flags[flag]])

    def treeview2_button_press_event_cb (self, tree, event):
        if event.button != 3:
            return False
        
        if not self.tree2pop:
            self.tree2pop = gtk.Menu()
        else:
            self.tree2pop.foreach (self.tree2pop.remove)
            
        menuitem = gtk.MenuItem ('Group Options')
        menuitem.connect ('activate', self.treeview2_menu_activate_cb, None)
        self.tree2pop.add (menuitem)
        self.tree2pop.add (gtk.SeparatorMenuItem ())

        pathinfo = tree.get_path_at_pos (int (event.x), int (event.y))

        if pathinfo == None:
            menuitem = gtk.MenuItem ('Add Group')
            menuitem.connect ('activate', self.treeview2_menu_activate_cb, None)
            self.tree2pop.add (menuitem)
        else:
            tree.grab_focus ()
            tree.set_cursor (pathinfo[0], pathinfo[1], 0)

            menuitem = gtk.MenuItem ('Item Option')
            menuitem.connect ('activate', self.treeview2_menu_activate_cb, pathinfo)
            self.tree2pop.add (menuitem)

        self.tree2pop.show_all ()
        self.tree2pop.popup (None, None, None, event.button, event.time)

        return True
    def treeview2_menu_activate_cb (self, widget, userdata):
        print "%s activated, userdata %s " % (widget, userdata)

    def treeview1_row_activated_cb (self, treeview, path, column):
        print "hello world\n"

    
    def on_preference_activate (self, widget, userdata = None):
        prefxml = gtk.glade.XML (self.gladexml, 'dialog1')
        prefxml.signal_autoconnect(self)
        pref = prefxml.get_widget('dialog1')
        resp = pref.run ()
        print "run returns %d\n" % resp

        pref.destroy ()

    def on_dialog1_delete_event (self, widget, event, userdata = None):
        print "delete event\n"
        return True
    def treeview4_expose_event_cb (self, widget, event):
        print "expose event %s " % event

    def treeview4_visibility_notify_event_cb (self, widget, event):
        if not self.bbfile: return False

        if self.bbfile.notify.has_key ('treeview4'):
            return False
        else:
            self.task_store.clear ()

        task_graph = data.getVar('_task_graph', self.bbfile.d)
        
        for task in task_graph.allnodes ():
            completed = build.stamp_is_current(task, self.bbfile.d)
            parents = task_graph.getparents(task)
        
            self.task_store.append (None, [task, parents, completed])

        self.bbfile.notify['treeview4'] = 1

    def treeview4_stamp_toggled_cb (self, widget, path, userdata = None):
        iter = self.task_store.get_iter (path);
        if not iter: return False

        task, stamp = self.task_store.get (iter, TASK_NAME_COL, TASK_STAMP_COL)
        nostamp = data.getVarFlag (task, 'nostamp', self.bbfile.d)
        if nostamp and nostamp == "1":
            return False

        stampfile = "%s.%s" % (data.getVar('STAMP', self.bbfile.d, 1), task)

        try:
            if stamp and os.access(stampfile, os.F_OK):
                os.remove (stampfile)
            else:
                build.mkstamp (task, self.bbfile.d)
        except OSError, e:
            print ("Toggle stamp error %s" % e)
        else:
            self.task_store.set_value (iter, TASK_STAMP_COL, not stamp)

        return True

    def treeview4_cursor_changed_cb (self, tree, userdata = None):
        frame2 = self.ide.get_widget ('frame2')
        frame2.hide_all ()

        selection = tree.get_selection ()
        model, iter = selection.get_selected ()

        task = model.get_value (iter, 0)

        table = self.ide.get_widget('table2')
        table.foreach (table.remove)
        table.resize (1, 2)

        t = data.getVar ('T', self.bbfile.d, 1)
        try:
            files = os.listdir (t)
        except OSError:
            return False
        
        match = re.compile (r'''^(log|run)\.%s\.\d+''' % task, 0)
        
        dict = {}
        for fn in files:
            if match.match (fn):
                st = os.stat ("%s/%s" % (t, fn))
                
                dict[fn] = st.st_ctime
        del files 
        if not dict: 
            return False

        table.resize (len (dict) + 1, 2)

        group = None
        index = 0
        
        import time

        for fn in dict.keys():
            radio = gtk.RadioButton ( group, fn, False)
            if not group: group = radio

            table.attach (radio, 0, 1, index, index + 1) 
            label = gtk.Label (time.ctime(dict[fn]))
            table.attach (label, 1, 2, index, index + 1)
            index += 1
        
        del dict 

        table.attach (gtk.HSeparator(), 0, 2, index, index + 1)
        table.show_all()
        frame2.show_all ()
        

    def treeview4_select_cursor_row_cb (self, tree, userdata):
        print "select cursor row signal"

    def treeview4_row_activated_cb (self, tree, path, column):
        iter = self.task_store.get_iter (path)
        task = self.task_store.get_value (iter, 0)
        
        if build.stamp_is_current (task, self.bbfile.d, 0):
            return False
        else:
            build.exec_task (task, self.bbfile.d)


##########################################################################
# Start function - called from the Bitbake command line utility
##########################################################################

def start( aCooker ):
    global cooker,bb_cache

    cooker = aCooker
    cooker.cb = None
    bb_cache = cache.init (cooker)

    bbide = BitbakeIDE()
    bbide.main()

if __name__ == "__main__":
    print "SHELL: Sorry, this program should only be called by Bitbake."

#!/usr/bin/env python
# Author: Jiff Shen <jiff.shen@avocent.com>

import sys, string, copy

class Packages (object):
    def __init__(self, fn = "Packages"): 
        self.map = {}
        self._map = {}
        try:
            fh = open (fn, "r")
        except OSError:
            sys.stderr.write ( "cannot open %s\n" % fn)
            sys.exit(1)
        
        pn = ""
        for line in fh.readlines ():
            line = line.strip (' \n')
            # end of package
            if not line:
                pn = ""
                continue

            # invalid line
            if not ':' in line:
                continue

            var, val = line.split (':', 1)
            var = var.strip()
            val = val.strip()
            if var == "Package":
                pn = val
                self.map[val] = {} 
                self.map[val]['pn'] = val
            elif not pn:
                continue
            else:
                self.map[pn][var] = val

        fh.close ()
        for pkg in self.map.keys():
            self.convert (pkg)

        self._map = copy.deepcopy (self.map)


    def set (self, pkg, deps = []):
        self.map[pkg] = {}
        self.map[pkg]['pn'] = pkg
        self.map[pkg]['deps'] = deps
        self.map[pkg]['Depends'] = string.join (deps, ", ")

    def convert (self, x):
        deps = []
        if not self.map[x].has_key('deps'):
            self.map[x]['deps'] = []
            if self.map[x].has_key('Depends'):
                deps = self.map[x]['Depends'].split(',')
                deps = [f.strip() for f in deps]
                for dep in deps:
                    dep=dep.split()[0] 
                    if self.map.has_key(dep):
                        self.map[x]['deps'].append (dep)
            else:
                self.map[x]['deps'] = []

        if self.map[x].has_key('OE'):
            self.map[x]['stage'] = string.rsplit (self.map[x]['OE'], '-', 1)[0] + "-stage"
        else:
            self.map[x]['stage'] = ""

        
    def _tred (self, pn, li = [], cyclic = []):
        for dep in self.map[pn]['deps']:
            if not self.map.has_key(dep):
                self.map[pn]['deps'].remove (dep)
            elif dep in cyclic:
                sys.stderr.write ("Warning: cyclic loop dependency detected: %s->%s\n" % (pn, dep))
                self.map[pn]['deps'].remove (dep)
            elif self._tred (dep, li, cyclic + [pn]):
                self.map[pn]['deps'].remove (dep)

        if not self.map[pn]['deps']:
            del self.map[pn]
            li.append (pn)
            return True
        else:
            return False
    def tred (self, pn, default_deps = [], stage = False):
        li = []
        
        def get_stage (dict, deps):
            li = []
            for dep in deps:
                if dict.has_key (dep):
                    li.append (dict[dep]['stage'])
                else:
                    sys.stderr.write ("tred: Warning, %s not found\n" % dep)

            return li

        if stage:
            if not default_deps and self.map.has_key(pn):
                default_deps = self.map[pn]['deps']
            default_deps = get_stage(self.map, default_deps)
            pn = 'RDEP_'+pn

        if not self.map.has_key(pn):
            self.set (pn, default_deps)

        while not self._tred (pn,li): True
        li.pop ()

        self.map = copy.deepcopy (self._map)
        return li 

def main ():
    import optparse
    parser = optparse.OptionParser( version = "ipk dependence version 0.1", usage = """%prog -P Packages -p package [-s|--stage] [default depends]""" )

    parser.add_option( "-P", None, help = "Packages file",
            action = "store", dest = "info", default = 'Packages' )
    parser.add_option( "-p", None, help = "package name",
            action = "store", dest = "pkg", default = None )
    parser.add_option( "-s", "--stage", help = "get stage depends from RDEPENDS",
            action = "store_true", dest = "stage", default = False )

    options, args = parser.parse_args(sys.argv)
    if not options.pkg:
        sys.stderr.write ("package name not specified\n")
        return 1
    
    pkg = options.pkg
    info = Packages (options.info)
    
    default_deps = args [1:]

    li = info.tred (pkg, default_deps, options.stage)

    print string.join (li)
    return 0 
     
if __name__ == "__main__":
    main ()

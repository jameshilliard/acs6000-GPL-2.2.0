EXTRA_RSYNC             ?= ${@os.getenv('EXTRA_RSYNC','')}
IPK_SHARE_URI           ?= ${@os.getenv('IPK_SHARE_URI','')}
RSYNC_URI               = "${IPK_SHARE_URI}${@['/', '']['${IPK_SHARE_URI}'.endswith('/')]}"
INHIBIT_IPK_UPLOAD      ?= "0"

PSTAGE_TMPDIR           = "${TMPDIR}/tmp-stage"
PSTAGE_TMPDIR_STAGE     = "${PSTAGE_TMPDIR}/staging"
PSTAGE_TMPDIR_CROSS     = "${PSTAGE_TMPDIR}/cross"

PSTAGE_INSTALL_CMD      = "ipkg-cl install -force-downgrade -force-depends -f ${DEPLOY_DIR_IPK}/ipkg.conf -o "
PSTAGE_UPDATE_CMD       = "ipkg-cl update -f ${DEPLOY_DIR_IPK}/ipkg.conf -o "
PSTAGE_REMOVE_CMD       = "ipkg-cl remove -force-depends -recursive -f ${DEPLOY_DIR_IPK}/ipkg.conf -o "

PSTAGE_PKG_PREF         = ''
PSTAGE_PKG_SUFF         = '-stage'
PSTAGE_PKG              = "${PSTAGE_PKG_PREF}${PN}${PSTAGE_PKG_SUFF}"
PSTAGE_PKG_FN           = "${PSTAGE_PKG}_${PV}-${PR}_${PACKAGE_ARCH}.ipk"              

SPAWNFILE = "${STAGING_DIR}/pkgmaps/${PF}.spawn"
PACKAGEFUNCS_append = " do_write_ipk_list"

TARGET_ARCH_ORIGIN := ${TARGET_ARCH}

def inhibit_upload (d, ipk):
    import bb, re
    patterns = ('.+-dbg_.+', '.+-dev_.+', '.+-doc_.+', 'glibc-gconv-.+', 'glibc-charmap-.+', 
            r'''.+-locale-[a-z]{2}([\-\+][a-z]{2})?''', '.+-locale_.+', '.+-localedata-.*', 'locale-base-.+')
    for pattern in patterns:
        pattern = bb.data.expand (pattern, d)
        if re.match (pattern, ipk):
            return True 
    else:
        return False

python do_write_ipk_list () {
    import os, sys

    packages = bb.data.getVar('PACKAGES', d, 1)
    if not packages:
        bb.debug(1, "PACKAGES not defined, nothing to package")
        return

    # Generate ipk.conf if it or the stamp doesnt exist
    spawnfile = bb.data.getVar ('SPAWNFILE', d, 1)
    bb.mkdirhier (os.path.dirname (spawnfile))
    f = open(spawnfile, "w")
    for spawn in packages.split():
        #check if the packagename has changed due to debian shlib renaming
        pkgname = bb.data.getVar('PKG_%s' % spawn, d, 1)
        if not pkgname:
            pkgname = spawn

        arch = bb.data.getVar ('PACKAGE_ARCH_%s' % spawn, d, 1)
        if not arch:
            arch = bb.data.getVar ('PACKAGE_ARCH', d, 1)
        
        pv = bb.data.getVar ('PV', d, 1)
        pr = bb.data.getVar ('PR', d, 1)
        package = "%s_%s-%s_%s.ipk" % (pkgname, pv, pr, arch)
        if inhibit_upload (d, package):
            continue
        if os.path.exists (os.path.join (bb.data.getVar('DEPLOY_DIR_IPK', d, 1), package)):
            f.write("%s\n" % package)
    f.close()
}

def get_deps (d):
    import bb

    from __main__ import cooker
    deps = bb.utils.explode_deps (bb.data.getVar('DEPENDS', d, True) or [])
    rdeps = []
    if bb.data.getVar ('BUILD_ALL_DEPS', d, 1) == "1":
        rdeps = bb.utils.explode_deps (bb.data.getVar('RDEPENDS', d, True) or [])
    try:
        deps = cooker.get_all_depends (deps, rdeps, [], {})
    except AttributeError, e:
        return []

    return deps

def stage_get_deps (d, sep=' ', prefix='', suffix=''):
    import bb

    deps = get_deps (d)
    if not deps:
        return ""
    
    deps = map (lambda x: prefix+x+suffix, deps)

    return sep.join (deps)

python do_clean_append() {
    fn = bb.data.expand('${DEPLOY_DIR_IPK}/${PSTAGE_PKG_FN}', d)
    if os.path.exists (fn):
        os.remove (fn)
}

addtask prestage after do_fetch before do_patch
do_prestage[dirs] = "${DEPLOY_DIR_IPK}"
python pstage_do_prestage () {
    import bb, os, sys, re

    tmpdir = bb.data.getVar ('TMPDIR', d, 1)
    ipkdir = bb.data.getVar ('DEPLOY_DIR_IPK', d, 1)
    usrdir = os.path.join (tmpdir, 'usr')
    if os.path.islink (usrdir) and not os.path.exists (usrdir):
        os.remove (usrdir)

    if not os.path.lexists (usrdir):
        os.symlink (ipkdir, usrdir)

    def list_installed ():
        fp = os.popen ('ipkg-cl -o %s list_installed' % tmpdir)
        bb.debug (1, "List installed ipkgs")
        installed = {}
        m = re.compile (r'''^(\S+)\s+-\s+(\S+)\s+-''')
        for line in fp.readlines ():
            match = m.match (line)
            if not match:
                continue
            bb.debug (2, "%s - %s" % (match.group(1), match.group(2)))
            installed[match.group(1)] = match.group(2)
        del m
        fp.close ()
        return installed

    installed_map =  list_installed ()
    pstage_pkg = bb.data.getVar ('PSTAGE_PKG', d, 1)
    if installed_map.has_key (pstage_pkg):
        bb.note ("Removing already installed package %s" % pstage_pkg)
        os.system (bb.data.expand ('${PSTAGE_REMOVE_CMD} ${TMPDIR} %s' % pstage_pkg, d))
        del installed_map [pstage_pkg]

    fd = open ('ipkg.conf', "w", 0644)
    
    priority = 1 
    ipkgarchs="${BUILD_ARCH} all any noarch ${TARGET_ARCH_ORIGIN} ${PACKAGE_ARCHS} ${PACKAGE_EXTRA_ARCHS} ${MACHINE}"
    for arch in ipkgarchs.split ():
        fd.write ("arch %s %d\n" % (bb.data.expand (arch, d), priority))
        priority += 5
    
    fd.write ("src oe file:%s\n" % ipkdir)
    fd.close ()

    alldeps = get_deps (d)
    bb.debug (2, "Depends on %s" % alldeps)

#    assume_provided = bb.data.getVar ('ASSUME_PROVIDED', d, 1).split ()
    assume_staged = bb.data.getVar ('ASSUME_STAGED', d, 1).split ()
     
    deps = alldeps[:]
    for dep in alldeps:
        if not dep in assume_staged:
            deps.remove (dep)
            continue
        if installed_map.has_key (dep+"-stage"):
            deps.remove (dep)
            continue
    deps = map (lambda x: x+"-stage", deps)

    if gsp_enabled('IPK_DOWNLOAD', d) == "1":
        rsync_uri = bb.data.getVar ('RSYNC_URI', d, 1)
        if not rsync_uri:
            bb.fatal ("IPK_SHARE_URI is required for IPK_DOWNLOAD feature")
        else:
            opts = bb.data.getVar ('EXTRA_RSYNC', d, 1)
            for dep in deps:
                bb.note ("Synchronize %s from repository %s" % (dep, rsync_uri))
                retval = os.system ("rsync %s %s_*.ipk %s" % (opts, os.path.join (rsync_uri, dep), ipkdir))
                if retval:
                    bb.fatal ("Sychronize with %s failed" % rsync_uri)
                 
    retval = os.system ('ipkg-make-index -p Packages .')
    if retval:
        bb.fatal ("ipkg-make-index failed\n")

    if not os.path.exists ('Packages'):
        return True

    os.system ( bb.data.expand ("${PSTAGE_UPDATE_CMD} ${TMPDIR}", d))

    if deps:
        bb.debug (2, "Install ipk packages %s" % ' '.join (deps))
        if os.system ('%s %s %s' % (bb.data.getVar ('PSTAGE_INSTALL_CMD', d, 1), tmpdir, ' '.join (deps))):
            bb.fatal ("Installation %s failed" % deps )

    installed_map = list_installed ()
    for dep, ver in installed_map.items():
        dep = dep.rsplit ('-', 1)[0]
        spawnfile = "%s-%s.spawn" % (dep, ver)
        pkgmaps = bb.data.expand ("${STAGING_DIR}/pkgmaps/", d)
        spawnfile = os.path.join (pkgmaps, spawnfile)
        if not os.path.exists (spawnfile):
            continue
        fp = open (spawnfile, "r")
        for line in fp.xreadlines ():
            line = line.rstrip ('\n')
            if inhibit_upload (d, line):
                continue
            if os.path.exists (os.path.join (ipkdir, line)):
                continue
            bb.note ("Synchronize %s with repository %s" % (line, rsync_uri))
            retval = os.system ("rsync %s %s %s" % (opts, os.path.join (rsync_uri, line), ipkdir))
            if retval:
                retval >>= 8
                bb.debug (0, "rsync returned with error code %d" % retval)
                if retval == 20:
                    bb.fatal ("Keyboard Interrupt")
                elif retval == 23:
                    bb.error ("Sychronize with %s failed" % rsync_uri)
                
        fp.close ()
}

addtask pstage after do_populate_staging before do_ipk_upload
do_pstage[dirs] = "${T}  ${PSTAGE_TMPDIR} ${DEPLOY_DIR_IPK}"
python pstage_do_pstage () {
    import os, stat

    fns = {}
    queue = []
    def tred (graph, task):
        if task in queue:
            return
        tasks =  graph.getparents(task)
        for t in tasks:
            tred (graph, t)
        if not task in queue:
            queue.append (task)

    task_graph = bb.data.getVar ('_task_graph', d, 1)

    tred (task_graph, 'do_pstage')

    for task in queue:
        watch = bb.data.getVarFlag (task, 'watch', d)
        if watch != "1":
            continue

        fn = "%s/inotify.%s" % (bb.data.getVar ('T', d, 1), task)
        if os.access (fn, os.R_OK):
            fh = open (fn, "r")
            for line in fh.readlines ():
                line = line.rstrip ('\n')
                if not fns.has_key (line):
                    fns[line] = 1
            fh.close ()
        else:
            bb.note ("%s not exists" % fn)

    if not fns:
        bb.debug (2, "Creating empty archive for %s" % bb.data.expand ('${PN}-stage_${PV}-${PR}', d))

    os.system ("rm -rf %s" % bb.data.getVar ('PSTAGE_TMPDIR', d, 1))
    for fn in fns.keys():
        try:
            st = os.lstat (fn)
        except OSError, e:
            bb.debug (0, "%s" % e)
            continue
        if stat.S_ISDIR (st.st_mode):
            continue

        fm = fn.replace (bb.data.getVar ('TMPDIR', d, 1), bb.data.getVar ('PSTAGE_TMPDIR', d, 1))
        if stat.S_ISLNK (st.st_mode):
            if not os.path.exists (fn):
                bb.note ("ignore dangling link %s" % fn)
                continue
                
            fl = os.readlink (fn) 
            if fl.startswith ('/'):
                if not fl.startswith (bb.data.getVar ('TMPDIR', d, 1)):
                    bb.note ("%s points to %s" % (fn, fl))
                else:
                    fl = fl.rstrip ('/')
                    fldirs = fl.split ('/')
                    fndirs = fn.split ('/')
                    fldirs.pop()
                    fndirs.pop()
                    while len (fldirs) and len (fndirs):
                        if fldirs[0] == fndirs[0]:
                            fldirs.pop(0)
                            fndirs.pop(0)
                        else:
                            break

                    f = os.path.basename (fl)
                    fl = "../" * len (fndirs) + '/'.join (fldirs)
                    fl = os.path.join (fl, f)
                    os.unlink (fn)
                    os.symlink (fl, fn)

        try:
            dir = os.path.dirname (fm)
            bb.mkdirhier (dir)
            os.link (fn, fm)
        except OSError, e:
            bb.debug (0, "%s" % e)

    ctrldir = bb.data.expand ('${PSTAGE_TMPDIR}/CONTROL', d)
    bb.mkdirhier (ctrldir)

    vars = {'Package' : '${PSTAGE_PKG}', 'Version' : '${PV}-${PR}', 
        'Depends' : stage_get_deps(d, ', ', bb.data.getVar ('PSTAGE_PKG_PREF', d, 1), bb.data.getVar ('PSTAGE_PKG_SUFF', d, 1)), 
        'Section' : '${SECTION}',
        'Architecture' : '${PACKAGE_ARCH}',
        'Maintainer' : '${MAINTAINER}', 
        'Source' : '${SRC_URI}',
        'Description' : '${DESCRIPTION}',
        'OE' : '${PN}-${PV}',
        'HomePage' : '${HOMEPAGE}',
        'Priority' : 'Optional',
        'Provides' : '${PROVIDES}'
        }

    fh = file (ctrldir + "/control", "w")
    for (key, value) in vars.items (): 
        fh.write ("%s: %s\n" % (key, bb.data.expand (value, d)))
    fh.close ()

    os.system (bb.data.expand ("${IPKGBUILDCMD} ${PSTAGE_TMPDIR}", d))
    os.system ("ipkg-make-index -p Packages .")
    os.system (bb.data.expand ("${PSTAGE_UPDATE_CMD} ${TMPDIR}", d))
    os.system (bb.data.expand ("${PSTAGE_INSTALL_CMD} ${TMPDIR} ${PSTAGE_PKG}", d))

    os.system ("rm -rf %s" % bb.data.getVar ('PSTAGE_TMPDIR', d, 1))
}

addtask ipk_upload after do_pstage before do_build
do_ipk_upload[dirs] = "${DEPLOY_DIR_IPK}"
python pstage_do_ipk_upload () {
    import os, sys, re
    if gsp_enabled('IPK_UPLOAD', d) == "1":
        upload = bb.data.getVar('INHIBIT_IPK_UPLOAD', d, 1)
        if upload == "1":
            bb.note ("Upload disabled in bb-file for %s" % bb.data.getVar ('P', d, 1))
            return 0

        dest = bb.data.getVar ('RSYNC_URI', d, 1)

        extra_rsync = bb.data.getVar ('EXTRA_RSYNC', d, 1)

        spawnfile = bb.data.getVar ('SPAWNFILE', d, 1)
        packages = []
        if os.access (spawnfile, os.R_OK):
            f = open (spawnfile, "r")
            packages = f.readlines ()
            f.close ()
            packages = map (lambda x: x.rstrip('\n'), packages)
           
        pn = bb.data.getVar ('PN', d, 1)
        pv = bb.data.getVar ('PV', d, 1)
        pr = bb.data.getVar ('PR', d, 1)
        arch = bb.data.getVar ('PACKAGE_ARCH', d, 1)

        packages.append ("%s-stage_%s-%s_%s.ipk" % (pn, pv, pr, arch))
        for package in packages:
            if not inhibit_upload (d, package):
                if os.path.exists (package):
                    cmd = "rsync %s %s %s" % (extra_rsync, package, dest)
                    bb.note ("Uploading %s" % package)
                    if os.system (cmd):
                        bb.fatal ("Upload failed")
                    bb.note ("")
                else:
                    bb.error ("ipk %s not found" % package)
    else:
        bb.note ("Skipped")
}

EXPORT_FUNCTIONS do_prestage do_pstage do_ipk_upload

python () {
    import bb
    if bb.data.inherits_class ('native', d): 
        bb.data.setVarFlag ('do_install', 'watch', "1", d)
    if bb.data.inherits_class ('cross', d):
        bb.data.setVarFlag ('do_compile', 'watch', "1", d)
        bb.data.setVarFlag ('do_install', 'watch', "1", d)
    
    for klass in ('kernel', 'module', 'image_ipk', 'rootfs_ipk'):
        if bb.data.inherits_class (klass, d):
            bb.data.setVar ('INHIBIT_IPK_UPLOAD', '1', d)
            break
}

do_package[watch] = "1"
do_stage[watch] = "1"
do_populate_staging[watch] = "1"
do_pstage[watch] = "0"
addhandler pstage_eventhandler
python pstage_eventhandler() {
    import os, sys, stat, time
    from bb.event import Handled, NotHandled, getName
    
    name = getName(e)

    def watch_getdir (root, top = {}, parent = None):
        if parent == None:
            parent = top
        ldir = []
        for dir in root.split ('/'):
            if not dir: 
                continue
            ldir.append (dir)
            if not parent.has_key (dir):
                st = os.lstat ('/' + '/'.join (ldir))
                parent[dir] = {}
                parent[dir]['_key_size'] = st.st_size
                parent[dir]['_key_ctime'] = st.st_ctime
            parent = parent[dir]
        return parent
                
    def watch_update (root, files = [], top = {}, parent = None):
        changes = []
        if parent == None:
            parent = watch_getdir (root, top)
        for fn in files:
            st = os.lstat (os.path.join (root, fn))
            if parent.has_key (fn) and (st.st_ctime == parent[fn]['_key_ctime']):
                continue
            else:
                parent[fn] = {}
                parent[fn]['_key_size'] = st.st_size
                parent[fn]['_key_ctime'] = st.st_ctime
                changes.append (fn)
                
        return changes

            
    if name == 'TaskStarted':
        watch = bb.data.getVarFlag (e.task, 'watch', e.data)
        if watch == "1":
            watchdirs = bb.data.getVar ('WATCH', e.data, 0)
            if watchdirs == None:
                watchdirs = {}
                for dir in (bb.data.expand ('${CROSS_DIR}', e.data), bb.data.expand ('${STAGING_DIR}', e.data)):
                    if not os.path.lexists (dir):
                        bb.mkdirhier (dir)
                    for root, dirs, files in os.walk (dir):
                        watch_update (root, files, watchdirs)
                bb.data.setVar ('WATCH', watchdirs, e.data)
        else:
            bb.data.delVar ('WATCH', e.data)

    elif name.startswith('Task'):
        watch = bb.data.getVarFlag (e.task, 'watch', e.data)
        if watch == "1":
            ps_chg = []
            watchdirs = bb.data.getVar ('WATCH', e.data, 0)
            for dir in (bb.data.expand ('${CROSS_DIR}', e.data), bb.data.expand ('${STAGING_DIR}', e.data)):
                for root, dirs, files in os.walk (dir):
                    if not files:
                        continue
                    updates = watch_update (root, files, watchdirs)
                    ps_chg.extend (map (lambda x: root + "/" + x, updates))

            bb.data.setVar ('WATCH', watchdirs, e.data)

            inf = bb.data.expand ('${T}/inotify.%s' % e.task, e.data)
            tmp = bb.data.getVar ('TMPDIR', e.data, 1)
            if ps_chg:
                f = open (inf, "w")
                for fn in ps_chg:
                    fn.replace (tmp, '')
                    f.write (fn + "\n")
                f.close ()
                ps_chg = []

    return NotHandled
}

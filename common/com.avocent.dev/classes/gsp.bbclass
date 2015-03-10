inherit base

GSP_FEATURES ?= ""

def gsp_enabled (feat, d):
    import bb
    features = bb.data.getVar('GSP_FEATURES', d, 1)
    features = features.split()

    if feat in features:
        return "1"
    else:
        return "0"

def confirm (question, var, flag, d):
    import bb
    answer = bb.data.getVarFlag (var, flag, d)
    if answer == None:
        while (True):
            try:
                answer = raw_input (question)
            except Exception, e: 
                answer = "no"
                break

            if answer == "yes":
                break
            elif answer == "no":
                bb.note ("Cancelled")
                break
        bb.data.setVarFlag (var, flag, answer, d)
    return answer

do_fetch[bbdepcmd] = ""
do_fetch[nostamp] = "1"
python __anonymous () {
    import bb

    if gsp_enabled('NOFETCH', d) == "1":
        bb.data.setVarFlag ('do_fetch', 'content', "    bb.build.exec_func('gsp_fake_fetch', d)\n", d)
        bb.data.setVarFlag ('do_fetch', 'python', "1", d)
        bb.data.setVarFlag ('do_fetch', 'force', "1", d)
        if gsp_enabled('SRC_CHECK', d) == "1":
            bb.data.setVarFlag ('do_build', 'nostamp', "1", d)
}

python gsp_fake_fetch () {
    import re, os

    bb.data.update_data(d)

    src_uri = bb.data.getVar('SRC_URI', d)
    if not src_uri:
        return
    src_uri = bb.data.expand(src_uri, d)
    ls = []
    for uri in src_uri.split():
        try:
            l = bb.data.expand(bb.fetch.localpath(uri, d), d)
        except bb.MalformedUrl, e:
            raise FuncFailed('Unable to generate local path for malformed uri: %s' % e)
        # dont need any parameters for extraction, strip them off
        l = re.sub(';.*$', '', l)
        if os.access (l, os.R_OK):
            ls.append (os.path.realpath (l))
            continue

        fn = os.path.basename (l)
        for dir in bb.data.getVar ('FILESPATH', d, 1).split(":"):
            if os.access (os.path.join (dir, fn), os.R_OK):
                ls.append (os.path.realpath (os.path.join (dir, fn)))
                break
        else:
            bb.fatal ("Not fetched URI %s" % uri)

    bbfile = bb.data.getVar ('FILE', d, 1)
    if not bbfile in ls:
        ls.append (bbfile)

    import md5
    md5sum = md5.new ()
    ls.sort ()
    for l in ls:
        if os.path.isfile (l):
            fp = open (l, "r")
            for line in fp.xreadlines ():
                md5sum.update (line)
            fp.close ()
        elif os.path.isdir (l):
            import stat, re
            for root, dirs, files in os.walk (l):
                dirs.sort()
                for dir in dirs[:]:
                    if dir == ".svn" or dir == "CVS":
                        dirs.remove (dir)
                files.sort()
                for fn in files:
                    for pat in ('.*~$', '.*\.swap$', '.*\.temp$', '.*\.temp$', '^svn-commit\..*'):
                        if re.match (pat, fn):
                            break
                    else:
                        pn = os.path.join (root, fn)
                        if not os.path.isfile (pn):
                            continue
                        fp = open (pn, "r")
                        for line in fp.xreadlines ():
                            md5sum.update (line)
                        fp.close ()
        else:
            bb.note ("Not regular file %s" % l)
    digest = md5sum.hexdigest()
    del md5sum
    bb.debug (2, "Source files digest %s" % digest)

    bb.data.setVarFlag ('do_fetch', 'md5sum', digest, d)

    if gsp_enabled ("SRC_CHECK", d) == "1":
        stampfile = "%s.%s" % (bb.data.getVar ('STAMP', d, 1), 'do_fetch')
        if os.access (stampfile, os.R_OK):
            fp = open (stampfile, "r")
            olddigest = fp.readline () 
            fp.close ()
            bb.debug (1, "Old digest %s" % olddigest)
            if olddigest != digest:
                question = "Source updated, rebuild [yes/no]? "
                answer = confirm ("Source code updated, rebuild [yes/no]?", 'CLEAN', 'answer', d)
                if answer == 'yes':
                    bb.build.exec_task ('do_clean', d)
                else:
                    bb.data.delVarFlag ('do_fetch', 'md5sum', d)
}

addtask objclean
EXTRA_OBJCLEAN ?= "clean"
OBJCLEAN_BOUNDARIES ?= "do_patch do_unpack do_fetch"
do_objclean[nostamp] = "1"
do_objclean[dirs] = "${S}"
do_objclean[bbdepcmd] = ""
gsp_do_objclean () {
    oe_runmake ${EXTRA_OBJCLEAN}
    for stamp in $(ls -t ${STAMP}.* | sed -r 's,${STAMP}\.,,' )
    do
        for exclude in ${OBJCLEAN_BOUNDARIES}
        do
            if [ $exclude == $stamp ]
            then
                return 0
            fi
        done
        rm -f ${STAMP}.$stamp
    done
} 

python do_clean_prepend () {
    answer = confirm ("About to erase %s\nAre you sure [yes/no]? " % bb.data.expand ('${WORKDIR}', d), 'CLEAN', 'answer', d)
    if answer != 'yes':
        return
}

python gsp_do_rebuild_prepend () {
    answer = confirm ("About to erase %s\nAre you sure [yes/no]? " % bb.data.expand ('${WORKDIR}', d), 'CLEAN', 'answer', d)
    if answer != 'yes':
        return
}

EXPORT_FUNCTIONS do_objclean 

# define PREMIRRORS to avocent gsp repository
PREMIRRORS[func] = "0"
PREMIRRORS() {
http://.*/.* svnget://${GSP_REPOS}/sources/oe
ftp://.*/.* svnget://${GSP_REPOS}/sources/oe
}

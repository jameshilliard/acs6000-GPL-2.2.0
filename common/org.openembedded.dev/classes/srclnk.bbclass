DEPLOY_DIR_SRCLNK ?= "${DEPLOY_DIR}/srclnk"

# used as part of a path. make sure it's set
DISTRO ?= "openembedded"

def get_src_tree(d):
    import bb
    import os, os.path

    workdir = bb.data.getVar('WORKDIR', d, 1)
    if not workdir:
        bb.error("WORKDIR not defined, unable to find source tree.")
        return

    s = bb.data.getVar('S', d, 0)
    if not s:
        bb.error("S not defined, unable to find source tree.")
        return

    s_tree_raw = s.split('/')[1]
    s_tree = bb.data.expand(s_tree_raw, d)

    src_tree_path = os.path.join(workdir, s_tree)
    try:
        os.stat(src_tree_path)
    except OSError:
        bb.fatal("Expected to find source tree in '%s' which doesn't exist." % src_tree_path)
    bb.debug("Assuming source tree is '%s'" % src_tree_path)

    return s_tree

#def get_pkg_src_def (d):
#    import bb
#    import os, os.ospath
#    deploydir = bb.data.getVar('DEPLOY_DIR', d, 1);
#
#    if not deploydir:
#        bb.error("DEPLOY_DIR not defined, unable to find package source tree definition.")
#        return
#
#    pkglnkdir = os.path.join(deploydir, "pkglnk");
#    try:
#        os.listdir (pkglnkdir)
#    except OSError:
#        bb.note ("No package workdir soft link created");


srclnk_do_create_symlink (){

    [ -d ${DEPLOY_DIR_SRCLNK} ] || mkdir -p ${DEPLOY_DIR_SRCLNK}

    src_tree=${@get_src_tree(d)}
    
    oenote "Creating symlink ${PN} in ${DEPLOY_DIR_SRCLNK}"
    ln -snf ${S} ${DEPLOY_DIR_SRCLNK}/${PN}
}

do_create_symlink[dirs] = "${S}"

EXPORT_FUNCTIONS do_create_symlink

addtask create_symlink after do_unpack before do_patch


MAINTAINER = "Lynn Lin <lynn.lin@avocent.com>"
DESCRIPTION = "remove useless .svn dir "

do_cleansvndir () {
 (cd ${WORKDIR}; rm -rf $(find . -name ".svn");)
}

EXPORT_FUNCTIONS do_cleansvndir
addtask cleansvndir  before do_configure after do_unpack do_patch

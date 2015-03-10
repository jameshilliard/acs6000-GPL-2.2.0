# This program is the util of gspmgmt, all the logic are here, can be used by
# command line and GUI interface.
#
# Author: Ice Liu
# History: Drafted on Jun 18, 2007

SVN_URL = "https://sha-svn.corp.avocent.com/svn/avocent-gsp/"
SVN_LOCAL_URL = "https://sha-svn.corp.avocent.com/svn/avocent-gsp/"

# Currently, store the mapping of project and modules into a dictionary.
# Will be moved into xml file later.
modict = {
    "all":"common/com.avocent.dev \
    common/org.openembedded.dev \
    projects/mergepoint \
    utils \
    projects/hebei \
    sources/generic/cmd-uboot \
    sources/generic/lzma \
    bitbake",
    "common":"common/com.avocent.dev common/org.openembedded.dev",
    "mergepoint":"projects/mergepoint",
    "utils":"utils",
    "hebei":"projects/hebei",
    "cmd-uboot":"sources/generic/cmd-uboot",
    "lzma":"sources/generic/lzma",
    "bitbake":"bitbake"
    }

# To verify the project is valid or not via name
def isvalid(mname):
    if modict.has_key(mname):
    	return True
    else:
	return False


# To get all the relative svn path of a module,
# currently it is from a dictionary,
# and it will be from a xml file definition in the future.
def getmods(mname):
    return modict[mname]



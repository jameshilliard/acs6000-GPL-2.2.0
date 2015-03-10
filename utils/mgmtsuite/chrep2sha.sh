#!/bin/bash
##
## chrep2sha.sh
## 
## Made by Ice Liu
## Login   <ice@sha-iliu-m2>
## 
## Started on  Fri Mar 21 14:16:39 2008 Ice Liu
## Last update Fri Mar 21 14:16:39 2008 Ice Liu
##
## This program is for changing the repository from https://fre-svn/ to https://sha-svn
## then changing from https/svn://sha-svn/svn/avocent-gsp to https/svn://sha-svn/svn/avocent-sha
##

function updatesvninfo
{
    echo "Update the URL of fre-svn.corp.avocent.com to sha-svn.corp.avocent.com and update svn:// to https://, "
    sed $1 -e 's/fre-svn/sha-svn/' | sed -e 's/svn:\/\//https:\/\//' | sed -e 's/sha-scm.corp.avocent.com\/avocent-gsp/sha-svn.corp.avocent.com\/svn\/avocent-gsp/'> $1.tmp
    #diff $1 $1.tmp
    mv $1.tmp $1
}

# the SVN URL or repository recorded in $DIR/.svn/entries files, find them and update them
export PATH=/opt/utils/mgmtsuite:$PATH
find ${PWD}/ -name "entries" > entries.list

while read LINE
do
    echo ""
    echo "file name: $LINE"
    updatesvninfo $LINE
done < entries.list

rm -f entries.list

exit 0

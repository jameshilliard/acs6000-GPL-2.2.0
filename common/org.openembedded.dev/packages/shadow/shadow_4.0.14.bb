DESCRIPTION = "shadow"
SECTION = "sys/apps"
LICENSE = "GPL"
DEPENDS = "libpam"

SRC_URI = "ftp://ftp.pld.org.pl/software/shadow/old/shadow-${PV}.tar.bz2 \
        file://uclibc.patch;patch=1 \
        file://passwd-P-option.patch;patch=1 \
        file://fix-configure.patch;patch=1 \
        file://su.patch;patch=1 \
        file://login.defs \
        file://login.access \
        file://limits \
        file://login \
        file://shadow \
        file://passwd \
        file://system-auth \
        file://su \
        file://other \
        file://chage \
        file://chsh \
        file://chfn \
        file://chpasswd \
        file://groupadd \
        file://groupdel \
        file://groupmod \
        file://useradd \
        file://userdel \
        file://usermod \
        file://newusers \
        file://passwd.passwd \
        file://shadow.passwd \
        file://group.passwd \
        file://default_useradd \
"
SRC_URI_append_GSP = " \
        file://permit \
        file://dsviewdownlocal \
        file://dsview \
        file://dsviewlocal \
        file://kerberosdownlocal \
        file://kerberos \
        file://kerberoslocal \
        file://ldapdownlocal \
        file://ldapdownlocal-radius \
        file://ldap \
        file://ldaplocal \
        file://local \
        file://localradius \
        file://localtacplus \
        file://none \
        file://otp \
        file://otplocal \
        file://radiusdownlocal \
        file://radius \
        file://radiuslocal \
        file://samba \
        file://s_ldapdownlocal-radius \
        file://s_local \
        file://smbdownlocal \
        file://smb \
        file://smblocal \
        file://s_radius \
        file://s_tacplus \
        file://sys-passwd \
        file://tacplusdownlocal \
        file://tacplus \
        file://tacpluslocal \
        file://localnis \
        file://nisdownlocal \
        file://nis \
        file://nislocal \
        file://s_nis \
"

inherit autotools gettext

EXTRA_OECONF = "--without-selinux "

FILES_${PN} += "\
	/usr/share/locale/ja_JP* \
	/usr/share/locale/zh_CN* \
	"
do_install_append () {
	
	olddir=`pwd`

	install -d -m 0755 ${D}/etc
	install -d -m 0755 ${D}/etc/pam.d
	install -d -m 0755 ${D}/etc/default

	install -m 0644 ${WORKDIR}/login  ${D}/etc/pam.d
	install -m 0600 ${WORKDIR}/passwd  ${D}/etc/pam.d
	install -m 0600 ${WORKDIR}/shadow  ${D}/etc/pam.d
	install -m 0644 ${WORKDIR}/system-auth  ${D}/etc/pam.d
	install -m 0600 ${WORKDIR}/su  ${D}/etc/pam.d
	install -m 0644 ${WORKDIR}/other  ${D}/etc/pam.d

	install -m 0600 ${WORKDIR}/ch{age,sh,fn,passwd} ${D}/etc/pam.d
	install -m 0600 ${WORKDIR}/group{add,mod,del} ${D}/etc/pam.d
	install -m 0600 ${WORKDIR}/user{add,mod,del} ${D}/etc/pam.d
	install -m 0600 ${WORKDIR}/newusers ${D}/etc/pam.d

	install -m 0644 ${WORKDIR}/limits ${D}/etc
	install -m 0600 ${WORKDIR}/login.access ${D}/etc
	install -m 0644 ${WORKDIR}/login.defs ${D}/etc
	install -m 0644 ${WORKDIR}/passwd.passwd ${D}/etc/passwd
	install -m 0644 ${WORKDIR}/shadow.passwd ${D}/etc/shadow
	install -m 0644 ${WORKDIR}/group.passwd ${D}/etc/group
	install -m 0644 ${WORKDIR}/default_useradd ${D}/etc/default/useradd

    if [ "${SHARECODE}" = "GSP" ]; then
        for file in dsview ldaplocal passwd s_radius dsviewdownlocal \
                    local permit s_tacplus dsviewlocal localnis radius su \
                    groupadd localradius radiusdownlocal sys-passwd \
                    groupdel localtacplus radiuslocal tacplus \
                    groupmod nis samba tacplusdownlocal kerberos \
                    nisdownlocal s_ldapdownlocal-radius tacpluslocal \
                    kerberosdownlocal nislocal s_local useradd \
                    kerberoslocal none smb userdel ldap other smbdownlocal \
                    usermod ldapdownlocal otp smblocal \
                    ldapdownlocal-radius otplocal s_nis	
        do
	        install -m 0600 ${WORKDIR}/$file  ${D}/etc/pam.d
        done
    fi
}

do_install_append_pmhd () {
    for file in $(grep -l "auth.*optional.*pam_shm.so" ${D}/etc/pam.d/*)
    do
      sed -i "s/\(^[[:space:]]*\)\(auth\)\([[:space:]]*\)\(optional\)\(.*\)\(pam_shm.so.*$\)/\1\2\3\4\5\6\n\1\2\3[success=ok default=1]\5pam_pmcli.so\n\1\2\3required\5pam_redirect.so service=permit/g" $file
    done
}

do_install_append_acs6000() {
        install -d ${D}/usr/share
        install -d ${D}/usr/share/locale
        install -d ${D}/usr/share/locale/ja_JP.UTF-8
        install -d ${D}/usr/share/locale/ja_JP.UTF-8/LC_MESSAGES
        install -m 0644 ${S}/po/ja.gmo ${D}/usr/share/locale/ja_JP.UTF-8/LC_MESSAGES/shadow.mo
        install -d ${D}/usr/share/locale/zh_CN.UTF-8
        install -d ${D}/usr/share/locale/zh_CN.UTF-8/LC_MESSAGES
        install -m 0644 ${S}/po/zh_CN.gmo ${D}/usr/share/locale/zh_CN.UTF-8/LC_MESSAGES/shadow.mo
}

pkg_postinst () {
if test "x$D" = "x"; then
# create /etc/shadow
  pwconv
  [ -d /home ] || (install -d /home; chmod 775 /home)
  if [ $(grep -c "^admin:" /etc/passwd) -eq 1 -a ! -d /home/admin ] ; then
    install -d -m 755 $(grep "^admin:" /etc/passwd | awk -F ':' '{ print "-o " $3 " -g " $4 }') /home/admin
    install -d $(grep "^admin:" /etc/passwd | awk -F ':' '{ print "-o " $3 " -g " $4 }') /home/admin/.ssh
	chmod 700 /home/admin/.ssh
  fi
  [ -d /home/root/.ssh ] || (install -d /home/root/.ssh; chmod 700 /home/root/.ssh)
else
  exit 1
fi
}

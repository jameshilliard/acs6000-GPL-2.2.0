DESCRIPTION = "Ncurses library"
HOMEPAGE = "http://www.gnu.org/software/ncurses/ncurses.html"
LICENSE = "MIT"
SECTION = "libs"
DEPENDS = "ncurses-native"
PACKAGES_prepend = "ncurses-tools "
PACKAGES_append = " ncurses-terminfo"
FILES_ncurses_append = " ${datadir}/tabset"
RSUGGESTS_${PN} = "ncurses-terminfo"
RPROVIDES = "libncurses5"

inherit autotools

# This keeps only tput/tset in ncurses
# clear/reset are in already busybox
FILES_ncurses-tools = "${bindir}/tic ${bindir}/toe ${bindir}/infotocap ${bindir}/captoinfo ${bindir}/infocmp ${bindir}/clear.${PN} ${bindir}/reset.${PN} ${bindir}/tack "
FILES_ncurses-terminfo = "${datadir}/terminfo"
FILES_${PN} = "${bindir}/tput ${bindir}/tset ${libdir}/lib*.so.* usr/share/tabset etc/terminfo"

PARALLEL_MAKE=""

EXTRA_OECONF = "--with-shared \
	        --without-profile \
	        --without-debug \
	        --disable-rpath \
	        --enable-echo \
	        --enable-const \
	        --without-ada \
	        --enable-termcap \
	        --without-cxx-binding \
	        --with-terminfo-dirs=${datadir}/terminfo \
	        --enable-overwrite"
export BUILD_CCFLAGS = "-I${S}/ncurses -I${S}/include ${BUILD_CFLAGS}"
export BUILD_LDFLAGS = ""
export EXTRA_OEMAKE = '"BUILD_LDFLAGS=" "BUILD_CCFLAGS=${BUILD_CCFLAGS}"'

do_stage() {
	autotools_stage_all
	ln -sf curses.h ${STAGING_INCDIR}/ncurses.h
	ln -sf libncurses.so ${STAGING_LIBDIR}/libtermcap.so
	ln -sf libncurses.a ${STAGING_LIBDIR}/libtermcap.a
}

do_install() {
	autotools_do_install

	# our ncurses has termcap support
	ln -sf libncurses.so ${D}${libdir}/libtermcap.so
	ln -sf libncurses.a ${D}${libdir}/libtermcap.a

	# include some basic terminfo files
	# stolen ;) from gentoo and modified a bit
	# for x in ansi console dumb linux rxvt screen sun vt{52,100,102,200,220} xterm-color xterm-xfree86
  TERMS="1178 1730-lm 2621 2621-wl 2621A 2621a 386at 3b1 4025ex 4027ex 4410-w 5051 5410-w 5620 5630-24 5630DMD-24 6053 6053-dg 605x 605x-dg 630-lm 630MTG-24 730MTG-24 730MTG-41 730MTG-41r 730MTGr 730MTGr-24 8510 955-hb 955-w \
         Apple_Terminal-ascii Apple_Terminal-ascii-m Apple_Terminal-m Apple_Terminal \
         Eterm Eterm-color LFT-PC850 MtxOrb MtxOrb162 MtxOrb204 NCR260VT300WPP NCRVT100WPP \
         P12 P12-M P12-M-W P12-W P14 P14-M P14-M-W P14-W P4 P5 P7 P8 P8-W P9 P9-8 P9-8-W P9-W \
         Q306-8-pc Q310-vip-H Q310-vip-H-am Q310-vip-Hw Q310-vip-w Q310-vip-w-am X-hpterm \
         a210 a80 a980 aa4080 aaa aaa+dec aaa+rv aaa+unk aaa-18 aaa-18-rv aaa-20 aaa-22 aaa-24 aaa-24-rv aaa-26 aaa-28 aaa-30 aaa-30-ctxt aaa-30-rv aaa-30-rv-ctxt aaa-30-s aaa-30-s-ctxt aaa-30-s-rv aaa-30-s-rv-ct aaa-36 aaa-36-rv \
         aaa-40 aaa-40-rv aaa-48 aaa-48-rv aaa-60 aaa-60-dec-rv aaa-60-rv aaa-60-s aaa-60-s-rv aaa-ctxt aaa-db aaa-rv aaa-rv-ctxt aaa-rv-unk aaa-s aaa-s-ctxt aaa-s-rv aaa-s-rv-ctxt aaa-unk aas1901 abm80 abm85 abm85e abm85h abm85h-old \
         act4 act5 addrinfo adds980 addsviewpoint addsvp60 adm+sgr adm1 adm11 adm1178 adm12 adm1a adm2 adm20 adm21 adm22 adm3 adm31 adm31-old adm36 adm3a adm3a+ adm42 adm42-ns adm5 aepro aixterm aixterm-m aixterm-m-old aj aj510 aj830 \
         aj832 alt2 alt3 alt4 alt5 alt7 alt7pc alto-h19 alto-heath altoh19 altoheath altos-2 altos-3 altos-4 altos-5 altos2 altos3 altos4 altos5 altos7 altos7pc ambas ambassador amiga amiga-8bit amiga-h amiga-vnc amp219 amp219w ampex-219 \
         ampex-219w ampex-232 ampex175 ampex175-b ampex210 ampex219 ampex219w ampex232 ampex232w ampex80 annarbor4080 ansi ansi+arrows ansi+csr ansi+cup ansi+erase ansi+idc ansi+idl ansi+idl1 ansi+inittabs ansi+local ansi+local1 ansi+pp ansi+rca \
         ansi+rep ansi+sgr ansi+sgrbold ansi+sgrdim ansi+sgrso ansi+sgrul ansi+tabs ansi-color-2-emx ansi-color-3-emx ansi-emx ansi-generic ansi-m ansi-mini ansi-mono ansi-mr ansi-mtabs ansi-nt ansi.sys ansi.sys-old ansi.sysk ansi43m ansi77 ansi80x25 \
         ansi80x25-mono ansi80x25-raw ansi80x30 ansi80x30-mono ansi80x43 ansi80x43-mono ansi80x50 ansi80x50-mono ansi80x60 ansi80x60-mono ansil ansil-mono ansis ansis-mono ansisysk ansiw ap-vm80 apl apollo apollo_15P apollo_19L apollo_color \
         apple-80 apple-ae apple-soroc apple-uterm apple-uterm-vb apple-videx apple-videx2 apple-videx3 apple-vm80 apple2e apple2e-p apple80p appleII appleIIc appleIIe appleIIgs arm100 arm100-am arm100-w arm100-wam at386 atari att2300 att2350 \
         att4410 att4410-w att4410v1 att4410v1-w att4415 att4415+nl att4415-nl att4415-rv att4415-rv-nl att4415-w att4415-w-nl att4415-w-rv att4415-w-rv-n att4418 att4418-w att4420 att4424 att4424-1 att4424m att4425 att4425-nl att4425-w att4426 \
         att500 att505 att505-24 att510a att510d att513 att5310 att5320 att5410 att5410-w att5410v1 att5410v1-w att5418 att5418-w att5420 att5420+nl att5420-nl att5420-rv att5420-rv-nl att5420-w att5420-w-nl att5420-w-rv att5420-w-rv-n att5420_2 \
         att5420_2-w att5425 att5425-nl att5425-w att5430 att5620 att5620-1 att5620-24 att5620-34 att5620-s att605 att605-pc att605-w att610 att610-103k att610-103k-w att610-w att615 att615-103k att615-103k-w att615-w att620 att620-103k att620-103k-w \
         att620-w att630 att630-24 att6386 att700 att730 att730-24 att730-41 att7300 att730r att730r-24 att730r-41 avatar avatar0 avatar0+ avatar1 avt avt+s avt-ns avt-rv avt-rv-ns avt-rv-s avt-s avt-w avt-w-ns avt-w-rv avt-w-rv-ns avt-w-rv-s avt-w-s aws awsc \
         b-128 bantam basic4 basis bct510a bct510d beacon bee beehive beehive3 beehive4 beehiveIIIm beterm \
         bg1.25 bg1.25nv bg1.25rv bg2.0 bg2.0nv bg2.0rv bg3.10 bg3.10nv bg3.10rv bh3m bh4 bitgraph blit bobcat \
         bq300 bq300-8 bq300-8-pc bq300-8-pc-rv bq300-8-pc-w bq300-8-pc-w-rv bq300-8rv bq300-8w bq300-pc bq300-pc-rv \
         bq300-pc-w bq300-pc-w-rv bq300-rv bq300-w bq300-w-8rv bq300-w-rv bsdos-pc bsdos-pc-m bsdos-pc-mono bsdos-pc-nobold bsdos-ppc bsdos-sparc \
         c100 c100-1p c100-4p c100-rv c100-rv-4p c104 c108 c108-4p c108-8p c108-rv c108-rv-4p c108-rv-8p c108-w c108-w-8p c300 c301 c321 ca22851 \
         cad68-2 cad68-3 cbblit cbunix cci cci1 cdc456 cdc721 cdc721-esc cdc721ll cdc752 cdc756 cg7900 cgc2 cgc3 chromatics ci8510 cit-80 cit101 \
         cit101e cit101e-132 cit101e-n cit101e-n132 cit101e-rv cit500 cit80 citc citoh citoh-6lpi citoh-8lpi citoh-comp citoh-elite citoh-pica \
         citoh-prop citoh-ps coco3 coherent color_xterm colorscan commodore concept concept-avt concept100 concept100-rv concept108 concept108-4p \
         concept108-8p concept108-w-8 concept108-w8p concept108rv4p cons25 cons25-iso-m cons25-iso8859 cons25-koi8-r cons25-koi8r-m cons25-m \
         cons25l1 cons25l1-m cons25r cons25r-m cons25w cons30 cons30-m cons43 cons43-m cons50 cons50-iso-m cons50-iso8859 cons50-koi8r \
         cons50-koi8r-m cons50-m cons50l1 cons50l1-m cons50r cons50r-m cons60 cons60-iso cons60-iso-m cons60-koi8r cons60-koi8r-m \
         cons60-m cons60l1 cons60l1-m cons60r cons60r-m contel300 contel301 contel320 contel321 cops cops-10 cops10 crt crt-vt220 \
         cs10 cs10-w ct82 ct8500 ctrm cx cx100 cyb110 cyb83 cygwin cygwinB19 \
         d132 d2 d2-dg d200 d200-dg d210 d210-dg d211 d211-7b d211-dg d214 d214-dg d215 d215-7b d215-dg d216+ \
         d216+25 d216+dg d216-dg d216-unix d216-unix-25 d216e+ d216e+dg d216e-dg d216e-unix d217-dg d217-unix \
         d217-unix-25 d220 d220-7b d220-dg d230 d230-dg d230c d230c-dg d400 d400-dg d410 d410-7b d410-7b-w d410-dg \
         d410-w d411 d411-7b d411-7b-w d411-dg d411-w d412+ d412+25 d412+dg d412+s d412+sr d412+w d412-dg d412-unix \
         d412-unix-25 d412-unix-s d412-unix-sr d412-unix-w d413-dg d413-unix d413-unix-25 d413-unix-s d413-unix-sr \
         d413-unix-w d414-unix d414-unix-25 d414-unix-s d414-unix-sr d414-unix-w d430-dg d430-dg-ccc d430-unix \
         d430-unix-25 d430-unix-25-ccc d430-unix-ccc d430-unix-s d430-unix-s-ccc d430-unix-sr d430-unix-sr-ccc \
         d430-unix-w d430-unix-w-ccc d430c-dg d430c-dg-ccc d430c-unix d430c-unix-25 d430c-unix-25-ccc d430c-unix-ccc \
         d430c-unix-s d430c-unix-s-ccc d430c-unix-sr d430c-unix-sr-ccc d430c-unix-w d430c-unix-w-ccc d450 d450-dg d460 \
         d460-7b d460-7b-w d460-dg d460-w d461 d461-7b d461-7b-w d461-dg d461-w d462+ d462+25 d462+dg d462+s d462+sr \
         d462+w d462-dg d462-unix d462-unix-25 d462-unix-s d462-unix-sr d462-unix-w d462e-dg d463-dg d463-unix \
         d463-unix-25 d463-unix-s d463-unix-sr d463-unix-w d464-unix d464-unix-25 d464-unix-s d464-unix-sr \
         d464-unix-w d470 d470-7b d470-dg d470c d470c-7b d470c-dg d555 d555-7b d555-7b-w d555-dg d555-w d577 \
         d577-7b d577-7b-w d577-dg d577-w d578 d578-7b d578-dg d80 d800 datagraphix datamedia2500 datapoint \
         dataspeed40 dd5000 ddr ddr3180 dec-vt100 dec-vt220 dec-vt330 dec-vt340 dec-vt400 decpro decwriter \
         delta dg+ccc dg+color dg+color8 dg+fixed dg-ansi dg-generic dg100 dg200 dg210 dg211 dg450 dg460-ansi \
         dg6053 dg6053-old dg605x dg6134 dgkeys+11 dgkeys+15 dgkeys+7b dgkeys+8b dgmode+color dgmode+color8 \
         dgunix+ccc dgunix+fixed diablo diablo-lm diablo1620 diablo1620-m8 diablo1640 diablo1640-lm diablo1640-m8 \
         diablo1720 diablo1730 diablo1740 diablo1740-lm diablo450 diablo630 dialogue dialogue80 digilog dku7003 \
         dku7003-dumb dku7102 dku7102-old dku7102-sna dku7103-sna dku7202 dm1520 dm1521 dm2500 dm3025 dm3045 dm80 \
         dm80w dmchat dmd dmd-24 dmd-34 dmd1 dmdt80 dmdt80w dmterm dp3360 dp8242 ds40 ds40-2 dt-100 dt-100w dt100 \
         dt100w dt110 dt80 dt80-sas dt80w dtc300s dtc382 dtterm dumb dw dw1 dw2 dw3 dw4 dwk dwk-vt \
         ecma+color ecma+sgr elks elks-ansi elks-glasstty elks-vt52 emots emu env230 envision230 ep40 ep4000 ep4080 \
         ep48 ergo4000 esprit esprit-am eterm ex155 excel62 excel62-rv excel62-w excel64 excel64-rv excel64-w exec80 \
         f100 f100-rv f110 f110-14 f110-14w f110-w f1720 f1720a f200 f200-w f200vi f200vi-w falco falco-p fenix fenixw \
         fixterm fortune fos fox freedom freedom-rv freedom100 freedom110 freedom200 \
         gator gator-52 gator-52t gator-t gigi glasstty gnome go-225 go140 go140w go225 graphos graphos-30 \
         gs5430 gs5430-22 gs5430-24 gs6300 gsi gt100 gt100a gt40 gt42 guru guru+rv guru+s guru+unk guru-24 guru-33 \
         guru-33-rv guru-33-s guru-44 guru-44-s guru-76 guru-76-lp guru-76-s guru-76-w guru-76-w-s guru-76-wm guru-lp \
         guru-nctxt guru-rv guru-s \
         h-100 h-100bw h100 h100bw h19 h19-a h19-b h19-bs h19-g h19-smul h19-u h19-us h19a h19g h19k h19kermit h19us \
         h29a-kc-bc h29a-kc-uc h29a-nkc-bc h29a-nkc-uc h80 ha8675 ha8686 hazel hds200 he80 heath heath-19 heath-ansi \
         heathkit heathkit-a hft hft-c hft-c-old hft-old hirez100 hirez100-w hmod1 hp hp+arrows hp+color hp+labels \
         hp+pfk+arrows hp+pfk+cr hp+pfk-cr hp+printer hp110 hp150 hp2 hp236 hp2382 hp2382a hp2392 hp2397 hp2397a hp2621 \
         hp2621-48 hp2621-a hp2621-ba hp2621-fl hp2621-k45 hp2621-nl hp2621-nt hp2621-wl hp2621A hp2621a hp2621a-a hp2621b \
         hp2621b-kx hp2621b-kx-p hp2621b-p hp2621k45 hp2621p hp2621p-a hp2622 hp2622a hp2623 hp2623a hp2624 hp2624-10p hp2624a \
         hp2624a-10p hp2624b hp2624b-10p hp2624b-10p-p hp2624b-4p hp2624b-4p-p hp2624b-p hp2626 hp2626-12 hp2626-12-s hp2626-12x40 \
         hp2626-ns hp2626-s hp2626-x40 hp2626a hp2626p hp2627a hp2627a-rev hp2627c hp262x hp2640a hp2640b hp2641a hp2644a \
         hp2645 hp2645a hp2647a hp2648 hp2648a hp300h hp45 hp700 hp700-wy hp70092 hp70092A hp70092a hp9837 hp9845 hp98550 \
         hp98550a hp98720 hp98721 hpansi hpex hpex2 hpgeneric hpsub hpterm htx11 hz1000 hz1420 hz1500 hz1510 hz1520 \
         hz1520-noesc hz1552 hz1552-rv hz2000 \
         i100 i3101 i3164 i400 ibcs2 ibm+color ibm-apl ibm-pc ibm-system1 ibm3101 ibm3151 ibm3161 ibm3161-C ibm3162 \
         ibm3163 ibm3164 ibm327x ibm5051 ibm5081 ibm5081-c ibm5151 ibm5154 ibm5154-c ibm6153 ibm6153-40 ibm6153-90 \
         ibm6154 ibm6154-c ibm6155 ibm8503 ibm8507 ibm8512 ibm8513 ibm8514 ibm8514-c ibm8604 ibmaed ibmapa16 ibmapa8 \
         ibmapa8c ibmapa8c-c ibmega ibmega-c ibmmono ibmmpel-c ibmpc ibmpc3 ibmpc3r ibmpc3r-mono ibmpcx ibmvga ibmvga-c \
         ibmx icl6402 icl6404 icl6404-w ifmr ims-ansi ims950 ims950-b ims950-rv infoton intertec intertube intertube2 \
         intext intext2 intextii ips ipsi iq120 iq140 iris-ansi iris-ansi-ap iris-ansi-net iris-color iris40 \
         jaixterm jaixterm-m jerq \
         k45 kaypro kaypro2 kds6402 kds7372 kds7372-w kermit kermit-am klone+acs klone+color klone+koi8acs klone+sgr \
         klone+sgr-dumb kt7 kt7ix kterm kterm-co kterm-color ktm kvt \
         la120 layer lft lft-pc850 linux linux-c linux-c-nc linux-koi8 linux-koi8r linux-lat linux-m linux-nic lisa \
         lisaterm lisaterm-w liswb ln03 ln03-w lpr luna luna68k \
         m2-nam mac mac-w mach mach-bold mach-color macintosh macterminal-w mai masscomp masscomp1 masscomp2 mdl110 \
         megatek memhp mgr mgr-linux mgr-sun mgterm microb microbee microterm microterm5 mime mime-3ax mime-fb mime-hb \
         mime1 mime2 mime2a mime2a-s mime2a-v mime314 mime340 mime3a mime3ax mimei mimeii minitel minitel-2 minitel-2-nam \
         minitel1 minitel1b minitel1b-80 minix minix-old minix-old-am mm314 mm340 mod mod24 modgraph modgraph2 modgraph48 \
         mono-emx ms-vt100 msk227 msk22714 msk227am mskermit227 mskermit22714 mskermit227am mt-70 mt4520-rv mt70 \
         nansi.sys nansi.sysk nansisys nansisysk ncr160vppp ncr160vpwpp ncr160vt100an ncr160vt100pp ncr160vt100wan \
         ncr160vt100wpp ncr160vt200an ncr160vt200pp ncr160vt200wan ncr160vt200wpp ncr160vt300an ncr160vt300pp ncr160vt300wan \
         ncr160vt300wpp ncr160wy50+pp ncr160wy50+wpp ncr160wy60pp ncr160wy60wpp ncr260intan ncr260intpp ncr260intwan ncr260intwpp \
         ncr260vppp ncr260vpwpp ncr260vt100an ncr260vt100pp ncr260vt100wan ncr260vt100wpp ncr260vt200an ncr260vt200pp ncr260vt200wan \
         ncr260vt200wpp ncr260vt300an ncr260vt300pp ncr260vt300wan ncr260vt300wpp ncr260wy325pp ncr260wy325wpp ncr260wy350pp \
         ncr260wy350wpp ncr260wy50+pp ncr260wy50+wpp ncr260wy60pp ncr260wy60wpp ncr7900 ncr7900i ncr7900iv ncr7901 ncrvt100an \
         ncrvt100pp ncrvt100wan ncrvt100wpp ncsa ncsa-m ncsa-m-ns ncsa-ns ncsa-vt220 ncsa-vt220-8 nec nec5520 newhp newhpkeyboard \
         news news-29 news-29-euc news-29-sjis news-33 news-33-euc news-33-sjis news-42 news-42-euc news-42-sjis news-a news-o \
         news-old-unk news-unk news28 news28-a news29 news31 news31-a news31-o news33 news40 news40-a news40-o news42 newscbm \
         newscbm-a newscbm-o newscbm33 next nextshell northstar nwe501 nwe501-a nwe501-o nwp-511 nwp-517 nwp-517-w nwp251-a \
         nwp251-o nwp511 nwp512 nwp512-a nwp512-o nwp513 nwp513-a nwp513-o nwp514 nwp514-a nwp514-o nwp517 nwp517-w nwp518 \
         nwp518-a nwp518-o nxterm \
         o31 o4112-nd o85h oabm85h oblit oc100 oconcept ofcons ojerq oldibmpc3 oldpc3 oldsun omron opus3n1+ origibmpc3 origpc3 \
         os9LII osborne osborne-w osborne1 osborne1-w osexec otek4112 otek4113 otek4114 otek4115 owl \
         p12 p12-m p12-m-w p12-w p14 p14-m p14-m-w p14-w p19 p4 p5 p7 p8 p8-w p8gl p9 p9-8 p9-8-w p9-w pc-coherent pc-minix \
         pc-venix pc3 pc3-bold pc3r pc3r-m pc6300plus pc7300 pcansi pcansi-25 pcansi-25-m pcansi-33 pcansi-33-m pcansi-43 \
         pcansi-43-m pcansi-m pcansi-mono pcansi25 pcansi25m pcansi33 pcansi33m pcansi43 pccons pcconsole pcix pckermit \
         pckermit12 pckermit120 pcmw pcplot pcvt25 pcvt25w pcvt28 pcvt28w pcvt35 pcvt35w pcvt40 pcvt40w pcvt43 pcvt43w pcvt50 \
         pcvt50w pcvtXX pcz19 pe1100 pe1200 pe1251 pe550 pe6100 pe6300 pe6312 pe7000c pe7000m pilot pmcons pmconsole printer \
         prism12 prism12-m prism12-m-w prism12-w prism14 prism14-m prism14-m-w prism14-w prism2 prism4 prism5 prism7 prism8 \
         prism8-w prism8gl prism9 prism9-8 prism9-8-w prism9-w pro350 ps300 psterm psterm-80x24 psterm-90x28 psterm-96x48 \
         psterm-basic psterm-fast psx_ansi pt100 pt100w pt200 pt200w pt210 pt250 pt250w pt505 pt505-22 pt505-24 pty \
         qansi qansi-g qansi-m qansi-t qansi-w qdcons qdss qnx qnx4 qnxm qnxt qnxt2 qnxt4 qnxtmono qnxw qume qume5 qvt101 \
         qvt101+ qvt101p qvt102 qvt103 qvt103-w qvt108 qvt119 qvt119+ qvt119+-25 qvt119+-25-w qvt119+-w qvt119-25-w qvt119-w \
         qvt119p qvt119p-25 qvt119p-25-w qvt119p-w qvt203 qvt203+ qvt203-25 qvt203-25-w qvt203-w qvt203-w-am \
         rbcomm rbcomm-nam rbcomm-w rca rcons rcons-color rebus3180 regent regent100 regent20 regent200 regent25 regent40 \
         regent40+ regent60 rt6221 rt6221-w rtpc rxvt rxvt-basic \
         s4 sb1 sb2 sb3 sbi sbobcat sc410 sc415 scanset scoansi screen screen-w screen.teraterm screen.xterm-r6 screen.xterm-xfree86 \
         screen2 screen3 screwpoint scrhp sibo simterm soroc soroc120 soroc140 spinwriter st52 sun sun-1 sun-12 sun-17 sun-24 sun-34 \
         sun-48 sun-c sun-cgsix sun-cmd sun-e sun-e-s sun-il sun-nic sun-s sun-s-e sun-ss5 sun1 sun2 sune superbee superbee-xsb \
         superbeeic superbrain sv80 swtp synertek synertek380 system1 \
         t10 t1061 t1061f t16 t3700 t3800 t653x tab tab132 tab132-15 tab132-rv tab132-w tab132-w-rv tandem6510 tandem653 tek \
         tek4012 tek4013 tek4014 tek4014-sm tek4015 tek4015-sm tek4023 tek4024 tek4025 tek4025-17 tek4025-17-ws tek4025-cr \
         tek4025-ex tek4025a tek4025ex tek4027 tek4027-ex tek4105 tek4105-30 tek4105a tek4106brl tek4107 tek4107brl tek4109 \
         tek4109brl tek4112 tek4112-5 tek4112-nd tek4113 tek4113-34 tek4113-nd tek4114 tek4115 tek4125 tek4205 tek4207 \
         tek4207-s tek4404 teleray teletec teraterm terminet terminet1200 terminet300 tgtelnet ti700 ti733 ti735 ti745 ti800 \
         ti916 ti916-132 ti916-220-7 ti916-220-8 ti916-8 ti916-8-132 ti924 ti924-8 ti924-8w ti924w ti926 ti926-8 ti928 ti928-8 \
         ti931 ti_ansi tn1200 tn300 trs16 trs2 trs80II trsII ts-1 ts-1p ts1 ts100 ts100-ctxt ts100-sp ts1p tt505-22 tty33 tty35 \
         tty37 tty40 tty43 tty4420 tty4424 tty4424-1 tty4424m tty4426 tty5410 tty5410-w tty5410v1 tty5410v1-w tty5420 tty5420+nl \
         tty5420-nl tty5420-rv tty5420-rv-nl tty5420-w tty5420-w-nl tty5420-w-rv tty5420-w-rv-n tty5425 tty5425-nl tty5425-w \
         tty5620 tty5620-1 tty5620-24 tty5620-34 tty5620-s ttydmd tvi-2p tvi803 tvi9065 tvi910 tvi910+ tvi912 tvi912-2p tvi912b \
         tvi912c tvi912cc tvi914 tvi920 tvi920-2p tvi920b tvi920c tvi921 tvi924 tvi925 tvi925-hi tvi92B tvi92D tvi950 tvi950-2p \
         tvi950-4p tvi950-rv tvi950-rv-2p tvi950-rv-4p tvi955 tvi955-hb tvi955-w tvi970 tvi970-2p tvi970-vb tvipt tws-generic \
         tws2102-sna tws2103 tws2103-sna \
         ultima2 ultimaII uniterm uniterm49 unixpc unknown uts30 \
         v200-nam v320n v3220 v5410 vanilla vapple vc103 vc203 vc303 vc303a vc403a vc404 vc404-s vc414 vc414h vc415 venix versaterm \
         vi200 vi200-f vi200-rv vi300 vi300-old vi50 vi500 vi50adm vi55 vi550 vi603 viewpoint viewpoint3a+ viewpoint60 viewpoint90 \
         vip vip-H vip-Hw vip-w vip7800-H vip7800-Hw vip7800-w visa50 visual603 vitty vk100 vp3a+ vp60 vp90 vremote vs100 vs100-x10 \
         vsc vt-61 vt100 vt100-am vt100-bm vt100-bm-o vt100-bot-s vt100-nam vt100-nam-w vt100-nav vt100-nav-w vt100-s vt100-s-bot \
         vt100-s-top vt100-top-s vt100-vb vt100-w vt100-w-am vt100-w-nam vt100-w-nav vt100nam vt102 vt102-nsgr vt102-w vt125 vt131 \
         vt132 vt200 vt200-8 vt200-8bit vt200-js vt200-old vt200-w vt220 vt220-8 vt220-8bit vt220-js vt220-nam vt220-old vt220-w \
         vt220d vt300 vt300-nam vt300-w vt300-w-nam vt320 vt320-k3 vt320-k311 vt320-nam vt320-w vt320-w-nam vt320nam vt330 vt340 \
         vt400 vt400-24 vt420 vt420f vt420pc vt420pcdos vt50 vt50h vt510 vt510pc vt510pcdos vt52 vt520 vt525 vt61 vt61.5 \
         wren wrenw wsiris wsvt25 wsvt25m wy-75ap wy-99fgt wy-99fgta wy100 wy100q wy120 wy120-25 wy120-25-w wy120-vb wy120-w \
         wy120-w-vb wy120-wvb wy150 wy150-25 wy150-25-w wy150-vb wy150-w wy150-w-vb wy160 wy160-25 wy160-25-w wy160-42 wy160-42-w \
         wy160-43 wy160-43-w wy160-tek wy160-vb wy160-w wy160-w-vb wy160-wvb wy185 wy185-24 wy185-vb wy185-w wy185-wvb wy30 wy30-mc \
         wy30-vb wy325 wy325-25 wy325-25w wy325-42 wy325-42w wy325-42w-vb wy325-42wvb wy325-43 wy325-43w wy325-43w-vb wy325-43wvb \
         wy325-80 wy325-vb wy325-w wy325-w-vb wy325-wvb wy325w-24 wy350 wy350-vb wy350-w wy350-wvb wy370 wy370-101k wy370-105k \
         wy370-EPC wy370-nk wy370-rv wy370-tek wy370-vb wy370-w wy370-wvb wy50 wy50-mc wy50-vb wy50-w wy50-wvb wy520 wy520-24 \
         wy520-36 wy520-36pc wy520-36w wy520-36wpc wy520-48 wy520-48pc wy520-48w wy520-48wpc wy520-epc wy520-epc-24 wy520-epc-vb \
         wy520-epc-w wy520-epc-wvb wy520-vb wy520-w wy520-wvb wy60 wy60-25 wy60-25-w wy60-316X wy60-42 wy60-42-w wy60-43 wy60-43-w \
         wy60-AT wy60-PC wy60-vb wy60-w wy60-w-vb wy60-wvb wy75 wy75-mc wy75-vb wy75-w wy75-wvb wy75ap wy85 wy85-8bit wy85-vb wy85-w \
         wy85-wvb wy99-ansi wy99a-ansi wy99f wy99fa wy99fgt wy99fgta wy99gt wy99gt-25 wy99gt-25-w wy99gt-tek wy99gt-vb wy99gt-w wy99gt-w-vb \
         wy99gt-wvb wyse-325 wyse-75ap wyse-vp wyse120 wyse120-25 wyse120-25-w wyse120-vb wyse120-w wyse120-wvb wyse150 wyse150-25 \
         wyse150-25-w wyse150-vb wyse150-w wyse150-w-vb wyse160 wyse160-25 wyse160-25-w wyse160-42 wyse160-42-w wyse160-43 wyse160-43-w \
         wyse160-vb wyse160-w wyse160-wvb wyse185 wyse185-24 wyse185-vb wyse185-w wyse185-wvb wyse30 wyse30-mc wyse30-vb wyse325 \
         wyse325-25 wyse325-25w wyse325-42 wyse325-42w wyse325-43 wyse325-43w wyse325-vb wyse325-w wyse325-wvb wyse350 wyse350-vb \
         wyse350-w wyse350-wvb wyse370 wyse50 wyse50-mc wyse50-vb wyse50-w wyse50-wvb wyse520 wyse520-24 wyse520-36 wyse520-36pc \
         wyse520-36w wyse520-36wpc wyse520-48 wyse520-48pc wyse520-48w wyse520-48wpc wyse520-epc wyse520-epc-w wyse520-p-wvb \
         wyse520-pc-24 wyse520-pc-vb wyse520-vb wyse520-w wyse520-wvb wyse60 wyse60-25 wyse60-25-w wyse60-316X wyse60-42 \
         wyse60-42-w wyse60-43 wyse60-43-w wyse60-AT wyse60-PC wyse60-vb wyse60-w wyse60-wvb wyse75 wyse75-mc wyse75-vb wyse75-w \
         wyse75-wvb wyse75ap wyse85 wyse85-8bit wyse85-vb wyse85-w wyse85-wvb wyse99gt wyse99gt-25 wyse99gt-25-w wyse99gt-vb \
         wyse99gt-w wyse99gt-wvb \
         x10term x1700 x1700-lm x1720 x1750 x68k x68k-ite x820 xdku xenix xerox xerox-lm xerox1720 xerox820 xl83 xtalk xterm \
         xterm+sl xterm+sl-twm xterm-16color xterm-24 xterm-256color xterm-88color xterm-8bit xterm-basic xterm-bold xterm-color \
         xterm-hp xterm-new xterm-nic xterm-noapp xterm-old xterm-pcolor xterm-r5 xterm-r6 xterm-sco xterm-sun xterm-vt220 \
         xterm-vt52 xterm-xf86-v32 xterm-xf86-v33 xterm-xf86-v333 xterm-xf86-v40 xterm-xfree86 xterm-xi xterm1 xtermc xtermm \
         xterms xterms-sun xwsh \
         z-100 z-100bw z100 z100bw z110 z110bw z19 z29 z29a z29a-kc-bc z29a-kc-uc z29a-nkc-bc z29a-nkc-uc z29b z30 z340 z340-nam \
         z39-a z39a z50 z8001 zen30 zen50 zen8001 zenith zenith29 zenith39-a zenith39-ansi zt-1 ztx ztx-1-a ztx11"     
         
#  We want to remove the terminals that are not part of $TERMS}

   allterms="$(find "${D}${datadir}/terminfo/" -type f -name "*" | sed -e :a -e '$b;N;s/\n/ /;ba' | sed -e 's/\. //g')"
   
   for x in ${TERMS}
   do
     terms_to_remove=`echo "${allterms}" | sed -e "s/[^ ]*${x} / /g"`
     allterms=${terms_to_remove}
   done
   
   for x in ${terms_to_remove}
   do
     rm ${x}
   done
   

	mv ${D}${bindir}/clear ${D}${bindir}/clear.${PN}
	mv ${D}${bindir}/reset ${D}${bindir}/reset.${PN}

    for lib in ${D}/${libdir}/lib*.so.*
    do
        test -L $lib && continue
        chmod a+x $lib
    done
}
		
		
pkg_postinst_ncurses-tools () {
	update-alternatives --install ${bindir}/clear clear clear.${PN} 100
	update-alternatives --install ${bindir}/reset reset reset.${PN} 100
}
				
			
pkg_prerm_ncurses-tools () {
	update-alternatives --remove clear clear.${PN}
	update-alternatives --remove reset reset.${PN}
}

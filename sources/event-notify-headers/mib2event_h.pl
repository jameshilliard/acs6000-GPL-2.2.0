#!/usr/bin/perl

# parsing MIB provided last on the commandline
# to produce header files for the event notification.

# hopefully avoid some of the hassle in translation.
# its ugly but its perl.

use File::stat;

my $in_file = $ARGV[0];
my $appliance_file = $ARGV[1];

my $event_prefix = "";
my $event_name_prefix = "";
my $mib_name;
if ($in_file =~ /.*\/(.*$)/) {
    $mib_name = $1;
} else
{
    $mib_name = "";
}
my $sb = stat($in_file);
my $mib_mod_time = scalar localtime $sb->mtime;
# print $mib_name . " Mib last Mod: " . $mib_mod_time. "\n";
$itisnow = scalar localtime;


open(MIB_IN, "$in_file") or die "Can't open the $in_file: $!";

my %objects;
my %sortable_objects;
my $trap_count = 0;

my %traps;
my %sorted_traps;
my %no_notify_conf_events;
my $expected;
my $current_parse;
my $trap_name;
my $obj_name;
my @productname;
my $familyoid;
my $trapmgoid;
my $productoid;
my $managmentoid;
my $needDuplicateTargetEvents = 0;
my $Id_targetEvents ;

while (<MIB_IN>)
{
    next if /^$/;

    if (/-- Familyoid (.*)/){
	
        $familyoid= $1;
        chop($familyoid);
        next;
	}
    if (/-- Productoid (.*)/){
        $productoid= $1;
        chop($productoid);
        next;
    }
    if (/-- Managementoid (.*)/){
        $managmentoid= $1;
        chop($managmentoid);
        next;
	}
    if (/-- TrapObjectoid (.*)/){
        $trapmgoid= $1;
        chop($trapmgoid);
        next;
    }

    if (/::=.* (\d\d*)/) {
        my $oid = $1;
#    	print $current_parse . ": " .$trap_name .": ". $obj_name . ": " . $oid . "\n";

        if ($current_parse =~ /object/)
        {
            $objects{$obj_name}{'oid'} = $oid;
            $sortable_objects{$oid} = $objects{$obj_name};
            # get the number from this line to  ...
            #print "Object: ". $objects{$obj_name}{'name'} . " " . $objects{$obj_name}{'oid'} . " " .$objects{$obj_name}{'syntax'} . "\n";
            $obj_name = ''; # got to the end of a record
            $current_parse = '';
        }
        if ($current_parse =~ /trap/)
        {
            $traps{$trap_name}{'oid'} = $oid;
            $sorted_traps{$oid} = $traps{$trap_name};
#    	    $sorted_traps{$oid}{'name'} = $traps{$trap_name}{'name'};
#    	    print "Trap: ". $objects{$obj_name}{'name'} . " " . $objects{$obj_name}{'oid'} . " " .$objects{$obj_name}{'syntax'} . "\n";
#    	    print "Trap: ". $traps{$trap_name}{'name'} . " " . $traps{$trap_name}{'oid'} . "\n";


            $trap_name = '';
            $current_parse = '';
        }
        next;
    }


    if (/(^\b.*\b) OBJECT-TYPE/){ # got a object record
        $obj_name = $1;
        $current_parse = "object";
        $objects{$obj_name}{'name'} = $obj_name;
#    	print $current_parse ." : ".$obj_name ."\n";
        next;
    }
    if ($obj_name && /SYNTAX (.*)/) { # build the data structure for the objects.
        my $expected_type = "TYPE_STRING" if ($1 =~ /STRING/);
        $expected_type = "TYPE_NUMBER" if ($1 =~ /INTEGER/);
        $objects{$obj_name}{'syntax'} = $expected_type;
#    	print $current_parse ." : ".$obj_name .": " .$objects{$obj_name}{'syntax'} ."\n";
    }

    if (/(^\b.*\b) TRAP-TYPE/){
        $trap_name = $1;
        $trap_count++;
        $current_parse = "trap";
        $traps{$trap_name}{'name'} = $trap_name;
#       print $current_parse . ": " . $traps{$trap_name}{'name'}. "\n";
        $inTRap = 1;
        next;
    }

#    my $num_vars;
    if (/ENTERPRISE/) {
        $expected = "VARIABLES";
        $num_vars = 0;
        next;
    }

    if (/VARIABLES/ && $expected == "VARIABLES") {
        $expected = "VARIABLES";
        next;
    }
# can be 0 to many
# always on a seperate line -- this will need to be corrected at some point --
# or raise an error
#
    if ($expected =~ /VARIABLES/ && /}/ || /\bDESCRIPTION\b/) {
        $expected = "SUMMARY";
        $traps{$trap_name}{'num_vars'} = $num_vars;
#    	print $current_parse . ": " . $traps{$trap_name}{'name'}.": " . $traps{$trap_name}{'num_vars'}. "\n";
        next;
    }
    if ($expected =~ /VARIABLES/ && /\b(.*)\b[,]*/) {
        # we're getting variables
        my $var = $1;
        $traps{$trap_name}{'variables'}[$num_vars] = $var;
#    	print $current_parse . ": " . $traps{$trap_name}{'name'}."- " . $num_vars;
#    	print "->".$traps{$trap_name}{'variables'}[$num_vars]. "\n";
        $num_vars++;
        next;
    }


    if (/SUMMARY[^\"]*\"(.*)\"/)
    {
        $traps{$trap_name}{'summary'} = $1;
#       print $current_parse .": " . $traps{$trap_name}{'name'}.": " . $traps{$trap_name}{'summary'} . "\n";
        next;
    }
    if ($inTRap && /ARGUMENTS (.*)$/) {
        $traps{$trap_name}{'args_list'} = $1;
        chop($traps{$trap_name}{'args_list'});
#       print $current_parse .": " . $traps{$trap_name}{'name'}.": " . $traps{$trap_name}{'args_list'} . "\n";
        next;
    }
    if ($inTRap && /SEVERITY (.*)$/) {
        $traps{$trap_name}{'severity'} = $1;
        chop($traps{$trap_name}{'severity'});
#       print $current_parse .": " . $traps{$trap_name}{'name'}.": " . $traps{$trap_name}{'severity'} . "\n";
        next;
    }
    if (/NAME[^\"]*\"(.*)\"/)
    {
        $traps{$trap_name}{'evtname'} = $1;
#        print $current_parse .": " . $traps{$trap_name}{'name'}.": " . $traps{$trap_name}{'evtname'} . "\n";
        next;
    }
    if (/SHORT[^\"]*\"(.*)\"/)
    {
        $traps{$trap_name}{'short'} = $1;
#       print $current_parse .": " . $traps{$trap_name}{'short'}.": " . $traps{$trap_name}{'summary'} . "\n";
        next;
    }

    if (/EVENT(.*)$/)
    {
        $traps{$trap_name}{'event'} = $1;
        chop($traps{$trap_name}{'event'});
#       print $traps{$trap_name}{'event'}."\n";
        next;
    }


## collect from MIB in the trap section:
# for event_snmptrap[] : foreach evt.
# {$num_args, {$list_args_object_name }, {$list_args_type}},
#
# for event_syslog[] : foreach evt:
# {$sys_log_message_summary, $arguements }
# from --#summary, and --#arugments


}

close(MIB_IN);


######################################################################################
# Read in the configuration file specfiic for this appliance
#
#

open(APPLIANCE, "$appliance_file") or die "Can't open $appliance_file: $!";

#Read appliance config file
while (<APPLIANCE>)
{
    next if /^$/;
    #get model name for events prefix from appliance file
    if(/-- Model (.*)/)
    {
        $productname[0] = $1 ;
        next;
    }
    if( /-- NeedDuplicateTargetEvents (.*)/){
        $needDuplicateTargetEvents = 1 ;
        $Id_targetEvents = $1;
        chop($Id_targetEvents);
        $Id_targetEvents =~ s/\s+$//;
        $Id_targetEvents =~ s/^\s+//;
        next;
    }
    if (/-- EventPrefix (.*)/)
    {
        $event_prefix = $1 ;
        next;
    }
    if (/-- EventNamePrefix (.*)/)
    {
        $event_name_prefix = $1 ;
        next;
    }


    if (/-- BeginTraps/)
    {
        while (<APPLIANCE>)
        {
            if (/(.*)(\s+)--(.*)/)
            {
                push(@usedtraps, $1);
                my $flag = $3;
                $flag =~ s/\s+$//;
                $flag =~ s/^\s+//;
                if ($flag eq "no_notify_conf")
                {
                    $no_notify_conf_events{$1} = 1;
                }
            }
            else
            {
                push(@usedtraps, $_);
            }
        }
#      @usedtraps = <APPLIANCE>;
    }

}
chomp(@usedtraps);
close(APPLIANCE);

#use Data::Dumper;
#print __FILE__ . ":usedtraps - " . Dumper(\@usedtraps) . "\n";
#print __FILE__ . ":no_notify_conf_events - " . Dumper(\%no_notify_conf_events) . "\n";

# print "end process\nshow results:\n";
##############################################################################
# outputting files:
# trap_oids.h

my $file_name = __FILE__;

my $out_mod_comment =<<"OMC";
#if 0
# /* this file created by $file_name: $itisnow
#  * from MIB: $mib_name,
#  *   MIB modified: $mib_mod_time
#  **/
#endif
OMC

$trap_oid =<<"THD";
/* defines the OIDs */
$out_mod_comment

#define avMgmt		".1.3.6.1.4.1.10418.$familyoid"
#define avTrapObject	"$managmentoid.$trapmgoid"
#define avProduct	$productoid
#define avManagement	$managmentoid

THD

my $final_trap;
sub numerically { $a <=> $b }
$trap_oid .= "\n/* Enterprise Traps */\n";
for $tk (sort numerically keys %sorted_traps) {
    $trap_oid .= "#define $sorted_traps{$tk}{'name'}\t\t$sorted_traps{$tk}{'oid'}\n";
    $final_trap = $tk;
}

$event_notif_conf =<<"ENCH";
# Configure the event notification daemon
# if hand editing no spaces between CONF=value

$out_mod_comment
[totalevent]
range=$final_trap
# configure the SNMPTrap destination
# IP address of the destination of the SNMPTrap messages ( up to 5)
[snmptrap]
destination0=
destination1=
destination2=
destination3=
destination4=
trapCommunity=public
# community name used in the trap message (by default it is public)
# configure the syslog message facility LOG_LOCAL0 to LOG_LOCAL7
[syslog]
facility=LOG_LOCAL0

[email]
emailto=
smtpserver_ip_add=
emailport=25

[sms]
smsto=
sms_gateway_ip=
smsport=6701

# generate status : disable (0 - default value)
#                   send SNMP trap message (1)
#                   send syslog message (2) default
#                   send SNMP trap and syslog message (3)
#                       not available (-1)
[event]
ENCH

$evt_connect_h =<<"EVCNH";
$out_mod_comment
/*
 *
 writeevt generates a event notification message, which
  will be written to the fifo device.
  Parameters :
	. event_number : number of the event
	. format : type of the parameters : 'i' = interger
                                            's' = string
					    'u' = username
                   Ex. "iss" : interger, string, string
        . va_list : list of the parameters for the evnet
****************************************************************/

void writeevt(int evtn, char * format,...);

EVCNH

$evt_msg_head =<<"MSGH";
$out_mod_comment

/* the max size of syslog message
 * message that exceeds this size will be truncated
 */
#define MAX_SYSLOG_MSG	1024

typedef enum {
	TYPE_NUMBER,
	TYPE_STRING
} VARIABLE_TYPE;

MSGH

$evt_msg_internal =<<"EVT_INTH";
/* link : internal event number (sent by applications) and
 * its correspondent TRAP number and Syslog message **/
int event_internal[] = {
EVT_INTH

$evt_msg_snmptrap =<<"EMSTH";
#define MAX_ARGUMENTS	6
struct EVENT_SNMPTrapOID {
	int variables_len;
	int variables[MAX_ARGUMENTS];
	VARIABLE_TYPE variable_type[MAX_ARGUMENTS];
} event_snmptrap[] = {
EMSTH

$evt_msg_syslog =<<"EMSYSLH";
struct EVENT_SYSLOG {
	char summary[1024];
	unsigned char	arguments[MAX_ARGUMENTS];
} event_syslog[] = {
EMSYSLH

$evt_prop_file=<<"EPROPF";
EPROPF

$evt_def_file=<<"EDEFFILE";
/* defines the events */
$out_mod_comment

EDEFFILE

$evt_shl_file=<<"ESHLFILE";
ESHLFILE

$evt_arg_file=<<"EARGFILE";
EARGFILE

######################################################################
# file the file strings

#sub numerically { $a <=> $b }

#my $final_trap;

#$trap_oid .= "\n/* Enterprise Traps */\n";
#for $tk (sort numerically keys %sorted_traps) {
#    $trap_oid .= "#define $sorted_traps{$tk}{'name'}\t\t$sorted_traps{$tk}{'oid'}\n";
#    $final_trap = $tk;
#}
#print $final_trap. ": " . $trap_count . " \n";

$trap_oid .= "\n/* Trap objects */\n";
for $ok (sort numerically keys %sortable_objects) {
    $trap_oid .= "#define $sortable_objects{$ok}{'name'}\t\t$sortable_objects{$ok}{'oid'}\n";
}
$evt_msg_head .= "#define MAX_EVENT_NUMBER $final_trap\n";
$evt_msg_head .= "#define NUM_ENTERPRISE_TRAPS	$final_trap\n\n";
$evt_duplicate_xml .="<duplicateEvents>\n";

## spin through the traps.
for $evt (1..$final_trap) {
    if ($sorted_traps{$evt}) {
        # got a trap with this number:

        $evtval = 0;
        $sorted_event = $sorted_traps{$evt}{'event'};
        for ($sorted_event) {
            s/^\s+//;
            s/\s+$//;
        }
        foreach (@usedtraps)
        {
            split(/ /, $_);
            if (@_[0] eq $sorted_event) {
                if (@_[1] eq 'trap') {
                    $evtval = 1;
                }
                if (@_[1] eq 'syslog') {
                    $evtval = 2;
                }
                last;
            }
        }

        if ($no_notify_conf_events{$sorted_event})
        {
            $event_notif_conf .= "$evt=-1\n";
        }
        else
        {
            $event_notif_conf .= "$evt=$evtval\n";
        }
        $evt_connect_h .= "#define EVT$evt\t\t$evt\n";
        $evt_msg_internal .= "\t$evt, /* $sorted_traps{$evt}{'name'} */\n";

        # do the  snmp messages.
        $evt_msg_snmptrap .= " /* EVT-$evt: $sorted_traps{$evt}{'name'} */\n";
        $evt_msg_snmptrap .= " { $sorted_traps{$evt}{'num_vars'}, { ";

        my $arg_types;
        # spin through the arguments
        for $c ( 0 .. ( $sorted_traps{$evt}{'num_vars'} - 1 ) ){
            my $argn = $sorted_traps{$evt}{'variables'}[$c];
            $evt_msg_snmptrap .= "$argn, ";
            $arg_types .= "$objects{$argn}{'syntax'}, ";
        }

        $evt_msg_snmptrap .= "}, { $arg_types } },\n";

        # do the syslog messages
        $evt_msg_syslog .= " /* EVT-$evt: $sorted_traps{$evt}{'name'} */\n";
#    	$evt_msg_syslog .= " {\"$sorted_traps{$evt}{'summary'}\", $sorted_traps{$evt}{'args_list'} },\n";
        $evt_msg_syslog .= "{\"\", $sorted_traps{$evt}{'args_list'} },\n";
        $evt_prop_file .= "Event$evt.name=$productname[0] $sorted_traps{$evt}{'evtname'}\n";
        $evt_prop_file .= "Event$evt.short=$productname[0] $sorted_traps{$evt}{'short'}\n";
        $sorted_traps{$evt}{'summary'} =~ s/\%\D/\%s/g;
        $evt_prop_file .= "Event$evt.long=$sorted_traps{$evt}{'summary'}\n";
        $evt_def_file .= "#define\t$sorted_traps{$evt}{'event'}\t$evt\n";
        $evt_shl_file .= "$sorted_traps{$evt}{'event'}=$evt\n";
        $evt_arg_file .= "Event$evt.name=$productname[0] $sorted_traps{$evt}{'evtname'}\n";
        $evt_arg_file .= "Event$evt.short=$productname[0] $sorted_traps{$evt}{'short'}\n";
        $evt_arg_file .= "Event$evt.long=";
        my@new =split(/\%\D/,$sorted_traps{$evt}{'summary'});
        my $i=0;
        my $evt_long_name ="";
        $size= scalar(@new);
        if($size==1) {
            $evt_arg_file .= "$new[0]\n";
            $evt_long_name .= "$new[0]\n";
        }
        else{
            $size=$size-1;
            for($i = 0; $i < $size ; $i++) {
                 $evt_arg_file .= "$new[$i]";
                 $evt_arg_file .= "{$i}";
                 $evt_long_name .= "$new[$i]";
                 $evt_long_name .= "{$i}";

            }

            $evt_arg_file .= ".\n";
            $evt_long_name .= ".\n";

        }
        my $m_args ="";
        for $i (0..($sorted_traps{$evt}{"num_vars"} -1)) {
            $m_args .= $sorted_traps{$evt}{'variables'}[$i].",";
        }
        #print "$evt ".$m_args."\n";

        my $evt_name = "Event$evt";
        my $evt_severity="";
        $evt_severity  = $sorted_traps{$evt}{'severity'};
        $evt_severity  =~ s/^\s+//;
        $evt_severity  =~ s/\s+$//;
        $evt_severity  =~ s/NON-CRITICAL/NONCRITICAL/;
        $evt_severity  =~ s/INFORMATIONAL/INFORMATION/;
        $evt_severity  =~ s/MINOR/INFORMATION/;
        $evt_severity  =~ s/ALERT/INFORMATION/;
        $evt_severity  = ucfirst(lc($evt_severity));
        $evt_severity  = "avct".$evt_severity;
        $evt_duplicate_xml .= "\t<element elementId=\"$event_name_prefix$evt_name\" classification=\"avctEventId\" scope=\"avctDomain\"\n";
        $evt_duplicate_xml .= "\t\ttype=\"$event_prefix$evt_name\" label=\"$evt_name\"\n";
        $evt_duplicate_xml .= "\t\tseverity=\"$evt_severity\">\n";
        $evt_duplicate_xml .= "\t\t<relationships>\n";
        $evt_duplicate_xml .= "\t\t\t<relationship classification=\"child\" type=\"avctTraps\" scope=\"avctSystem\"/>\n";
        $evt_duplicate_xml .= "\t\t</relationships>\n";
        $evt_duplicate_xml .= "\t</element>\n" ;
        if($needDuplicateTargetEvents){
            if($Id_targetEvents){
                if( $m_args  =~ /$Id_targetEvents/)
                {
                    my $myline;
                    $myline = "Event$evt.unit.name=$productname[0] $sorted_traps{$evt}{'evtname'}\n";
                    $myline =~ s/Appliance//i;
                    $evt_arg_file .= $myline;
                    $myline = "Event$evt.unit.short=$productname[0] $sorted_traps{$evt}{'short'}\n";
                    $myline =~ s/Appliance//i;
                    $evt_arg_file .= $myline;
                    $myline = "Event$evt.unit.long=$evt_long_name";
                    $evt_arg_file .= $myline;
                    $evt_name = "Event$evt.unit";
                    $evt_duplicate_xml .= "\t<element elementId=\"$evt_name\" classification=\"avctEventId\" scope=\"avctDomain\"\n";
                    $evt_duplicate_xml .= "\t\ttype=\"$evt_name\" label=\"$evt_name\"\n";
                    $evt_duplicate_xml .= "\t\tseverity=\"$evt_severity\">\n";
                    $evt_duplicate_xml .= "\t\t<relationships>\n";
                    $evt_duplicate_xml .= "\t\t\t<relationship classification=\"child\" type=\"avctUnits\" scope=\"avctSystem\"/>\n";
                    $evt_duplicate_xml .= "\t\t</relationships>\n";
                    $evt_duplicate_xml .= "\t</element>\n" ;

                }
            }
        }
    }
    else {
        $event_notif_conf .= "$evt=-1\n";
        $evt_msg_internal .= "\t0, /* placeholder -- EVT$evt not used */\n";
        $evt_msg_snmptrap .= " /* EVT-$evt not used */\n { 0, { 0} },\n";
        $evt_msg_syslog .= " /* EVT-$evt not used */\n { \"\", { 0,} },\n";
        $evt_prop_file .= "Event$evt.name= not used \nEvent$evt.short= not used \nEvent$evt.long= not used \n";
    }

}

# finish the strings
$evt_msg_internal .= "}; /* end event_internal[] */\n";
$evt_msg_snmptrap .= "}; /* end event_snmptrap[] */\n";
$evt_msg_syslog .=   "}; /* end event_syslog[]   */\n";

$evt_duplicate_xml .="</duplicateEvents>\n";



#print $evt_msg_head . "\n";
#print $evt_msg_internal. "\n";
#print $evt_msg_snmptrap . "\n";
#print $evt_msg_syslog . "\n";
#print $event_notif_conf . "\n"; # done.
# this file should be moved to the appropriate spot.

#print $evt_connect_h . "\n"; #done

##############################################################################
# print to files

open(TRAP_OID, ">trap_oids.h") or die "counld not create file\n";
print TRAP_OID $trap_oid;
close(TRAP_OID);

#open(MSG_H, ">event_msgs.h") or die " could not create file event_msgs.h\n";
open(MSG_H, ">event_msgs.h") or die " could not create file event_msgs.h\n";
print MSG_H $evt_msg_head;
print MSG_H $evt_msg_internal;
print MSG_H $evt_msg_snmptrap;
print MSG_H $evt_msg_syslog;
close(MSG_H);

open(NOTIF, ">event_notif.conf") or die "could not create file event_notif.conf\n";
print NOTIF $event_notif_conf;
close(NOTIF);

open(CONNECT, ">event_connect.h") or die "could not create file event_connect.h\n";
print CONNECT $evt_connect_h;
close(CONNECT);

open(PROPFILE,">events.properties")or die "could not create file events.properties\n";
print PROPFILE $evt_prop_file;
close(PROPFILE);

open(DEFFILE,">event_def.h")or die "could not create file event_def.h\n";
print DEFFILE $evt_def_file;
close(DEFFILE);

open(SHLFILE,">event_shell")or die "could not create file event_shell\n";
print SHLFILE $evt_shl_file;
close(SHLFILE);

open(ARGFILE,">nmm_events.properties")or die "could not create file event_shell\n";
print ARGFILE $evt_arg_file;
close(ARGFILE);


#if($needDuplicateTargetEvents){
open(EVT_NMM_XML,">nmm_duplicate_events.xml")or die "could not create file event_shell\n";
print EVT_NMM_XML $evt_duplicate_xml;
close(EVT_NMM_XML);
#}

###############################################################################
# testing the data structures
#foreach $k (sort keys %sorted_traps){
#    print "\n" . $k . " :-";
#
#    foreach $tk (keys %{ $sorted_traps{$k} } ) {
#	print "\n\t" . $tk . ": ";
#	if ($tk =~ /variables/) {
#	    for $i (0..($sorted_traps{$k}{"num_vars"} -1)) {
#		print "\n\t\t" .$i ." => ". $sorted_traps{$k}{$tk}[$i];
#	    }
#	    next;
#	}
#	print  $sorted_traps{$k}{$tk};
#    }
#}

#foreach $k (keys %traps) {
#    print "\n" . $k . " :-";
#    foreach $tk (keys %{ $traps{$k} } ) {
#	print "\n\t" . $tk . ": ";
#	if ($tk =~ /variables/) {
#	    for $i (0..($traps{$k}{"num_vars"} -1)) {
#		print "\n\t\t" .$i ." => ". $traps{$k}{$tk}[$i];
#	    }
#	    next;
#	}
#	print  $traps{$k}{$tk};
#    }
#}
#print "\n";
#foreach $k (keys %objects) {
#    print "\n" . $k . " :-";
#    foreach $tk (keys %{ $objects{$k} } ) {
#	print "\n\t" . $tk . ": ";
#	if ($tk =~ /variables/) {
#	    for $i (0..($objects{$k}{"num_vars"} -1)) {
#		print "\n\t\t" .$i ." => ". $objects{$k}{$tk}[$i];
#	    }
#	    next;
#	}
#	print  $objects{$k}{$tk};
#    }
#}
#print "************************************************************************\n";
#print "Please move and commit the following files to ****libcyevent package****\n";
#print "****event_connect.h****\n";
#print "****event_def.h****\n";
#print "****event_shell****\n";
#print "************************************************************************\n";

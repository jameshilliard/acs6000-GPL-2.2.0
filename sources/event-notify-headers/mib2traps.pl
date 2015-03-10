#!/usr/bin/perl

# parsing MIB provided last on the commandline
# to produce header files for the event notification.

# hopefully avoid some of the hassle in translation.
# its ugly but its perl.

use File::stat;

$in_file = $ARGV[$#ARGV];

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
my $num_enum=0;

my %traps;
my %sorted_traps;
my $expected;
my $current_parse;
my $trap_name;
my $obj_name;
my $familyoid;
my $trapmgoid;
my $productoid;
my $managmentoid;

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
#	print $current_parse . ": " .$trap_name .": ". $obj_name . ": " . $oid . "\n";

	if ($current_parse =~ /object/)
	{
	    $objects{$obj_name}{'oid'} = $oid;
	    $sortable_objects{$oid} = $objects{$obj_name};
# get the number from this line to  ...
#	    print "Object: ". $objects{$obj_name}{'name'} . " " . $objects{$obj_name}{'oid'} . " " .$objects{$obj_name}{'syntax'} . "\n";
	    $obj_name = ''; # got to the end of a record
	    $current_parse = '';
	}
	if ($current_parse =~ /trap/)
	{
	    $traps{$trap_name}{'oid'} = $oid;
	    $sorted_traps{$oid} = $traps{$trap_name};
#	    $sorted_traps{$oid}{'name'} = $traps{$trap_name}{'name'};
#	    print "Trap: ". $objects{$obj_name}{'name'} . " " . $objects{$obj_name}{'oid'} . " " .$objects{$obj_name}{'syntax'} . "\n";


	    $trap_name = '';
	    $current_parse = '';
	}
	next;
    }


    if (/(^\b.*\b) OBJECT-TYPE/){ # got a object record
	$obj_name = $1;
	$current_parse = "object";
	$objects{$obj_name}{'name'} = $obj_name;
#	print $current_parse ." : ".$obj_name ."\n";
	next;
    }
    if ($obj_name && /SYNTAX (.*)/) { # build the data structure for the objects.
	my $expected_type = "TYPE_STRING" if ($1 =~ /STRING/);
	$expected_type = "TYPE_NUMBER" if ($1 =~ /INTEGER/);
	$objects{$obj_name}{'syntax'} = $expected_type;
#	print $current_parse ." : ".$obj_name .": " .$objects{$obj_name}{'syntax'} ."\n";
    }
    if (/SYNTAX INTEGER {/){
	$expected = "enum";
        next;
        }

    if ( $expected =~ /enum/ && /\b(.*)\b[,]*/) {
        # we're getting enum
	if ($1 =~ /MAX-ACCESS/ ){
	$expected="";
	chop($enum);
#	print $obj_name;
	$objects{$obj_name}{'name'}{'enum'}[$num_enum] = $enum;
#        print "|$objects{$obj_name}{'name'}{'enum'}[$num_enum]\n";
	$enum="";
	next;
	}
	$enum .= "$1),";
	next;
}

    if (/(^\b.*\b) TRAP-TYPE/){
	$trap_name = $1;
	$trap_count++;
	$current_parse = "trap";
	$traps{$trap_name}{'name'} = $trap_name;
#	print $current_parse . ": " . $traps{$trap_name}{'name'}. "\n";
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
#	print $current_parse . ": " . $traps{$trap_name}{'name'}.": " . $traps{$trap_name}{'num_vars'}. "\n";
	next;
    }
    if ($expected =~ /VARIABLES/ && /\b(.*)\b[,]*/) {
	# we're getting variables
	my $var = $1;
	$traps{$trap_name}{'variables'}[$num_vars] = $var;
#	print $current_parse . ": " . $traps{$trap_name}{'name'}."- " . $num_vars;
#	print "->".$traps{$trap_name}{'variables'}[$num_vars]. "\n";
	$num_vars++;
	next;
    }


if (/SUMMARY[^\"]*\"(.*)\"/)
{
    $traps{$trap_name}{'summary'} = $1;
#    print $current_parse .": " . $traps{$trap_name}{'name'}.": " . $traps{$trap_name}{'summary'} . "\n";
    next;
}
if ($inTRap && /ARGUMENTS (.*)$/) {
	$traps{$trap_name}{'args_list'} = $1;
	chop($traps{$trap_name}{'args_list'});
#    print $current_parse .": " . $traps{$trap_name}{'name'}.": " . $traps{$trap_name}{'args_list'} . "\n";
	next;
    }
if (/NAME[^\"]*\"(.*)\"/)
{
    $traps{$trap_name}{'evtname'} = $1;
#    print $current_parse .": " . $traps{$trap_name}{'name'}.": " . $traps{$trap_name}{'evtname'} . "\n";
    next;
}
if (/SHORT[^\"]*\"(.*)\"/)
{
    $traps{$trap_name}{'short'} = $1;
#    print $current_parse .": " . $traps{$trap_name}{'short'}.": " . $traps{$trap_name}{'summary'} . "\n";
    next;
}

if (/EVENT(.*)$/)
{
    $traps{$trap_name}{'event'} = $1;
    chop($traps{$trap_name}{'event'});
#    print $traps{$trap_name}{'event'}."\n";
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

$jav_file=<<"EJAVFILE";
EJAVFILE
######################################################################
# file the file strings
sub numerically { $a <=> $b }
for $tk (sort numerically keys %sorted_traps) {
    $final_trap = $tk;
}

for $ok (sort numerically keys %sortable_objects) {
    $jav_file .= "$sortable_objects{$ok}{'name'}=.1.3.6.1.4.1.10418.$familyoid.$managmentoid.$trapmgoid.$sortable_objects{$ok}{'oid'}";
	if ($objects{$sortable_objects{$ok}{'name'}}{'name'}{'enum'}[0])
	{
		$jav_file .="|$objects{$sortable_objects{$ok}{'name'}}{'name'}{'enum'}[0]";	
	}
    $jav_file .= "\n";
}
$jav_file .= "\n\n";

# spin through the traps.
for $evt (1..$final_trap) {
    if (($sorted_traps{$evt})&&($sorted_traps{$evt}{'variables'}[0])) {
# got a trap with this number:
	$jav_file.="event.$evt=";
	# spin through the arguments
	for $c ( 0 .. ( $sorted_traps{$evt}{'num_vars'} - 1 ) ){
	    $jav_file.="$sorted_traps{$evt}{'variables'}[$c]|";
	}
	chop($jav_file);	
	$jav_file.="\n";
    } 
}

# finish the strings
# this file should be moved to the appropriate spot.

##############################################################################
# print to files

open(JAVFILE,">traps.properties")or die "could not create file traps.properties\n";
print JAVFILE $jav_file;
close(JAVFILE);

##############################################################################
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
#print "\n";

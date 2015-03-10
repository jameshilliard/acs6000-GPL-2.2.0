#!/usr/bin/perl

# parsing MIB provided last on the commandline
# to produce header files for the event notification.

# hopefully avoid some of the hassle in translation.
# its ugly but its perl.

use File::stat;

my $template_file = $ARGV[0];
my $appliance_file = $ARGV[1];

my $model;
my $mib_file;
my $family_type;
my $family_prefix;
my $family_infix;
my $family_oid;
my $revision;

my %traps;
#my %sorted_traps;
my $expected;
my $trap_name;

my @sub_contents;

$argcnt = $#ARGV + 1;
if ( $argcnt != 2 )
{
  print "\nGeneric Template to MIB conversion script.\n\n";
  print " Takes a MIB template and appliance config file as inputs\n";
  print " and outputs an appliance specific MIB.\n\n"; 
  print " usage: template2mib.pl MIB-template-file appliance-conf\n\n";
}

######################################################################################
# Read in the configuration file specfiic for this appliance
#
#

  open(APPLIANCE, "$appliance_file") or die "Can't open $appliance_file: $!";

  #Read appliance config file
  while (<APPLIANCE>)
  {
    next if /^$/;

    if (/-- Model (.*)/)
    {
        $model= $1;
        next;
    }
    if (/-- MIBname (.*)/)
    {
        $mib_file= $1;
    }
    if (/-- Family (.*)/)
    {
    	$family_type= $1;
	next;
    } 
    if (/-- Familyoid (.*)/)
    {
        $family_oid= $1;
        next;
    }
    if (/-- Revision (.*)/)
    {
        $revision= $1;
        next;
    }
    if (/-- BeginTraps/)
    {
        while (<APPLIANCE>)
        {
            if (/(.*)(\s+)--/)
            {
                push(@usedtraps, $1);
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

#  use Data::Dumper;
#  print __FILE__ . ":usedtraps - " . Dumper(\@usedtraps) . "\n";

  $family_prefix = lc($family_type);
  $family_infix = ucfirst($family_prefix);

  open(TEMPLATE, "$template_file") or die "Can't open $template_file: $!";
  @File = <TEMPLATE>;
  close (TEMPLATE);

  #text substituion
  for (@File)
  {
    s/_#MODEL_/$model/g;   
    s/_#FAMILY_OID_/$family_oid/g;
    s/_#FAMILY_TYPE_/$family_type/g;
    s/_#FAMILY_PREFIX_/$family_prefix/g;
    s/_#FAMILY_INFIX_/$family_infix/g;
    s/_#REVISION_/$revision/g;
    push(@sub_contents,$_);
  }

############################################################################
# Parse the MIB into our hash table.
# (Taken from mib2event.pl)
#

  for (@sub_contents)
  {
    next if /^$/;
      
    if (/::=.* (\d\d*)/) {
        my $oid = $1;

        if ($current_parse =~ /object/)
        {
            $objects{$obj_name}{'oid'} = $oid;
            $sortable_objects{$oid} = $objects{$obj_name};
# get the number from this line to  ...
            $obj_name = ''; # got to the end of a record
            $current_parse = '';
        }
        if ($current_parse =~ /trap/)
        {
            $traps{$trap_name}{'oid'} = $oid;
           # $sorted_traps{$oid} = $traps{$trap_name};
#           $sorted_traps{$oid}{'name'} = $traps{$trap_name}{'name'};

            $trap_name = '';
            $current_parse = '';
        }
        next;
    }
    if (/(^\b.*\b) OBJECT-TYPE/){ # got a object record
        $obj_name = $1;
        $current_parse = "object";
        $objects{$obj_name}{'name'} = $obj_name;
        next;
    }
    if ($obj_name && /SYNTAX (.*)/) { # build the data structure for the objects.
        my $expected_type = "TYPE_STRING" if ($1 =~ /STRING/);
        $expected_type = "TYPE_NUMBER" if ($1 =~ /INTEGER/);
        $objects{$obj_name}{'syntax'} = $expected_type;
    }

    if (/(^\b.*\b) TRAP-TYPE/){
        $trap_name = $1;
        $trap_count++;
        $current_parse = "trap";
        $traps{$trap_name}{'name'} = $trap_name;
        $inTRap = 1;
        next;
    }
   
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
        next;
    }
    if ($expected =~ /VARIABLES/ && /\b(.*)\b[,]*/) {
        # we're getting variables
        my $var = $1;
        $traps{$trap_name}{'variables'}[$num_vars] = $var;
        $num_vars++;
        next;
    }  
    if (/EVENT(.*)$/)
    {
      $traps{$trap_name}{'event'} = $1;
      $traps{$trap_name}{'event'} =~ s/ //;
      chop($traps{$trap_name}{'event'});
      next;
    }
  }
  
##############################################################################
# Remove those keys corresponding to traps that are not used in this appliance. 
#
#

  for my $k1 (keys %traps) { 
    for my $k2 (keys %{$traps{$k1}}) { 
      if ($k2 eq "event")
      {    
        #print "$traps{$k1}{$k2}\n";
        $found_in_hash = 0;
        foreach (@usedtraps)
        {
          split(/ /, $_);
          #print "$_\n";
          if (@_[0] eq $traps{$k1}{$k2})
          {
            #print "found match! event: ", @_[0], "\n" , ;
	    $found_in_hash = 1;
   	  }        
        }
        # delete the key if not found in the hash table
        if (!$found_in_hash)
        {
          delete $traps{$k1};
        }
      }
    } 
  }

#for (@usedtraps)
#{
#  print "$_\n";
#}

##############################################################################
# Read the variable array's from the hasb table.
#
#

  my @vars;
  for $k1 (keys %traps)
  {
    for $k2 (keys %{$traps{$k1}})
    {
      if ($k2 eq "variables") {
        $i=0;
        $num_vars = $traps{$k1}{'num_vars'};
        while ($i < $num_vars)
        {
          push (@vars, $traps{$k1}{$k2}[$i]);
          $i++;
        }
      }
    }
  }
#foreach (@vars)
#{
#  print "$_ \n";
#}

########################################################################
# Select object vars and traps specified in the configuration file to 
# write out new MIB.
#
#

my @mib_contents;
my $push_on=1;

open (MIB, ">$mib_file") or die "Can't open $mib_file: $!";

  for (@sub_contents)
  {
    next if /^$/;

    if (/(^\b.*\b) OBJECT-TYPE/){ # got a object record
      foreach $object_var (@vars)
      {
        if ($1 eq $object_var) {
          push (@mib_contents, $_);
          $push_on = 1;
          last;
        }
        else {
          $push_on = 0;
        }
      }             
      next;
    }
    if (/(^\b.*\b) TRAP-TYPE/)
    {
      for $trap_key (keys %traps)
      {
        if ($1 eq $trap_key)
        {
          push (@mib_contents, $_);
          $push_on = 1;
          last;
        }
        else {
          $push_on = 0;
        }
      }
      next;
    }
    if ($push_on)
    {
      push(@mib_contents, $_);
    }
  }

  if ($mib_contents[-1] !~ /END/)
  {
    push(@mib_contents, "END");
  }
     
	 

# Go to the beginning of the file
  seek(MIB,0,0);

  # Update the contents with the data from our array
  print MIB (@mib_contents);
  close(MIB);


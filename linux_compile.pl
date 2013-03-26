#! /usr/bin/perl

#List the path to Qmake. This is the default value.
$default_qmake_path="/usr/bin/qmake";

# Do not edit below this line!

system(clear);
print "################################################################\n";
print "\tRoboJournal Build Helper Script for Linux/Unix\n";
print "\tVersion 1.0 -- March 22, 2013\n";
print "################################################################\n";
print "\n\n";
print "This script helps you to compile and install RoboJournal on Linux-\n";
print "based systems.\n\n";

choose_path:
print "\nEnter the path to Qmake (leave it blank to use the default
value).\n\n";
print ": ";


$qmake=<STDIN>;
chomp($qmake);

if($qmake eq ""){
  $qmake=$default_qmake_path;
  print "\nThis script will use the default Qmake location
[$default_qmake_path].\n\n";
  print "Is this correct? (Y/N)\n\n:";
  $choice1=<STDIN>;
  chomp($choice1);

  if($choice1 =~ m/[y]|[yes]/i){
    goto choose_build;
  }
  else{
    goto choose_path;
  }
}
else{
  print "Using the qmake stored at [$qmake]...\n\n";
}

choose_build:
print "\n\nDo you want to create a package build? (Y/N)\nTip: You should answer N unless you need to package RoboJournal
for the Debian repositories.\n\n:";
 $choice2=<STDIN>;

 if($choice2 =~ m/[y]|[yes]/i){
   
  }
  else{

  }


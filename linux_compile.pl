#! /usr/bin/perl

# List the path to Qmake(default value == /usr/bin/qmake). If you use a custom
# or non-standard Qmake, it helps to include the absolute (full) path to it here 
# so you don't have to type it in by hand every time you use this script.

$default_qmake_path="/usr/bin/qmake";

# Do *not* edit below this line!
##########################################################################
##########################################################################
##########################################################################

# This file is part of RoboJournal.
# Copyright (c) 2012, 2013 by Will Kraft <pwizard@gmail.com>.
# MADE IN USA
#
#
# RoboJournal is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# at your option) any later version.
#
# RoboJournal is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
#
# You should have received a copy of the GNU General Public License
# along with RoboJournal.  If not, see <http://www.gnu.org/licenses/>.

# Will Kraft, 3/28/13: Yeah, I just hacked this together. Goto is not elegant
# or pretty but it works; this isn't complex enough for 
# spaghetti code to be a problem. In any event, this is just a simple 
# build script most people are going to use just once anyway so it can be 
# kludged together and still serve its purpose.

system(clear);
print "################################################################\n";
print "\tRoboJournal Build Helper Script for Linux/Unix\n";
print "\tVersion 1.0 -- March 28, 2013 by Will Kraft\n";
print "################################################################\n";

$install_location="/usr/local/bin/robojournal";
if(-e $install_location){
die "\n\nABORT: You do not need to build RoboJournal from source because it is\nalready installed at $install_location.\n\nTo prevent conflicts, you must uninstall that executable before you can\nbuild this one! Since you must have built the currently-installed version\nfrom source at some point, run \"sudo make uninstall\" in the installed\nversion's build directory to remove it.\n\n\n";
}

$install_location2="/usr/bin/robojournal";
if(-e $install_location2){
die "\n\nABORT: You do not need to build RoboJournal from source because\nit is already installed at $install_location2.\n\nTo prevent conflicts, you must uninstall the current \"robojournal\"\npackage before you can build this version!\n\n\n";
}

$collectiongenerator="/usr/bin/qcollectiongenerator";
unless(-e $collectiongenerator){
die "\n\nABORT: Setup cannot continue because QCollectionGenerator is not installed\nin the correct location (/usr/bin/qcollectiongenerator). This program is\nneeded to prepare the documentation files. Debian (and Ubuntu/Mint) users\nshould install the \"qt4-dev-tools\" package to fix this problem.\n\n\n";
}

print "\n\n";
print "This script helps you to compile and install RoboJournal on Linux-\n";
print "based systems by automating most of the process.\n\n";



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
  print "Is this correct? (Y/N)\n\n: ";
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
for the Debian repositories.\n\n: ";
 $choice2=<STDIN>;

print "\n\nDo you want to install the program after compiling it? (Y/N)\nFYI: Installation requires sudo access! Furthermore, documentation\ndoesn't work correctly without a proper installation.\n\n: ";
$choice3=<STDIN>;


if($choice3 =~ m/[y]|[yes]/i){
  $function="Compile and Installation";
  $launcher="robojournal";
  }
else{
  $function="Compile";
  $launcher="./robojournal";
}

 # create package build
 if($choice2 =~ m/[y]|[yes]/i){
   print "\n\nCleaning up...\n\n";
    system("make distclean");
    $qmake_cmd=$qmake . " CONFIG+=package robojournal.pro";
    print " Running qmake: ";
    print $qmake_cmd . "\n\n";
    system($qmake_cmd);
    system("make -j 3");

    
    
    if($choice3 =~ m/[y]|[yes]/i){
      system ("sudo make install");
    }  
  }
  # create regular build
  else{
    print "\n\nCleaning up...\n\n";
    system("make distclean");
    $qmake_cmd=$qmake . " robojournal.pro";
    print " Running qmake: ";
    print $qmake_cmd . "\n\n";
    system($qmake_cmd);
    system("make -j 3");
   
    if($choice3 =~ m/[y]|[yes]/i){
      system ("sudo make install");
    }  
}

print "################################################################\n";
print "\t$function complete!\n\n";
print "Run \"$launcher\" (without quotation marks) to start the program.\n";
print "################################################################\n";

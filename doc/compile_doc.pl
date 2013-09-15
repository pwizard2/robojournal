#! /usr/bin/perl
#
# This file is part of RoboJournal.
# Copyright (c) 2012, 2013 by Will Kraft <pwizard@gmail.com>.
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

# Will Kraft (9/12/13): This script is meant to replace the old documentation
# build script (renamed "legacy_compile_doc.sh" in this release). This new script
# is a lot more robust because it uses several attempts to compile the 
# documentation and returns false only if all attempts fail.

##########################################################################

# declare preliminary variables.
$cmd=" doc/robojournal.qhcp -o doc/robojournal.qhc";

$exec1="qcollectiongenerator"; #use relative path first.
$exec2="/usr/bin/qcollectiongenerator"; # this absolute command should work on most systems.
$exec3="/usr/lib/x86_64-linux-gnu/qt4/bin/qcollectiongenerator"; #backup command in case Qt5 is installed too. 

# define output files
$output1="doc/robojournal.qhc";
$output2="doc/robojournal.qch";

print "##########################################################################\n";
print "\t\t\tCompiling Documentation...\n";
print "##########################################################################\n";

# build run command that gets fed into system();
$run = $exec1 . $cmd;

# try to build documentation with relative path first. (qcollectiongenerator)
system(`$run`);

if((!-e $output1) && (!-e $output2)){
  print "First documentation build attempt failed. Using alternate path 2...\n\n";
  $run = $exec2 . $cmd;
}
else{
  print "\nDocumentation successfully generated.\n\n";
  exit(0);
}

# try to build documentation with absolute path (/usr/bin/qcollectiongenerator)
system(`$run`);

if((!-e $output1) && (!-e $output2)){
  print "Second documentation build attempt failed. Using alternate path 3...\n\n";
  $run = $exec3 . $cmd;
}
else{
  print "\nDocumentation successfully generated.\n\n";
  exit(0);
}

# try to build documentation with failsafe path (/usr/lib/x86_64-linux-gnu/qt4/bin/qcollectiongenerator)
system(`$run`);

if((!-e $output1) && (!-e $output2)){
  die "Fatal error! Documentation could not be generated.\n";
}
else{
  print "\nDocumentation successfully generated.\n\n";
  exit(0);
}
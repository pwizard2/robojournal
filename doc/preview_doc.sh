#! /bin/bash

# Will Kraft (3/30/13) 
# Help file launcher script that launches the help collection file
# in Qt assistant. If the help file appears when this script is executed 
# then it means that everything installed correctly.

clear
echo Testing help file. Please wait...
echo 
echo 
echo \(If everything is installed correctly, Qt Assistant will appear and display 
echo the RoboJournal documentation.\)
echo 
# PATCH 4/18/13: all doc should be installed to the same place regardless of version
assistant -collectionFile /usr/share/doc/robojournal/robojournal.qhc


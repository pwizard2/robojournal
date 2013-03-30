#! /bin/bash

# Will Kraft, 3/30/13.
#
# [re]create documentation files from their base components. 
# This script is supposed to be execd during Qmake configuration;
# the paths may not work correctly if this script is used independently.
# 
# During a manual rebuild, all compiled documentation files (robojournal.qch
# and robojournal.qhc) should be installed to /usr/share/doc/robojournal-[version] 
# (i.e. /usr/share/doc/robojournal-0.4.1). 

qcollectiongenerator doc/robojournal.qhcp -o doc/robojournal.qhc
chmod 644 doc/robojournal.qch
chmod 644 doc/robojournal.qhc

#! /bin/bash

#put your email address here
email="foo@example.com"

# Where is the RoboJournal source folder stored? (list without trailing slash). This
# MUST be the real folder, not just a symlink to it!
source_dir=/home/will/qt_projects/robojournal

##############################################################################
#			DO NOT MODIFY ANYTHING BELOW THIS LINE!
##############################################################################

cd ../..
mkdir ~/robojournal-0.5

# The source folder is called "robojournal" if you are building from the Github copy. If you are building from a tarball, 
# the source folder is probably already called robojournal-0.5.

tar -czvf ~/robojournal-0.5/robojournal-0.5.tar.gz $source_dir --exclude=.git 


# do the packaging

cd ~/robojournal-0.5
dh_make -s -e $email -f ~/robojournal-0.5/robojournal-0.5.tar.gz



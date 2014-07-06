#! /bin/bash

# List location of robojournal-0.5 upstream folder.
UPSTREAM=/home/will/compile/robojournal-0.5

########################################################################
cd ../..
tar czvf robojournal_0.5.orig.tar.gz $UPSTREAM --exclude=.git
cd $UPSTREAM
mkdir ../debian
cd pkg
cp changelog compat control copyright rules watch ../debian
mkdir ../debian/source
touch ../debian/source/format
echo 3.0 \(quilt\) > ../debian/source/format
cd ../debian
debuild -us -uc

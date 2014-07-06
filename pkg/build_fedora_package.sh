#! /bin/bash


# This script creates an RPM package of RoboJournal for your computer's CPU architecture.
# For the sake of simplicity, it assumes RPM build tools are already installed and all 
# RoboJournal dependencies are met. If you are *NOT* running a relatively new version of 
# Fedora (>= 18), this script is probably useless to you.

# Will Kraft (6/28/14)

# Uncomment the next line if you have a virgin system (i.e. need to set up the RPM build tools/get dependencies too).
# sudo yum install @development-tools fedora-packager qt qt-assistant qt-mysql qt-devel qt-webkit qt-webkit-devel gcc-c++

rpmdev-setuptree
cp robojournal.spec ~/rpmbuild/SPECS
cd ..
cd ..
cp -r robojournal robojournal-0.5
tar -czvf ~/rpmbuild/SOURCES/robojournal-0.5.tar.gz robojournal-0.5 --exclude=.git
rm -rf robojournal-0.5
rpmbuild -ba ~/rpmbuild/SPECS/robojournal.spec
echo  
echo ---------------------------------------------------------------------------------------
echo RPM packages created, look inside the ~/rpmbuild/RPMS folder. Source RPMS are in the 
echo ~/rpmbuild/SRPMS folder.
echo ---------------------------------------------------------------------------------------
echo 

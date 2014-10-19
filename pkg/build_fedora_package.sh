#! /bin/bash

# This file is part of RoboJournal.
# Copyright (c) 2012-2015 by Will Kraft <pwizard@gmail.com>.
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

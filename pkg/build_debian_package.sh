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

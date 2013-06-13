/*
    This file is part of RoboJournal.
    Copyright (c) 2013 by Will Kraft <pwizard@gmail.com>.
    MADE IN USA

    RoboJournal is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RoboJournal is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RoboJournal.  If not, see <http://www.gnu.org/licenses/>.

    Class description/purpose (6/13/13), --Will Kraft:
    The HTMLCore class contains all methods that are required to turn raw Editor
    class output into a viable HTML-based entry that can be stored in a database.
    Once processed, the HTMLCore output is ready to be fed to MySQLCore or SQLiteCore.
*/

#ifndef HTMLCORE_H
#define HTMLCORE_H

#include <QString>

class HTMLCore
{
public:
    HTMLCore();
    QString Do_Post_Processing(QString rawtext, int wordcount);
};

#endif // HTMLCORE_H

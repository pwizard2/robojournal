/*
    This file is part of RoboJournal.
    Copyright (c) 2013 by Will Kraft <pwizard@gmail.com>.
    

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
    The HTMLCore also converts raw database output into a HTML document each time
    an entry needs to be displayed.

    Update 7/30/13: The HTMLCore now contains the code that outputs HTML or plain text
    from the database for the EntryExporter/ExportSingle/ExportMulti classes.
*/

#ifndef HTMLCORE_H
#define HTMLCORE_H

#include <QString>

class HTMLCore
{
public:
    HTMLCore();
    QString Do_Post_Processing(QString rawtext, int wordcount);
    QString ProcessEntryFromEditor(QString rawtext);
    QString AssembleEntry(QString id);
    void Do_Export(QString path, QString id, bool use_html);
    void Export_Multi(QString path, bool use_html, bool sort_asc);
    void Setup_Export_CSS();
    void Export_Loose_Journal_Entries(QString folder, bool use_html);

    QString body_font;
    QString header_font;
    QString body_font_size;
    QString header_font_size;
    QString datebox_bgcolor;
    QString datebox_color;
};

#endif // HTMLCORE_H

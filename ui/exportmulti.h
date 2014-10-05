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

    Class description / purpose:
    The ExportMulti class creates the "Export entire journal" page
    in the EntryExporter class window and is responsible for passing
    all necessary form data back to the ExtryExporter container
    class. --Will Kraft (8/3/13).
 */

#ifndef EXPORTMULTI_H
#define EXPORTMULTI_H

#include <QWidget>

namespace Ui {
class ExportMulti;
}

class ExportMulti : public QWidget
{
    Q_OBJECT
    
public:
    explicit ExportMulti(QWidget *parent = 0);
    ~ExportMulti();

    //static QString folder_name;
    static QString path;
    static QString filename;
    static bool sort_asc;
    static bool use_html;
    static bool no_merge;


    void HarvestValues();


private slots:
    void on_BrowseButton_clicked();

    void on_HTML_clicked();

    void on_PlainText_clicked();

    void on_IncludeExportDate_clicked();

    void on_ExportLooseFiles_toggled(bool checked);

    void on_PermitName_toggled(bool checked);

private:
    Ui::ExportMulti *ui;
    void PrimaryConfig();
    void Browse();
    void SetMassName();
};

#endif // EXPORTMULTI_H

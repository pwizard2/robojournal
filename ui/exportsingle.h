/*
    This file is part of RoboJournal.
    Copyright (c) 2015 by Will Kraft <pwizard@gmail.com>.
    

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
  */

#ifndef EXPORTSINGLE_H
#define EXPORTSINGLE_H

#include <QWidget>

namespace Ui {
class ExportSingle;
}

class ExportSingle : public QWidget
{
    Q_OBJECT
    
public:
    explicit ExportSingle(QWidget *parent = 0);
    ~ExportSingle();
    void HarvestValues();

    static QString filename;
    static QString path;
    static bool use_html;


    
private slots:
    void on_BrowseButton_clicked();

    void on_HTML_clicked();

    void on_PlainText_clicked();

    void on_IncludeDate_clicked();

    void on_IncludeJournalName_clicked();

    void on_AllowFreeEdits_toggled(bool checked);

private:
    Ui::ExportSingle *ui;
    void PrimaryConfig();
    void Browse();
    void SetName();
    QString SetExtension(QString path);
};

#endif // EXPORTSINGLE_H

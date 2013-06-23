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

    6/23/13: This class contains all journal creation functions for SQLite backends.
*/


#ifndef SQLITEJOURNALPAGE_H
#define SQLITEJOURNALPAGE_H

#include <QWidget>

namespace Ui {
class SQLiteJournalPage;
}

class SQLiteJournalPage : public QWidget
{
    Q_OBJECT
    
public:
    explicit SQLiteJournalPage(QWidget *parent = 0);
    ~SQLiteJournalPage();
    bool HarvestData();


public slots:
    void ClearForm();

signals:
    void unlockOK();
    void unlockNotOK();
    
private:
    Ui::SQLiteJournalPage *ui;
    void PrimaryConfig();
    void Browse(QString startpath);
    bool FilenameValid(QString filename);
    void ProcessFilename(QString filename, bool valid);
    void Create_My_Journals_Folder();


private slots:
      void on_DatabaseName_textChanged(const QString &arg1);
      void on_BrowseButton_clicked();
};

#endif // SQLITEJOURNALPAGE_H

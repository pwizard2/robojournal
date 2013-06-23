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

    6/23/13: This class contains/manages the joural creator page classes
    and communicates with the various SQL core classes.

*/

#ifndef NEWJOURNALCREATOR_H
#define NEWJOURNALCREATOR_H

#include <QDialog>
#include <QStackedWidget>
#include "ui/journalcreatorcoverpage.h"
#include "ui_journalcreatorcoverpage.h"
#include "ui/mysqljournalpage.h"
#include "ui_mysqljournalpage.h"
#include "ui/sqlitejournalpage.h"
#include "ui_sqlitejournalpage.h"

namespace Ui {
class NewJournalCreator;
}

class NewJournalCreator : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewJournalCreator(QWidget *parent = 0);
    ~NewJournalCreator();

    static QString journal_name;
    static QString username;
    static QString password;

    static QString hostname;
    static QString port;
    static QString root_password;

    static QString sqlite_journal_name;
    static QString sqlite_journal_path;


signals:
    void Clear_MySQL();
    void Clear_SQLite();

public slots:
    void lockOKButton();
    void unlockOKButton();
    void RestoreDefaults();
    
private slots:
    void on_DatabaseType_currentRowChanged(int currentRow);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_buttonBox_rejected();





private:
    Ui::NewJournalCreator *ui;

    void PrimaryConfig();

    QStackedWidget *stack;
    MySQLJournalPage *m;
    SQLiteJournalPage *s;
    JournalCreatorCoverPage *c;

    bool Create_MySQL_Database();
    bool Create_SQLite_Database();

    void accept();
};

#endif // NEWJOURNALCREATOR_H

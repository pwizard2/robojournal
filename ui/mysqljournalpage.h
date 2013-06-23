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

    6/23/13: This class contains all journal creation functions for MySQL backends.
*/


#ifndef MYSQLJOURNALPAGE_H
#define MYSQLJOURNALPAGE_H

#include <QWidget>


namespace Ui {
class MySQLJournalPage;
}

class MySQLJournalPage : public QWidget
{
    Q_OBJECT
    
public:
    explicit MySQLJournalPage(QWidget *parent = 0);
    ~MySQLJournalPage();

    bool Validate();
    void HarvestData();


signals:
    void unlockOK();
    void unlockNotOK();

public slots:
    void ClearForm();

    
private slots:
    void on_Password1_textChanged();

    void on_Password2_textEdited();

    void on_JournalName_editingFinished();

    void on_Username_editingFinished();

private:
    Ui::MySQLJournalPage *ui;
    void PrimaryConfig();
    void PasswordsMatch();
    int PasswordStrength(QString passwd);



};

#endif // MYSQLJOURNALPAGE_H

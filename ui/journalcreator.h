/*
    This file is part of RoboJournal.
    Copyright (c) 2012 by Will Kraft <pwizard@gmail.com>.
    

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

#ifndef JOURNALCREATOR_H
#define JOURNALCREATOR_H

#include <QDialog>

namespace Ui {
    class JournalCreator;
}

class JournalCreator : public QDialog
{
    Q_OBJECT

public:
    explicit JournalCreator(QWidget *parent = 0);
    ~JournalCreator();

private slots:
    void on_DBType_currentIndexChanged(int index);

    void on_Password2_textChanged();

    void on_Password_textChanged();

    void on_tabWidget_currentChanged();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::JournalCreator *ui;
    void ShowDescription(int index);
    void PasswordMatch();
    bool Validate();
    void FillIn();
    void Clear();
    void CreateJournal();
    void PrimaryConfig();
};

#endif // JOURNALCREATOR_H

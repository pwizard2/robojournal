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
  */

#ifndef TAGREMINDER_H
#define TAGREMINDER_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class TagReminder;
}

class TagReminder : public QDialog
{
    Q_OBJECT
    
public:
    explicit TagReminder(QWidget *parent = 0);
    ~TagReminder();
    int Lookup();
    int GetEntries();
    
private slots:
    void on_ManageTags_clicked();
    void on_EntryList_itemSelectionChanged();
    void reject();


private:
    Ui::TagReminder *ui;
    void PrimaryConfig();
    void CreateList();
    void LaunchTagger();
    void RefreshList();
};

#endif // TAGREMINDER_H

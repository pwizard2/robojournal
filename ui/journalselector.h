/*
    This file is part of RoboJournal.
    Copyright (c) 2012 by Will Kraft <pwizard@gmail.com>.
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
  */


#ifndef JOURNALSELECTOR_H
#define JOURNALSELECTOR_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <QAbstractButton>

namespace Ui {
class JournalSelector;
}

class JournalSelector : public QDialog
{
    Q_OBJECT
    
public:
    explicit JournalSelector(QWidget *parent = 0);
    ~JournalSelector();
    
private slots:
    void on_SearchButton_clicked();

    void on_UseDefaults_clicked(bool checked);

    void on_JournalList_itemClicked(QTreeWidgetItem *item, int column);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_JournalList_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_ManagePermissions_clicked();

    void on_DBType_currentIndexChanged(int index);

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::JournalSelector *ui;

    void JournalSearch();
    void CreateTree(QStringList journals);
    void SetPreferences();
    bool Validate();
    void ResetForm();

};

#endif // JOURNALSELECTOR_H

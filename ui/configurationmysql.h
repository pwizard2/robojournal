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


#ifndef CONFIGURATIONMYSQL_H
#define CONFIGURATIONMYSQL_H

#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui {
class ConfigurationMySQL;
}

class ConfigurationMySQL : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigurationMySQL(QWidget *parent = 0);
    ~ConfigurationMySQL();
    void GetChanges();

private slots:
    void on_DefaultPort_clicked();

    void on_Port_textChanged(const QString &arg1);

    void on_KnownJournals_itemDoubleClicked(QTreeWidgetItem *item);

private:

    Ui::ConfigurationMySQL *ui;
    void PopulateForm();
    void Show_Known_Journals();
    void ApplyDefaultProperties(QTreeWidgetItem *item);
    void Harvest_Favorite_Databases();
    void demoteDatabase(QTreeWidgetItem *item);

    QTreeWidgetItem *default_db;

    // Bugfix 11/17/13: Prevent the messagebox from being displayed more than once. If the SQLite driver is not present, the messagebox
    // displays over and over again, locking up the entire program. This bool keeps track of whether the box has been displayed
    // already. --Will Kraft
    bool has_displayed;

    // Store whether the confirmation response should be shown for ApplyDefaultProperties. This is here to prevent the
    // notification from being shown when the form loads up.
    bool load_up;
};

#endif // CONFIGURATIONMYSQL_H

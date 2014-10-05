/*
    This file is part of RoboJournal.
    Copyright (c) 2014 by Will Kraft <pwizard@gmail.com>.

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


#ifndef CONFIGURATIONSQLITE_H
#define CONFIGURATIONSQLITE_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class ConfigurationSQLite;
}

class ConfigurationSQLite : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigurationSQLite(QWidget *parent = 0);
    ~ConfigurationSQLite();
    void GetChanges();

private:
    Ui::ConfigurationSQLite *ui;
    void PopulateForm();
    void Show_Known_Journals();
    void ApplyDefaultProperties(QListWidgetItem *item);
    void demoteDatabase(QListWidgetItem *item);
    void Harvest_Favorite_Databases();

    QListWidgetItem *default_db;

private slots:
    void on_Favorites_itemDoubleClicked(QListWidgetItem *item);

};

#endif // CONFIGURATIONSQLITE_H

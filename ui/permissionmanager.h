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

    6/23/13: This class simplifies the granting of journal database permissions
    to low-level user accounts. Once this works properly, the user will no
    longer have to run GRANT or CREATE USER queries manually.
*/

#ifndef PERMISSIONMANAGER_H
#define PERMISSIONMANAGER_H

#include <QDialog>
#include <QPushButton>
#include "sql/sqlshield.h"

namespace Ui {
    class PermissionManager;
}

class PermissionManager : public QDialog
{
    Q_OBJECT

public:
    explicit PermissionManager(QWidget *parent = 0);
    ~PermissionManager();

    static int backend;
    static QString username;
    static QString password;

private slots:
    void on_Scan_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::PermissionManager *ui;
    void PrimaryConfig();
    bool Validate();
    void DatabaseScan();
    void ResetForm();
};

#endif // PERMISSIONMANAGER_H

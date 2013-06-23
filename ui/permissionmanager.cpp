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

    6/23/13: This class simplifies the granting of journal database permissions
    to low-level user accounts. Once this works properly, the user will no
    longer have to run GRANT or CREATE USER queries manually.
*/


#include "permissionmanager.h"
#include "ui_permissionmanager.h"
#include <QPushButton>
#include "sql/mysqlcore.h"
#include "sql/sqlshield.h"

PermissionManager::PermissionManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PermissionManager)
{
    ui->setupUi(this);
    PrimaryConfig();

}

PermissionManager::~PermissionManager()
{
    delete ui;
}

void PermissionManager::PrimaryConfig(){
    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    int width=this->width();
    int height=this->height();
    this->setMaximumSize(width,height);
    this->setMinimumSize(width,height);

    ok=ui->buttonBox->button(QDialogButtonBox::Ok);
    ok->setEnabled(false);

    // define default settings
    ui->CreateUser->setChecked(true);
    ui->GrantSettings->setDisabled(true);
}

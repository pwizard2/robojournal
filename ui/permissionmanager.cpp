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
#include <QIcon>
#include <QMessageBox>

PermissionManager::PermissionManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PermissionManager)
{
    ui->setupUi(this);
    PrimaryConfig();

}

int PermissionManager::backend;
QString PermissionManager::username;
QString PermissionManager::password;

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

    QPushButton *ok=ui->buttonBox->button(QDialogButtonBox::Ok);
    ok->setEnabled(false);

    // define default settings
    ui->CreateUser->setChecked(true);
    ui->GrantSettings->setDisabled(true);

    ui->RootPassword->setFocus();

    //connect(ui->RootPassword, SIGNAL(returnPressed()), ui->Scan, SLOT(click()));

    switch(backend){
        case 0:
            this->setWindowTitle(this->windowTitle() + " - MySQL");
        break;

        case 1:
            // do nothing; SQLite doesn't need configuration.
        break;

        case 2:
            this->setWindowTitle(this->windowTitle() + " - Postgres");
        break;
    }
}

void PermissionManager::DatabaseScan(){

    // MySQL Mode
    if(backend==0){

        ui->Databases->clear();

        QString hostname, port, root_password;

        hostname=ui->DBHost->text();

        if(ui->DBHost->text().isEmpty()){
            hostname=ui->DBHost->placeholderText();
        }

        port=ui->Port->text();

        if(port.isEmpty()){
            port=ui->Port->placeholderText();
        }

        root_password=ui->RootPassword->text();

        SQLShield s;

        hostname=s.Break_Injections(hostname);
        port=s.Break_Injections(port);
        root_password=s.Break_Injections(root_password);

        QIcon db(":/icons/database.png");

        MySQLCore m;

        QStringList journals=m.GetDatabaseList(hostname,port,"root",root_password,false);
        journals.sort();

        for(int i=0; i<journals.size(); i++){
            QString db_name=journals.at(i);
            ui->Databases->addItem(db,db_name);
        }

        ui->GrantSettings->setEnabled(true);

        if(ui->Databases->count() > 0){

            ui->Scan->setEnabled(false);

            // unlock ok button
            QPushButton *ok=ui->buttonBox->button(QDialogButtonBox::Ok);
            ok->setEnabled(true);
        }

        if((!username.isEmpty()) || (!password.isEmpty())){
            ui->Username->setText(username);
            ui->Password->setText(password);
        }
    }
}

// reset form values. 6/27/13.
void PermissionManager::ResetForm(){
    ui->DBHost->clear();
    ui->Databases->clear();
    ui->Password->clear();
    ui->Username->clear();
    ui->Port->clear();
    ui->UserHost->clear();
    ui->Scan->setEnabled(true);
    ui->RootPassword->clear();
    ui->CreateUser->setChecked(true);
    ui->GrantSettings->setEnabled(false);
}


bool PermissionManager::Validate(){

    QMessageBox m;

    if(ui->Username->text().isEmpty()){
        m.critical(this,"RoboJournal","You must enter a username.");
        ui->Username->setFocus();
        return false;
    }

    QRegExp root("root", Qt::CaseInsensitive);

    if(root.exactMatch(ui->Username->text())){
        m.critical(this,"RoboJournal","You cannot use root as a username! Please enter something else.");
        ui->Username->setFocus();
        ui->Username->clear();
        return false;
    }

    if(ui->Password->text().isEmpty()){
        m.critical(this,"RoboJournal","You must enter a password.");
        ui->Password->setFocus();
        return false;
    }

    else{
        return true;
    }
}

void PermissionManager::on_Scan_clicked()
{
    DatabaseScan();
}

void PermissionManager::on_buttonBox_accepted()
{
    bool valid=Validate();

    if(valid){

        // Grant permissions to a MySQL user
        if(backend==0){

            QString hostname, port, root_password, user_host, username, password, database;

            hostname=ui->DBHost->text();

            if(ui->DBHost->text().isEmpty()){
                hostname=ui->DBHost->placeholderText();
            }

            port=ui->Port->text();

            if(port.isEmpty()){
                port=ui->Port->placeholderText();
            }

            root_password=ui->RootPassword->text();

            user_host=ui->UserHost->text();

            if(user_host.isEmpty()){
                user_host=ui->UserHost->placeholderText();
            }

            username=ui->Username->text();
            password=ui->Password->text();
            database=ui->Databases->currentText();

            SQLShield s;
            hostname=s.Break_Injections(hostname);
            port=s.Break_Injections(port);
            root_password=s.Break_Injections(root_password);
            user_host=s.Break_Injections(user_host);
            username=s.Break_Injections(username);
            password=s.Break_Injections(password);

            QMessageBox b;
            MySQLCore m;
            bool success2=m.GrantPermissions(ui->CreateUser->isChecked(),database,user_host,
                                             username,port,hostname,password,root_password);

            if(success2){
                b.information(this,"RoboJournal","The necessary permissions to access database <b>" + database
                              + "</b> have been granted to " + username + "@" + user_host + ".");
                close();
            }
            else{
                b.critical(this,"RoboJournal","Due to an unknown error, RoboJournal could not grant permission to use <b>"
                           + database + "</b> to " + username + "@" + user_host + ".");
            }
        }
    }
}

void PermissionManager::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button->text()=="Restore Defaults"){
        ResetForm();
    }
}

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

#include "configurationmysql.h"
#include "ui_configurationmysql.h"
#include "core/buffer.h"
#include "ui/newconfig.h"

ConfigurationMySQL::ConfigurationMySQL(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationMySQL)
{
    ui->setupUi(this);
    PopulateForm();


}

ConfigurationMySQL::~ConfigurationMySQL()
{
    delete ui;
}

void ConfigurationMySQL::on_DefaultPort_clicked()
{
    ui->Port->setText("3306");
    ui->DefaultPort->setDisabled(true);
}

void ConfigurationMySQL::on_Port_textChanged(const QString &arg1)
{
    if(arg1 != "3306"){
        ui->DefaultPort->setDisabled(false);
    }
    else{
        ui->DefaultPort->setDisabled(true);
    }
}

void ConfigurationMySQL::PopulateForm(){

    ui->Username->setText(Buffer::defaultuser);
    ui->DefaultHost->setText(Buffer::defaulthost);
    ui->Port->setText(QString::number(Buffer::databaseport));
    ui->Database->setText(Buffer::defaultdatabase);

    ui->AllowRoot->setChecked(Buffer::allowroot);

}


void ConfigurationMySQL::GetChanges(){

    Newconfig::new_default_user=ui->Username->text();
    Newconfig::new_default_db=ui->Database->text();
    Newconfig::new_default_host=ui->DefaultHost->text();
    Newconfig::new_default_port=ui->Port->text();
    Newconfig::new_allow_root=ui->AllowRoot->isChecked();
}

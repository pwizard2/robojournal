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

#include "configurationgeneral.h"
#include "ui_configurationgeneral.h"
#include <QDate>
#include "core/buffer.h"
#include "ui/newconfig.h"
#include "ui/mainwindow.h"


ConfigurationGeneral::ConfigurationGeneral(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationGeneral)
{
    ui->setupUi(this);


    // Set up Database List
    QIcon mysql(":/icons/mysql_icon2.png");
    QIcon sqlite(":/icons/sqlite_icon.png");

    ui->Backend->addItem(mysql,"MySQL");
    ui->Backend->addItem(sqlite,"SQLite");


    PopulateForm();


}

ConfigurationGeneral::~ConfigurationGeneral()
{
    delete ui;
}


void ConfigurationGeneral::DateExample(int index){
    // get today's date for example
    QDate today;
    today=today.currentDate();

    int d=today.day();
    int y=today.year();
    int m=today.month();

    QString month=today.longMonthName(m);
    QString year=QString::number(y);
    QString day=QString::number(d);

    if(index==0){
        ui->DateExample->setText("Example: " + day + " " + month + ", " + year + " (DD-MM-YYYY)");
    }

    if(index==1){
        ui->DateExample->setText("Example: " + month + " " + day + ", " + year + " (MM-DD-YYYY)");
    }

    if(index==2){
        ui->DateExample->setText("Example: " + year + ", " + month + " " + day + " (YYYY-MM-DD)");
    }
}

void ConfigurationGeneral::on_DateFormat_currentIndexChanged(int index)
{
    DateExample(index);
}

// Set form fields to match config data in buffer
void ConfigurationGeneral::PopulateForm(){


    ui->AlwaysUseDefaults->setChecked(Buffer::alwaysusedefaults);
    ui->UseConfirmation->setChecked(Buffer::showwarnings);

    // 6/21/13: Hide the UsetoolbarLabels object because that item is deprecated in 0.5.
    // Hiding it is easier than stripping out the code from every class but I'm sure I'll do it eventually.
    // -- Will Kraft
    ui->UseToolbarLabels->setVisible(false);
    ui->UseToolbarLabels->setChecked(Buffer::show_icon_labels);

    ui->Usealternating->setChecked(Buffer::alternate_rows);
    ui->NameTitlebar->setChecked(Buffer::name_in_titlebar);
    ui->ToolbarPosition->setCurrentIndex(Buffer::toolbar_pos);
    ui->TagReminder->setChecked(Buffer::show_untagged_reminder);

    if(Buffer::backend=="MySQL"){
        ui->Backend->setCurrentIndex(0);
    }

    if(Buffer::backend=="SQLite"){
        ui->Backend->setCurrentIndex(1);
    }

    ui->RecordTime->setChecked(Buffer::keep_time);
    ui->UseMilitary->setChecked(Buffer::use_24_hour);

    ui->DateFormat->setCurrentIndex(Buffer::date_format);
    DateExample(Buffer::date_format);
    ui->DOW->setChecked(Buffer::use_dow);
}

void ConfigurationGeneral::GetChanges(){

    Newconfig::new_always_use_defaults=ui->AlwaysUseDefaults->isChecked();
    Newconfig::new_show_confirm=ui->UseConfirmation->isChecked();
    Newconfig::new_icon_labels=ui->UseToolbarLabels->isChecked();
    Newconfig::new_alternate_rows=ui->Usealternating->isChecked();
    Newconfig::new_name_in_titlebar=ui->NameTitlebar->isChecked();
    Newconfig::new_toolbar_pos=ui->ToolbarPosition->currentIndex();
    Newconfig::new_dbtype=ui->Backend->currentText();
    Newconfig::new_use_dow=ui->DOW->isChecked();
    Newconfig::new_record_time=ui->RecordTime->isChecked();
    Newconfig::new_24_hr=ui->UseMilitary->isChecked();
    Newconfig::new_date_mode=ui->DateFormat->currentIndex();
    Newconfig::new_show_untagged_reminder=ui->TagReminder->isChecked();
}

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

#include "configurationjournal.h"
#include "ui_configurationjournal.h"
#include <QIcon>
#include "core/buffer.h"
#include "ui/newconfig.h"

ConfigurationJournal::ConfigurationJournal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationJournal)
{
    ui->setupUi(this);
    PrimaryConfig();
    PopulateForm();
}

ConfigurationJournal::~ConfigurationJournal()
{
    delete ui;
}

void ConfigurationJournal::on_ShowAllEntries_clicked(bool checked)
{
    if(checked){
        ui->YearRange->setDisabled(true);
        ui->YearRange->setCurrentIndex(-1);
        ui->ShowRangeIndicator->setDisabled(true);
        ui->ShowRangeIndicator->setChecked(false);
        ui->label->setDisabled(true);
    }
    else{
        ui->YearRange->setDisabled(false);
        ui->YearRange->setCurrentIndex(0);
        ui->ShowRangeIndicator->setDisabled(false);
        ui->ShowRangeIndicator->setChecked(true);
        ui->label->setDisabled(false);

    }
}

void ConfigurationJournal::PrimaryConfig(){

}

// Load form data into temporary Newconfig buffer. this function gets called whenever the config data gets updated.
void ConfigurationJournal::GetChanges(){
    Newconfig::new_show_all_entries=ui->ShowAllEntries->isChecked();

    // Year range requires a 1-year offset since ComboBox index starts at 0 instead of 1.
    int year_range=ui->YearRange->currentIndex();
    year_range++;
    Newconfig::new_entry_range=year_range;

    Newconfig::new_indicator=ui->ShowRangeIndicator->isChecked();
    Newconfig::new_full_name=ui->FullName->text();
    Newconfig::new_gender_is_male=ui->Male->isChecked();
    Newconfig::new_sort_by_day=ui->SortByDay->isChecked();
    Newconfig::new_show_title=ui->ShowTitle->isChecked();
    Newconfig::new_autoload=ui->Autoload->isChecked();
    Newconfig::new_use_highlights=ui->Highlight->isChecked();
    Newconfig::new_use_full_name=ui->UseFullName->isChecked();
    Newconfig::new_highlight_color=ui->HighlightColor->currentIndex();
    Newconfig::new_entry_node_icon=ui->NodeIcon->currentIndex();

}

// Set form fields to match config data in buffer
void ConfigurationJournal::PopulateForm(){

    if(Buffer::allentries){
        ui->ShowAllEntries->click();
    }
    else{
        ui->ShowRangeIndicator->setChecked(Buffer::use_indicator);

        // QCombobox index values start at 0, so we need to subtract one year to compensate.
        int range=Buffer::entryrange-1;
        ui->YearRange->setCurrentIndex(range);
    }

    ui->FullName->setText(Buffer::full_name);

    if(Buffer::is_male){
        ui->Male->click();
    }
    else{
        ui->Female->click();
    }

    ui->SortByDay->setChecked(Buffer::sortbyday);
    ui->ShowTitle->setChecked(Buffer::show_title);
    ui->Autoload->setChecked(Buffer::autoload);
    ui->Highlight->setChecked(Buffer::use_highlight);
    ui->UseFullName->setChecked(Buffer::use_full_name);

    ui->NodeIcon->setCurrentIndex(Buffer::entry_node_icon);

    if(Buffer::use_highlight){
        ui->HighlightColor->setCurrentIndex(Buffer::highlight_color);
    }
    else{
        ui->HighlightColor->setDisabled(true);
        ui->HighlightColor->setCurrentIndex(Buffer::highlight_color);
    }

    if(ui->FullName->text().isEmpty()){
        ui->UseFullName->setChecked(false);
        ui->UseFullName->setEnabled(false);
    }

}

void ConfigurationJournal::on_Highlight_clicked(bool checked)
{

    if(checked){
        ui->HighlightColor->setDisabled(false);
    }
    else{
        ui->HighlightColor->setDisabled(true);
    }
}

void ConfigurationJournal::on_FullName_textChanged(const QString &arg1)
{
    if(arg1.isEmpty()){
        ui->UseFullName->setChecked(false);
        ui->UseFullName->setDisabled(true);
    }
    else{
        ui->UseFullName->setDisabled(false);
        ui->UseFullName->setChecked(true);
    }
}


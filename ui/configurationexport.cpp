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

#include "configurationexport.h"
#include "ui_configurationexport.h"
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include "core/buffer.h"
#include "ui/newconfig.h"
#include "ui/exportpreview.h"


ConfigurationExport::ConfigurationExport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationExport)
{
    ui->setupUi(this);
    PopulateForm();


}

ConfigurationExport::~ConfigurationExport()
{
    delete ui;
}

void ConfigurationExport::PopulateForm(){

    ProcessUnits(true,ui->HeaderUnit->currentIndex());
    ProcessUnits(false,ui->BodyUnit->currentIndex());

    ui->IncludeTags->setChecked(Buffer::include_tags_in_export);
    ui->RoundCorners->setChecked(Buffer::use_rounded_corners);
    ui->SystemColors->setChecked(Buffer::use_system_colors);


    // Bugfix: Hide "Include Tags" option for now. (implement in 0.5)
    ui->IncludeTags->setVisible(false);

    QFont hf(Buffer::header_font);
    QFont bf(Buffer::body_font);

    ui->HeaderFont->setCurrentFont(hf);
    ui->BodyFont->setCurrentFont(bf);

    if(Buffer::header_use_em){
        ui->HeaderUnit->setCurrentIndex(1);
        ProcessUnits(true,ui->HeaderUnit->currentIndex());
    }
    else{
        ui->HeaderUnit->setCurrentIndex(0);
        ProcessUnits(true,ui->HeaderUnit->currentIndex());
    }

    if(Buffer::body_use_em){
        ui->BodyUnit->setCurrentIndex(1);
        ProcessUnits(false,ui->BodyUnit->currentIndex());
    }
    else{
        ui->BodyUnit->setCurrentIndex(0);
        ProcessUnits(false,ui->BodyUnit->currentIndex());
    }

    for(int i=0; i<ui->HeaderSize->count(); i++){

        if(ui->HeaderSize->itemText(i)==Buffer::header_font_size){
            //cout << "Item: " << i  << endl;
            ui->HeaderSize->setCurrentIndex(i);
            break;
        }
    }

    for(int j=0; j<ui->BodySize->count(); j++){

        if(ui->BodySize->itemText(j)==Buffer::body_font_size){
            //cout << "Item: " << i  << endl;
            ui->BodySize->setCurrentIndex(j);
            break;
        }
    }

}

void ConfigurationExport::ProcessUnits(bool header, int index){

    QStringList units;

    // Set header units
    if(header){

        switch(index){

        case 0:
            units << "14" << "16" << "18" << "20" << "22" << "24" << "30";
            break;

        case 1:
            units << "2" << "2.5" << "3" << "3.5" << "4";
            break;
        }

        ui->HeaderSize->clear();
        ui->HeaderSize->addItems(units);
    }

    // Set body units
    else{
        switch(index){
            case 0:
            units << "8" << "9" << "10" << "11" << "12" << "14" << "16";
            break;

            case 1:
            units << "0.5" << "0.7" << "1" << "1.1" << "1.2" << "1.3" << "1.4" << "1.5";
            break;
        }

        ui->BodySize->clear();
        ui->BodySize->addItems(units);
    }
}



void ConfigurationExport::on_HeaderUnit_currentIndexChanged(int index)
{
    ProcessUnits(true,index);
}

void ConfigurationExport::on_BodyUnit_currentIndexChanged(int index)
{
    ProcessUnits(false,index);
}

void ConfigurationExport::GetChanges(){

    Newconfig::new_use_rounded_corners=ui->RoundCorners->isChecked();
    Newconfig::new_use_system_colors=ui->SystemColors->isChecked();
    Newconfig::new_include_tags_in_export=ui->IncludeTags->isChecked();

    Newconfig::new_header_font=ui->HeaderFont->currentText();
    Newconfig::new_body_font=ui->BodyFont->currentText();

    Newconfig::new_header_font_size=ui->HeaderSize->currentText();
    Newconfig::new_body_font_size=ui->BodySize->currentText();

    switch(ui->BodyUnit->currentIndex()){

    case 0:
        Newconfig::new_body_use_em=false;
        break;

    case 1:
        Newconfig::new_body_use_em=true;
        break;
    }

    switch(ui->HeaderUnit->currentIndex()){

    case 0:
        Newconfig::new_header_use_em=false;
        break;

    case 1:
        Newconfig::new_header_use_em=true;
        break;
    }
}


void ConfigurationExport::on_PreviewFonts_clicked()
{

    ExportPreview::header_font=ui->HeaderFont->currentText();
    ExportPreview::body_font=ui->BodyFont->currentText();
    ExportPreview::body_size=ui->BodySize->currentText();
    ExportPreview::header_size=ui->HeaderSize->currentText();

    switch(ui->BodyUnit->currentIndex()){
    case 0:
        ExportPreview::use_em_body=false;
        break;

    case 1:
        ExportPreview::use_em_body=true;
        break;
    }

    switch(ui->HeaderUnit->currentIndex()){
    case 0:
        ExportPreview::use_em_header=false;
        break;

    case 1:
        ExportPreview::use_em_header=true;
        break;
    }

    ExportPreview::use_rounded=ui->RoundCorners->isChecked();
    ExportPreview::use_system_colors=ui->SystemColors->isChecked();

    ExportPreview e(this);
    e.exec();

}

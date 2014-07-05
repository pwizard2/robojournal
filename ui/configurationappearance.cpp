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

#include "configurationappearance.h"
#include "ui_configurationappearance.h"
#include "core/buffer.h"
#include <QFont>
#include <QFileDialog>
#include <QColorDialog>
#include "ui/newconfig.h"

ConfigurationAppearance::ConfigurationAppearance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationAppearance)
{
    ui->setupUi(this);
    PopulateForm();
}

ConfigurationAppearance::~ConfigurationAppearance()
{
    delete ui;
}

void ConfigurationAppearance::on_UseTheme_clicked(bool checked)
{
    if(checked){
       ui->groupBox->setDisabled(false);

       // Bugfix: Disable background options out of the box.
       ui->ClearButton->click();
       // Bugfix:  Set system colors by default (7/4/14).
       ui->DateboxOverride->setChecked(true);
    }
    else{
        ui->groupBox->setDisabled(true);
        ui->DateboxOverride->setChecked(false);
    }
}

// Set form fields to match config data in buffer
void ConfigurationAppearance::PopulateForm(){

    // Hide this for now because the TextEdit doesn't support the background-attachment:fixed property
    ui->BGFixed->setVisible(false);

    if(Buffer::use_custom_theme){
        ui->UseTheme->click();

        QFont font(Buffer::font_face);
        ui->Font->setCurrentFont(font);

        for(int i=0; i<ui->FontSize->count(); i++){

            if(ui->FontSize->itemText(i)==Buffer::font_size){
                //cout << "Item: " << i  << endl;
                ui->FontSize->setCurrentIndex(i);
                break;
            }
        }

        ui->ColorValue->setText(Buffer::text_hexcolor);


        if(Buffer::use_background){
            ui->Entrylist_Background->setChecked(Buffer::set_tree_background);
            ui->TileBackground->setChecked(Buffer::tile_bg);
            ui->BGFixed->setChecked(Buffer::bg_is_fixed);
            ui->BGImage->setText(Buffer::background_image);
            ui->Entrylist_Background->setDisabled(false);
            ui->TileBackground->setDisabled(false);
        }
        else{
            ui->Entrylist_Background->setDisabled(true);
            ui->TileBackground->setDisabled(true);
            ui->BGFixed->setDisabled(true);
        }

        ui->DateboxOverride->setChecked(Buffer::datebox_override);
    }
    else{
        // perform a double click to disable the Custom Theme group
        ui->UseTheme->click();
        ui->UseTheme->click();

        QFont font(Buffer::font_face);
        ui->Font->setCurrentFont(font);

        for(int i=0; i<ui->FontSize->count(); i++){

            if(ui->FontSize->itemText(i)==Buffer::font_size){
                //cout << "Item: " << i  << endl;
                ui->FontSize->setCurrentIndex(i);
                break;
            }
        }

        // if Bgcolor is null, set black as default
        if((Buffer::text_hexcolor.isEmpty()) || (Buffer::text_hexcolor=="NULL")){
            Buffer::text_hexcolor="#000000";
        }

        ui->ColorValue->setText(Buffer::text_hexcolor);
        ui->BGImage->setText(Buffer::background_image);

        ui->Entrylist_Background->setChecked(Buffer::set_tree_background);
        ui->TileBackground->setChecked(Buffer::tile_bg);
        ui->DateboxOverride->setChecked(Buffer::datebox_override);
    }
}

//################################################################################################
// Background selection function
QString ConfigurationAppearance::BackgroundSelect(){
    QFileDialog f(this);
    QString file=f.getOpenFileName(this,"Choose Background Image","","Images (*.png *.jpg *.bmp *.gif)");
    return file;
}
//################################################################################################
//Function that allows the user to select a color and returns the color as RGB hex value
QString ConfigurationAppearance::ColorSelect(){

    // bugfix for 0.3 8/26/12: make the dialog load the current color when clicked
    QColor current(ui->ColorValue->text());
    QColorDialog d(this);
    d.setCurrentColor(current);
    d.setOption(QColorDialog::DontUseNativeDialog,true);
    d.exec();
    QColor  choice=d.selectedColor();
    QString color=choice.name();
    return color;
}



void ConfigurationAppearance::on_ColorButton_clicked()
{
    QString color=ColorSelect();
    color=color.toUpper();
    ui->ColorValue->setText(color);
}

void ConfigurationAppearance::on_BrowseButton_clicked()
{
    QString file=BackgroundSelect();

    if(!file.isEmpty()){
        ui->BGImage->setText(file);
        // re-enable background options
        ui->TileBackground->setEnabled(true);
        ui->Entrylist_Background->setEnabled(true);
        ui->BGFixed->setEnabled(true);
    }

}

void ConfigurationAppearance::GetChanges(){

    Newconfig::new_use_custom_theme=ui->UseTheme->isChecked();
    Newconfig::new_font_face=ui->Font->currentText();
    Newconfig::new_font_size=ui->FontSize->currentIndex();
    Newconfig::new_text_hexcolor=ui->ColorValue->text();
    Newconfig::new_background_image=ui->BGImage->text();
    Newconfig::new_bg_is_fixed=ui->BGFixed->isChecked();
    Newconfig::new_tile_bg=ui->TileBackground->isChecked();

    if(!ui->BGImage->text().isEmpty()){
        Newconfig::new_use_background=true;
    }
    else{
        Newconfig::new_use_background=false;
    }

    Newconfig::new_use_background_in_tree=ui->Entrylist_Background->isChecked();
    Newconfig::new_datebox_override=ui->DateboxOverride->isChecked();
}

void ConfigurationAppearance::on_ClearButton_clicked()
{
    ui->BGImage->clear();
    ui->TileBackground->setEnabled(false);
    ui->Entrylist_Background->setEnabled(false);
    ui->TileBackground->setChecked(false);
    ui->Entrylist_Background->setChecked(false);
    ui->BGFixed->setDisabled(true);
    ui->BGFixed->setChecked(false);
}


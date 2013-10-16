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

#include <QFileDialog>
#include "configurationeditor.h"
#include "ui_configurationeditor.h"
#include "core/buffer.h"
#include "ui/newconfig.h"
#include <iostream>
#include <QDir>
#include <QMessageBox>

ConfigurationEditor::ConfigurationEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationEditor)
{
    ui->setupUi(this);

    // hide spellcheck group box until version 0.5
    //ui->groupBox_3->setVisible(false);

    PopulateForm();

    // Will Kraft -- Bugfix 5/6/13: Temporarily disable spellcheck on Linux-based systems due to
    // the overwhelming problems I've had with it. I hope to have this fixed for good by 0.5 once
    // I've stripped out the entire spellchecker and replaced it with something better.

#ifdef __gnu_linux__
    ui->UseSpellCheck->setChecked(false);
    ui->ShowSpellingErrors->setChecked(false);
    ui->Dictionary->clear();

    ui->UseSpellCheck->setEnabled(false);
    ui->BrowseButton->setEnabled(false);
    ui->ShowSpellingErrors->setEnabled(false);
    ui->Dictionary->setEnabled(false);

    ui->UseSpellCheck->setToolTip("<p>This feature is temporarily unavailable on Linux-based operating systems.</p>");
    ui->ShowSpellingErrors->setToolTip("<p>This feature is temporarily unavailable on Linux-based operating systems.</p>");
    ui->Dictionary->setToolTip("<p>This feature is temporarily unavailable on Linux-based operating systems.</p>");
    ui->BrowseButton->setToolTip("<p>This feature is temporarily unavailable on Linux-based operating systems.</p>");

#endif

}

ConfigurationEditor::~ConfigurationEditor()
{
    delete ui;
}
//#############################################################################################################

void ConfigurationEditor::PopulateForm(){
    ui->TrimWhiteSpace->setChecked(Buffer::trim_whitespace);
    ui->ForceHyphens->setChecked(Buffer::use_html_hyphens);
    ui->ForceSmartQuotes->setChecked(Buffer::use_smart_quotes);
    ui->MiscFormatting->setChecked(Buffer::use_misc_processing);


    if(Buffer::use_custom_theme){
        ui->CustomThemeEditor->setDisabled(false);
        ui->CustomThemeEditor->setChecked(Buffer::use_custom_theme_editor);
    }
    else{
        ui->CustomThemeEditor->setDisabled(true);
        ui->CustomThemeEditor->setChecked(false);
    }

    if(Buffer::use_spellcheck){
        ui->UseSpellCheck->setChecked(true);
        ui->BrowseButton->setDisabled(false);
        ui->ShowSpellingErrors->setDisabled(false);
        ui->Dictionary->setDisabled(false);
        ui->ShowSpellingErrors->setChecked(Buffer::show_spell_errors_by_default);
        ui->Dictionary->setText(Buffer::current_dictionary);
    }
    else{
        ui->UseSpellCheck->setChecked(false);
        ui->ShowSpellingErrors->setDisabled(true);
        ui->ShowSpellingErrors->setChecked(false);
        ui->BrowseButton->setDisabled(true);
        ui->Dictionary->setDisabled(true);
        ui->Dictionary->clear();
    }
}

//#############################################################################################################
void ConfigurationEditor::GetChanges(){
    Newconfig::new_trim_whitespace=ui->TrimWhiteSpace->isChecked();
    Newconfig::new_use_html_hyphens=ui->ForceHyphens->isChecked();
    Newconfig::new_use_smart_quotes=ui->ForceSmartQuotes->isChecked();
    Newconfig::new_use_custom_theme_editor=ui->CustomThemeEditor->isChecked();
    Newconfig::new_use_spellcheck=ui->UseSpellCheck->isChecked();
    Newconfig::new_show_spell_errors_by_default=ui->ShowSpellingErrors->isChecked();
    Newconfig::new_current_dictionary=ui->Dictionary->text();
    Newconfig::new_use_misc_processing=ui->MiscFormatting->isChecked();

    // Preserve current AFF value if the user didn't make changes this session
    if(!aff_file.isEmpty()){
        Newconfig::new_current_dictionary_aff=aff_file;
    }
    else{
        Newconfig::new_current_dictionary_aff=Buffer::current_dictionary_aff;
    }
}

//#############################################################################################################
void ConfigurationEditor::on_BrowseButton_clicked()
{
    QString folderpath=QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator();

    // This code is from Jan Sundermeyer's Spellcheck program.
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Dictionary"), folderpath, tr("Dictionary (*.dic)"));

    QString new_Dict=fileName;


    //Find AFF file and save it.
    if(!new_Dict.isEmpty()){
        aff_file=Find_AFF_File(new_Dict);
    }

    if(!new_Dict.isEmpty()){
        ui->Dictionary->setText(new_Dict);
    }
    else{
        ui->UseSpellCheck->setChecked(false);
        ui->ShowSpellingErrors->setChecked(false);
        ui->BrowseButton->setEnabled(false);
        ui->ShowSpellingErrors->setEnabled(false);
        ui->Dictionary->setEnabled(false);
    }
}

//#############################################################################################################
// Find and return the AFF file for the chosen dictionary. The AFF file contains grammer rules and more importantly keeps the
// app from segfaulting on Windows.
QString  ConfigurationEditor::Find_AFF_File(QString dict){
    using namespace std;

    QString aff_file="none";

    int slashes=dict.count("/");

    QString directory=dict.section("/",0,slashes-1);
    //cout << "Directory: " << directory.toStdString() << endl;

    QString dict_name=dict.section("/",slashes);
    dict_name.chop(4);
    //cout << "Base dict name: " << dict_name.toStdString() << endl;

    dict_name=dict_name+".aff";

    QFile Aff(directory + QDir::separator() + dict_name);

    if(Aff.exists()){
        cout << "OUTPUT: Found AFF File for selected dictionary (" << Aff.fileName().toStdString() << ")" << endl;
        aff_file=Aff.fileName();

    }

    else{
        QMessageBox m;
        int h=m.critical(this,"RoboJournal","RoboJournal couldn't find the AFF file for "
                         "<b>" + dict + "</b> in the expected location. Do you want to find this file manually?",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);

        switch(h){
        case QMessageBox::Yes:
            aff_file= QFileDialog::getOpenFileName(this, tr("Select Aff File"),dict_name, tr("Aff Files (*.aff)"));
            break;

        case QMessageBox::No:
            // do nothing
            break;
        }
    }

    return aff_file;
}

//#############################################################################################################
void ConfigurationEditor::on_UseSpellCheck_clicked(bool checked)
{
    if(checked){
        ui->BrowseButton->setDisabled(false);
        ui->Dictionary->setDisabled(false);
        ui->ShowSpellingErrors->setDisabled(false);
        ui->ShowSpellingErrors->setChecked(true);

        if(Buffer::current_dictionary.isEmpty()){
            ui->BrowseButton->click();
        }
        else{
            ui->Dictionary->setText(Buffer::current_dictionary);
        }
    }
    else{
        ui->BrowseButton->setDisabled(true);
        ui->Dictionary->setDisabled(true);
        ui->ShowSpellingErrors->setDisabled(true);
        ui->ShowSpellingErrors->setChecked(false);
        ui->Dictionary->clear();
    }
}


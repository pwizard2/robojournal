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
#include "ui/customwords.h"

//#############################################################################################################
ConfigurationEditor::ConfigurationEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationEditor)
{
    ui->setupUi(this);

    // hide spellcheck group box until version 0.5
    //ui->groupBox_3->setVisible(false);

    PopulateForm();

    /*
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
    */

}

//#############################################################################################################
ConfigurationEditor::~ConfigurationEditor()
{
    delete ui;
}

//#############################################################################################################
// Linux-specific function that scans the computer for system-level dictionaries. It looks in the usual places
// like /usr/share/hunspell et. al. and adds each dictionary it finds in there to a Stringlist.
// This function is meant ot address an issue Ritesh told me about, where RoboJournal should depend
// on system-level resources instead of using its own --Will Kraft (11/1/13).

QStringList ConfigurationEditor::Scan_For_System_Dictionaries(){

    QStringList dictionaries;
    QStringList locations;

    QStringList filters;
    filters << "*.dic";

    // Add most likely install locations for dictionaries to the list.
    locations << "/usr/share/hunspell";
    //    locations << "/usr/share/myspell/dicts/";
    //    locations << "/usr/share/hyphen/";

    for(int i=0; i < locations.size(); i++){
        QString path=locations.at(i);
        QDir dir(path);
        dir.setNameFilters(filters);

        if(dir.exists()){
            QStringList batch=dir.entryList(QDir::Files | QDir::NoDotAndDotDot).replaceInStrings(QRegExp("^"),
                                                                                                 dir.absolutePath()+QDir::separator());

            dictionaries.append(batch);
        }
    }

    if(dictionaries.isEmpty()){
        QMessageBox m;
        m.critical(this,"RoboJournal","RoboJournal could not find any system-level dictionaries on your computer. To solve this problem,"
                   " you should install some Hunspell dictionaries with your distro's package management software.");
    }

    return dictionaries;
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
        ui->ManageUDWords->setDisabled(false);
        ui->Dictionary->setDisabled(false);
        ui->ShowSpellingErrors->setChecked(Buffer::show_spell_errors_by_default);
        ui->SystemLevelDic->setDisabled(false);



        if(Buffer::system_dic){
            ui->SystemLevelDic->setChecked(true);

            for(int i=0; i<ui->Dictionary->count(); i++){
                if(ui->Dictionary->itemText(i)==Buffer::current_dictionary){
                    ui->Dictionary->setCurrentIndex(i);
                    break;
                }
            }
        }
        else{
            ui->Dictionary->addItem(Buffer::current_dictionary);
        }
    }
    else{
        ui->UseSpellCheck->setChecked(false);
        ui->ShowSpellingErrors->setDisabled(true);
        ui->ShowSpellingErrors->setChecked(false);
        ui->BrowseButton->setDisabled(true);
        ui->Dictionary->setDisabled(true);
        ui->SystemLevelDic->setChecked(false);
        ui->SystemLevelDic->setDisabled(true);
        ui->ManageUDWords->setDisabled(true);
        ui->Dictionary->clear();
    }

#ifdef _WIN32
    // Disable system-level dictionaries on Windows because Windows doesn't have any system-level hunspell dictionaries by default.
    ui->SystemLevelDic->setDisabled(true);
    ui->SystemLevelDic->setChecked(false);
#endif
}

//#############################################################################################################
void ConfigurationEditor::GetChanges(){
    Newconfig::new_trim_whitespace=ui->TrimWhiteSpace->isChecked();
    Newconfig::new_use_html_hyphens=ui->ForceHyphens->isChecked();
    Newconfig::new_use_smart_quotes=ui->ForceSmartQuotes->isChecked();
    Newconfig::new_use_custom_theme_editor=ui->CustomThemeEditor->isChecked();
    Newconfig::new_use_spellcheck=ui->UseSpellCheck->isChecked();
    Newconfig::new_show_spell_errors_by_default=ui->ShowSpellingErrors->isChecked();
    Newconfig::new_current_dictionary=ui->Dictionary->currentText();
    Newconfig::new_use_misc_processing=ui->MiscFormatting->isChecked();
    Newconfig::new_system_dic=ui->SystemLevelDic->isChecked();

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

    if(!new_Dict.isEmpty()){

        aff_file=Find_AFF_File(new_Dict);
        bool itemAlreadyExists=false;

        for(int i=0; i < ui->Dictionary->count(); i++){

            QString row=ui->Dictionary->itemText(i);

            if(row==new_Dict){
                itemAlreadyExists=true;
                break;
            }
        }

        if(!itemAlreadyExists)
            ui->Dictionary->addItem(new_Dict);
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
                         "<b>" + dict + "</b> in the expected location. Do you want to find this file manually?",
                         QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);

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
        ui->SystemLevelDic->setDisabled(false);
        ui->ManageUDWords->setDisabled(false);

#ifdef __gnu_linux__
        // Have RoboJournal use system-level dictionaries by default if we are running on Linux --Will Kraft (11/1/13).
        ui->Dictionary->clear();
        ui->SystemLevelDic->click();
#endif

#ifdef _WIN32
        // Disable system-level dictionaries on Windows because Windows doesn't have any system-level Hunspell dictionaries by default.
        ui->SystemLevelDic->setDisabled(true);
        ui->SystemLevelDic->setChecked(false);

        // Add the default en_US dictionary to the list by default so users won't have to browse for it --Will Kraft (11/3/13).
        // Bugfix for Linux (11/17/13): don't add this dic if we're using system-level dictionaries. Move this code into the
        // Win32 block because Linux was still hitting it and adding the built-in dictionary even when Buffer::system_level was true.
        QFile default_en(QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator() + "en_US.dic");

        if(default_en.exists()){
            ui->Dictionary->addItem(default_en.fileName());
        }

#endif


    }
    else{
        ui->BrowseButton->setDisabled(true);
        ui->Dictionary->setDisabled(true);
        ui->ShowSpellingErrors->setDisabled(true);
        ui->ShowSpellingErrors->setChecked(false);
        ui->SystemLevelDic->setChecked(false);
        ui->SystemLevelDic->setDisabled(true);
        ui->Dictionary->clear();
        ui->ManageUDWords->setDisabled(true);
    }
}

//#############################################################################################################
void ConfigurationEditor::on_SystemLevelDic_toggled(bool checked){
    if(checked){
        ui->Dictionary->clear();
        QStringList dic=Scan_For_System_Dictionaries();
        ui->Dictionary->addItems(dic);
        ui->BrowseButton->setDisabled(true);
    }
    else{
        ui->BrowseButton->setDisabled(false);
        ui->Dictionary->clear();

        // Add the default en_US dictionary to the list by default so users won't have to browse for it --Will Kraft (11/3/13).
        QFile default_en(QDir::homePath()+ QDir::separator() + ".robojournal"+ QDir::separator() + "en_US.dic");

        if(default_en.exists()){
            ui->Dictionary->addItem(default_en.fileName());
        }
    }
}

//#############################################################################################################
// Launch custom words window (11/3/13).
void ConfigurationEditor::on_ManageUDWords_clicked()
{
    CustomWords w(this);
    w.exec();
}

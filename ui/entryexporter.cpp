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

    Class Description / Purpose:
    The EntryExporter class used to be responsible for converting database
    entries to HTML or plain text. As of RoboJournal 0.5, the EntryExporter
    is just a container for the ExportSingle/ExportMulti pages that do
    the actual export work. --Will Kraft (7/30/13).

  */


#include "ui/entryexporter.h"
#include "ui_entryexporter.h"
#include "ui/exportmulti.h"
#include "ui/exportsingle.h"
#include "ui/exportcreatedump.h"
#include <QStackedWidget>
#include "sql/mysqlcore.h"
#include "sql/sqlitecore.h"
#include <QPushButton>
#include "core/buffer.h"
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include "core/htmlcore.h"
#include <QDebug>
#include <QDateTime>
#include <QFileInfo>

#ifdef _WIN32
#include <windows.h>
#endif

EntryExporter::EntryExporter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EntryExporter)
{
    ui->setupUi(this);

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // do primary config
    PrimaryConfig();
}

QString EntryExporter::title;
QString EntryExporter::body;
QString EntryExporter::date;
QString EntryExporter::timestamp;
QString EntryExporter::id; // id of this entry (added for improved export function in 0.5).


EntryExporter::~EntryExporter()
{
    delete ui;
}

//################################################################################################
// New Primary config for ExtryExporter 0.5. --Will Kraft, 7/30/13
// This function sets up the stackedwidget and adds the "page" widgets.
void EntryExporter::PrimaryConfig(){

    stack=new QStackedWidget(this);
    stack->addWidget(single=new ExportSingle(this));
    stack->addWidget(multi=new ExportMulti(this));
    stack->addWidget(dump=new ExportCreateDump(this));

    ui->PageArea->setWidget(stack);
    ui->PageArea->setWidgetResizable(true);
    ui->PageArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->PageArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ui->buttonBox->setContentsMargins(9,0,9,9);

    // Add a nifty new "Export" button b/c the original button didn't really make sense in context.
    // The Export button now has an icon to help differentiate it from the Cancel button.
    // New for 0.5 -- Will Kraft (7/30/13).
    QIcon ex(":/icons/external.png");
    export_button=ui->buttonBox->addButton(tr("&Export"),QDialogButtonBox::AcceptRole);
    export_button->setIcon(ex);
    export_button->setDefault(true);

    // Prevent mysql dumps by hiding the Dump  database option if we are in SQLite mode.
    if(Buffer::backend=="SQLite"){
        QListWidgetItem *dump_row=ui->Menu->item(2);
        dump_row->setHidden(true);
    }

    // Keep the window from shrinking when the sizepolicy gets changed.
    ui->PageArea->setMinimumWidth(multi->width());
    ui->PageArea->setMinimumHeight(single->height());

    // Fix bug where a scrollbar appears on pages that don't need it. This code tells the stack that it's ok
    // to have different-sized widgets. The loop should start on Widget 1 because Widget 0 can have a squished
    // appearance if we set its size policy to ignore.
    for(int i=1; i<stack->count(); i++){
        stack->setCurrentIndex(i);
        stack->currentWidget()->setSizePolicy(QSizePolicy::Ignored,
                                              QSizePolicy::Ignored);
        adjustSize();
    }

    // Ensure the first page is visible when window opens.
    stack->setCurrentIndex(0);
}

//################################################################################################
// Validation function that checks to see if the file exists before doing an export operation.
// Changed to work with the new multi-paged interface in RoboJournal 0.5. (7/30/13).
bool EntryExporter::Validate(){
    using namespace std;

    this->setCursor(Qt::WaitCursor);

    HTMLCore h;
    int current_page=ui->Menu->currentRow();

    // validate single-entry export.
    if(current_page==0){

        single->HarvestValues();

        QString path=ExportSingle::path + QDir::separator() + ExportSingle::filename;
        // create an empty file object. This is just a dummy, we don't actually do anything with this here other than see if it exists.
        QFile file(path);

        //check to see if file exists already. if so, confirm before overwriting it
        if(file.exists()){
            QMessageBox c;
            int choice=c.question(this,"RoboJournal","<b>" + ExportSingle::path + "</b> already contains a file called<br><b>" +
                                  ExportSingle::filename + "</b>.<br><br>Do you want to replace the existing file?",
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

            switch(choice){
            case QMessageBox::Yes:
                h.Do_Export(path,EntryExporter::id,ExportSingle::use_html);
                this->setCursor(Qt::ArrowCursor);
                return true;
                break;

            case QMessageBox::No: // do nothing
                this->setCursor(Qt::ArrowCursor);
                return false;
                break;
            }

        }
        // $filename does not exist so just export data as $filename.
        else{
            h.Do_Export(path,EntryExporter::id,ExportSingle::use_html);
            this->setCursor(Qt::ArrowCursor);
            return true;
        }
    }

    // validate entire journal export.
    if(current_page==1){

        multi->HarvestValues(); // get the data.

        // Check to see if the journal should be exported as loose entries (true) or meged into one file (false).
        bool no_merge=ExportMulti::no_merge;

        if(no_merge){

            QDateTime t=QDateTime::currentDateTime();
            QString datestamp;
            switch(Buffer::date_format){
            case 0:
                datestamp=t.toString("dd-MM-yyyy");
                break;

            case 1:
                datestamp=t.toString("MM-dd-yyyy");
                break;

            case 2:
                datestamp=t.toString("yyyy-MM-dd");
                break;

            }

            QString type="html";

            if(!ExportMulti::use_html){
                type="txt";
            }


            QString exportpath=Buffer::database_name + "_" + type + "_" + datestamp;
            QString fullpath=ExportMulti::path + QDir::separator() + exportpath;

            //QDir parent_dir(ExportMulti::path);
            QDir newfolder(fullpath);

            //Check to see if there are any items in the folder.
            QStringList items=newfolder.entryList(QDir::AllEntries);

            bool isFolderEmpty=true;

            if(items.count() > 0){
                isFolderEmpty=false;
            }

            // Check to see if the export folder exists, create it if it does not.
            if((newfolder.exists()) && (!isFolderEmpty)){

                QMessageBox m;
                int choice=m.question(this,"RoboJournal", "<nobr><b>" + newfolder.path() +"</b></nobr> already exists and has files in it. "
                                      "Continuing the export operation will replace all items in the folder. Do you want to continue?",
                                      QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

                switch(choice){
                case QMessageBox::Yes:
                    h.Export_Loose_Journal_Entries(fullpath,ExportMulti::use_html);
                    this->setCursor(Qt::ArrowCursor);
                    return true;
                    break;

                case QMessageBox::No: // do nothing
                    this->setCursor(Qt::ArrowCursor);
                    return false;
                    break;
                }
            }

            // if the fullpath folder doesn't exist, create it and proceed with the export.
            else{

                newfolder.mkdir(fullpath);

                h.Export_Loose_Journal_Entries(fullpath,ExportMulti::use_html);
                this->setCursor(Qt::ArrowCursor);
                return true;
            }
        }
        else{

            QString path=ExportMulti::path + QDir::separator() + ExportMulti::filename;
            QFile file(path);

            if(file.exists()){
                QMessageBox c;
                int choice=c.question(this,"RoboJournal","<b>" + ExportMulti::path + "</b> already contains a file called<br><b>" +
                                      ExportMulti::filename + "</b>.<br><br>Do you want to replace the existing file?",
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

                switch(choice){

                case QMessageBox::Yes:

                    h.Export_Multi(path,ExportMulti::use_html,ExportMulti::sort_asc);
                    this->setCursor(Qt::ArrowCursor);
                    return true;
                    break;

                case QMessageBox::No: // do nothing
                    this->setCursor(Qt::ArrowCursor);
                    return false;
                    break;
                }

            }
            else{
                h.Export_Multi(path,ExportMulti::use_html,ExportMulti::sort_asc);
                this->setCursor(Qt::ArrowCursor);
                return true;
            }
        }
    }

    // This is the new MySQL dump feature, so no real validation is needed.
    if(current_page==2){
        this->setCursor(Qt::WaitCursor);

        bool getsettings=dump->HarvestData();
        bool success=false;

        if(getsettings)
            success=dump->Create_SQL_Dump(ExportCreateDump::export_path,ExportCreateDump::mysqldump_exec);

        this->setCursor(Qt::ArrowCursor);
        return success;
    }

    else{
        return false;
    }
}


//################################################################################################
// Updates static variables. This runs whenever MainWindow:: GetEntry() or MainWindow::Recent() is called.
void EntryExporter::UpdateValues(QString new_title, QString new_date, QString new_body, QString new_timestamp){
    title=new_title;
    date=new_date;
    body=new_body;
    timestamp=new_timestamp;
}

//################################################################################################

void EntryExporter::on_Menu_currentRowChanged(int currentRow)
{

    //prevent dump from working if the current journal is not local. There is no way to run MySQLDump on a remote server,
    //nor would we want to due to the nightmare security issues that would cause. --Will Kraft (1/12/14)
    if((Buffer::host !="localhost") && (Buffer::host != "127.0.0.1")){
        stack->setCurrentIndex(currentRow);

        if(currentRow==2){
            lockOKButton();
        }
        else{
            unlockOKButton();
        }
    }
    else{
        stack->setCurrentIndex(currentRow);

        QListWidgetItem *current=ui->Menu->item(currentRow);
        current->setSelected(true);
        unlockOKButton();
    }
}


//################################################################################################
void EntryExporter::accept(){

    bool proceed=Validate();

    // Close the form
    if(proceed){
        close();
    }
}

//#########################################################################################################
// Unlock the OK button to allow the user to submit the form. This also locks the
// DatabaseType object to prevent the user from switching journal types before clicking OK.
// Ported from NewJournalCreator class on 1/12/14 (Will Kraft).
void EntryExporter::unlockOKButton(){
    export_button->setEnabled(true);
    dump->setEnabled(true);

}

//#########################################################################################################
// Lock the OK button to allow the user to prevent the form from being submitted  This also unlocks the
// DatabaseType object so the user can switch the journal type if necessary.
// Ported from NewJournalCreator class on 1/12/14 (Will Kraft).
void EntryExporter::lockOKButton(){

    export_button->setEnabled(false);
    dump->setEnabled(false);

    QMessageBox m;
    m.critical(this, "RoboJournal", "RoboJournal cannot create a dump of the current journal because it is stored on a "
               "remote server  (<b>" + Buffer::host + "</b>). If you need to create a backup of this journal, you must run "
               "MySQLDump manually on the remote host.");
}

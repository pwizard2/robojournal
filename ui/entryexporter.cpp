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
    QPushButton *export_button=ui->buttonBox->addButton(tr("&Export"),QDialogButtonBox::AcceptRole);
    export_button->setIcon(ex);
    export_button->setDefault(true);

    // Prevent mysql dumps by hiding the Dump  database option if we are in SQLite mode.
    if(Buffer::backend=="SQLite"){
        QListWidgetItem *dump_row=ui->Menu->item(2);
        dump_row->setHidden(true);
    }
}

//################################################################################################
// Validation function that checks to see if the file exists before doing an export operation.
// Changed to work with the new multi-paged interface in RoboJournal 0.5. (7/30/13).
bool EntryExporter::Validate(){
    using namespace std;

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
                 return true;
                break;

            case QMessageBox::No: // do nothing
                return false;
                break;
            }

        }
        // $filename does not exist so just export data as $filename.
        else{
            h.Do_Export(path,EntryExporter::id,ExportSingle::use_html);
            return true;
        }
    }

    // validate entire journal export.
    if(current_page==1){

    }

    // This is MySQL dump feature, so no real validation is needed.
    if(current_page==2){

    }

    /*
    // Do mass export
    else{

        QMessageBox m;

        QString path=ui->ExportLocation->text() + QDir::separator() + ui->FileName_2->text();

        // create an empty file object. This is just a dummy, we don't actually do anything with this here other than see if it exists.
        QFile file(path);

        //check to see if file exists already. if so, confirm before overwriting it
        if(file.exists()){

            int choice=m.question(this,"RoboJournal","<b>" + ui->FileName_2->text() + "</b><br> already exists in <b>" +
                                  ui->ExportLocation->text() + "</b>.<br><br>Do you want to replace the existing file?",
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

            switch(choice){
            case QMessageBox::Yes:
                Mass_Export();
                break;

            case QMessageBox::No: // do nothing
                break;
            }
        }

        // file does not exist
        else{
            Mass_Export();
        }
    }
  */
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
     stack->setCurrentIndex(currentRow);

     QListWidgetItem *current=ui->Menu->item(currentRow);
     current->setSelected(true);
}


//################################################################################################
void EntryExporter::accept(){

    bool proceed=Validate();

    // Close the form
    if(proceed){
        close();
    }
}

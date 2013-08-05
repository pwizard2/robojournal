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

    Class description / purpose:
    The ExportCreateDump class allows the user to create backups of
    the current database by invoking mysqldump. The ExportCreateDump class
    is displayed as a "page" in the EntryExporter container
    class. --Will Kraft (8/3/13).
 */

#include "exportcreatedump.h"
#include "ui_exportcreatedump.h"
#include <QProcess>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include "core/buffer.h"

//################################################################################################
ExportCreateDump::ExportCreateDump(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExportCreateDump)
{
    ui->setupUi(this);
    PrimaryConfig();
}

//################################################################################################
ExportCreateDump::~ExportCreateDump()
{
    delete ui;
}

//################################################################################################

void ExportCreateDump::PrimaryConfig(){

    // OS-specific blocks that set variables depending on what the system has available.
#ifdef unix
    mysqldump_path="/usr/bin/mysqldump";
    gzip_path="/bin/gzip";
    QFile gzip(gzip_path);
    gzip_available=gzip.exists();
#endif

#ifdef _WIN32
    mysqldump_path="unset";
    gzip_available=false;

#endif

    QString greeting="RoboJournal can \"dump\" <nobr><b>" +Buffer::database_name + "</b></nobr> to a SQL backup file. The "
            "backup process preserves this journal\'s contents from the beginning up to the present time. "
            "You can use this backup to restore your journal if it is ever lost/deleted or if it needs to be "
            "migrated to a new host.";

    ui->Introduction->setText(greeting);
    ui->Introduction->setWordWrap(true);

    QString filename=setFilename();
    ui->DumpFileName->setText(filename);

    ui->AllowCustomName->setChecked(false);
    ui->DumpFileName->setReadOnly(true);

    ui->DumpPath->setText(mysqldump_path);

    // Set backup options depending on if the system has gzip installed.
    if(gzip_available){
        ui->AutoCompress->setChecked(gzip_available);
    }
    else{
        ui->AutoCompress->setChecked(false);
        ui->AutoCompress->setEnabled(false);
    }
}

//################################################################################################
// Simple function that creates and returns the filename for the dumpfile. --Will Kraft, (8/3/13).
QString ExportCreateDump::setFilename(){

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

    QString name=Buffer::database_name + "_" + datestamp + ".sql";
    return name;
}

//################################################################################################
void ExportCreateDump::on_AllowCustomName_toggled(bool checked)
{
    if(checked){
        ui->DumpFileName->setReadOnly(false);
        ui->DumpFileName->setFocus();
    }
    else{
        ui->DumpFileName->setReadOnly(true);
        ui->DumpFileName->clearFocus();
    }
}

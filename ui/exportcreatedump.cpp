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
#include <QDir>
#include <QFileDialog>
#include <iostream>
#include "core/buffer.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include "core/settingsmanager.h"

#ifdef _WIN32
#include <windows.h>
#endif

QString ExportCreateDump::export_path;
QString ExportCreateDump::mysqldump_exec;
bool ExportCreateDump::use_compress;

//################################################################################################
bool ExportCreateDump::HarvestData(){

    QString filename=ui->OutputPath->text() + QDir::separator() + ui->DumpFileName->text();

    ExportCreateDump::export_path=filename;

#ifdef _WIN32
    if(ui->DumpPath->text().isEmpty()){

        QString dump=dumpBrowse("");
        ui->DumpPath->setText(dump);

        return false;
    }
#endif

    ExportCreateDump::mysqldump_exec=ui->DumpPath->text();
    ExportCreateDump::use_compress=ui->AutoCompress->isChecked();
    return true;
}

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
QString ExportCreateDump::outputBrowse(QString current_dir)
{
    QString dir=QFileDialog::getExistingDirectory(this,tr("Select Directory"),
                                                  current_dir,QFileDialog::ShowDirsOnly);
    if(dir.isEmpty()){
        return current_dir;
    }
    else{
        return dir;
    }
}

//################################################################################################
QString ExportCreateDump::dumpBrowse(QString current_exec)
{

    QString exec;

#ifdef _WIN32
    exec=QFileDialog::getOpenFileName(this,tr("Select \'mysqldump\' Executable"),current_exec,"MySQLDump (mysqldump.exe)");
#endif

#ifdef unix
        exec=QFileDialog::getOpenFileName(this,tr("Select \'mysqldump\' Executable"),current_exec);
#endif

    if(exec.isEmpty()){
        return current_exec;
    }
    else{

#ifdef _WIN32
        exec=QDir::toNativeSeparators(exec);
        SettingsManager m;
        m.Save_Mysqldump_Path(exec);
#endif

        return exec;
    }
}

//################################################################################################
// Do the actual dump. This function is public and should be called from the EntryExporter class instead of directly;
// basically, loop the EntryExporter form back on itself. $filename refers to the complete absolute path of the output
// file while $mysqldump_path should contain the full absolute path to mysqldump. $compress tells us
// whether gzip should be used. --Will Kraft, (8/14/13).
bool ExportCreateDump::Create_SQL_Dump(QString filename, QString mysqldump_path, bool compress)
{
    using namespace std;

    /*

    QFile gzip("/bin/gzip");

    //check for gzip one last time just in case
    if(!gzip.exists()){
        cout << "OUTPUT: Warning-- /bin/gzip is not available. The dump file will NOT be compressed!" << endl;
        compress=false;
    }

#ifdef _WIN32
    compress=false;
    cout << "OUTPUT: Detected Windows operating system-- the dump file will NOT be compressed." << endl;
#endif

*/

    filename=QDir::toNativeSeparators(filename);

    QFile output(filename);
    QFile mysqldump(mysqldump_path);



    // Stop the dump process if the mysqldump executable does not exist (usually at /usr/bin/mysqldump on Unix/Linux)
    if(!mysqldump.exists()){
        QMessageBox m;
        m.critical(this,"RoboJournal","RoboJournal could not find the <i>mysqldump</i> executable at the expected location (<b>" +
                   mysqldump_path + "</b>).");
        cout << "OUTPUT: Aborting! mysqldump does not exist at " << mysqldump_path.toStdString() << endl;
        return false;
    }

    // Ask before overwrite if a dump file with the same path+name already exists.
    if(output.exists()){
        QMessageBox b;
        int choice=b.question(this,"RoboJournal","<b>" + filename + "</b> already exists. Do you want to replace it?",
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if(choice==QMessageBox::No)
            return false;
    }

    QString root_pass;
    QString database=Buffer::database_name;
    bool clickedOk=false;

    while(root_pass.isEmpty()){
        root_pass=QInputDialog::getText(this,"RoboJournal","Enter the MySQL root password for <b>"
                                        + Buffer::host + "</b>:",QLineEdit::Password,"",&clickedOk);
        if(!clickedOk)
            return false;
    }

    // complete the dump
    QProcess* dump=new QProcess();
    dump->setStandardOutputFile(filename,QIODevice::ReadWrite);

    QStringList args;
    args << "-u root ";
    args << "-p" + root_pass;
    args << " " + database;

    //cout << "cmd: " << args.join("").toStdString() << endl;

    mysqldump_path = mysqldump_path +" "+args.join("");
    //cout << mysqldump_path.toStdString() << endl;

    dump->start(mysqldump_path, QIODevice::ReadWrite);

    QMessageBox n;

//    if(output.size()){
//        n.critical(this,"RoboJournal","The backup operation was unsuccessful because the output file is empty. "
//                   "Please verify that you entered the correct root password and try again.");
//        output.deleteLater();
//        return false;
//    }
//    else{
        n.information(this,"RoboJournal","The current contents of <b>" + database + "</b> have been backed up to <b>"
                      + filename + "</b>.");
        return true;
    //}
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

    mysqldump_path=Buffer::mysqldump_path_win;

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

    QString homepath=QDir::homePath();

    // check for the presence of a ~/Documents folder on Unix. If we find one, use it.
#ifdef unix

    QString documents=homepath+"/Documents";
    QDir docs(documents);

    if(docs.exists()){
        homepath=documents;
    }

#endif

    // add special Windows pathname instructions for Documents folder. Get windows version b/c XP doesn't have a Documents folder.
#ifdef _WIN32

    DWORD dwVersion = GetVersion();
    DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));

    homepath=homepath.replace("/","\\");

    //Windows XP and older
    if(dwMajorVersion < 6){
        homepath=homepath+QDir::separator()+"My Documents";
    }
    // Windows Vista & newer
    else{
        homepath=homepath+QDir::separator()+"Documents";
    }


#endif

    ui->OutputPath->setText(homepath);

    ui->AllowCustomName->setChecked(false);
    ui->DumpFileName->setReadOnly(true);

    ui->DumpPath->setText(mysqldump_path);

    // Set backup options depending on if the system has gzip installed.
    //    if(gzip_available){
    //        ui->AutoCompress->setChecked(gzip_available);
    //    }
    //    else{
    ui->AutoCompress->setChecked(false);
    ui->AutoCompress->setVisible(false);
    //ui->AutoCompress->setEnabled(false);
    // }
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

//################################################################################################

void ExportCreateDump::on_FileBrowse_clicked()
{
    QString path=outputBrowse(ui->OutputPath->text());
    ui->OutputPath->setText(path);
}

//################################################################################################

void ExportCreateDump::on_DumpBrowse_clicked()
{
    QString dump=dumpBrowse(ui->DumpPath->text());
    ui->DumpPath->setText(dump);
}

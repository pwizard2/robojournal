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

//################################################################################################
// Derived from SQLiteJournalPage::HarvestData(). This function is used to make any necessary
// syntax changes to the Output filename (*.sql) before passing it along to MySQLDump (10/6/13).
bool ExportCreateDump::Verify_Output_FileName(){
    using namespace std;

    QRegExp test("[a-z|A-Z|0-9|_|-]*.sql");

    if(test.exactMatch(ui->DumpFileName->text())){
        return true;
    }
    else{
        QMessageBox m;
        m.information(this,"RoboJournal","The <b>Output filename</b> you specified is invalid. RoboJournal will attempt to fix it for you.");

        // Attempt to fix the filename automatically
        QString filename=ui->DumpFileName->text();

        // Give 3 tries to fix the filename before returning false
        for(int i=0; i<3; i++){
            filename=filename.simplified();
            filename=filename.replace(" ","_");
            QRegExp bad_extension(".sql.+");

            if(filename.contains(bad_extension)){
                filename=filename.remove(bad_extension);
                filename=filename.append(".sql");
            }

            ui->DumpFileName->setText(filename);

            if(test.exactMatch(filename)){
                cout << "OUTPUT: Fixed errors in file name: " << filename.toStdString() << endl;
                break;
                return true;
            }
        }

        if(!test.exactMatch(filename)){
            m.critical(this,"RoboJournal","The <b>Output filename</b> is still invalid. Please fix the filename manually (it "
                       "must end with \".sql\" and contain no spaces).");
            return false;
        }
    }

    return false;
}

//################################################################################################
// (6/3/13) Process raw filename. This function was originally part of SqliteJournalPage
// but was retrofitted for ExportCreateDump class on 10/6/13. -- Will Kraft
void ExportCreateDump::ProcessFilename(QString filename, bool valid){

    int length=filename.length()-4;

    QRegExp extension(".sql");

    // save possible regexp for filename: [a-zA-Z0-9_]*.db

    if((!filename.contains(extension)) && (!filename.isEmpty())  && (valid)){

        filename.append(".sql");

        if(length != 0){
            filename=filename.toLower();
            ui->DumpFileName->setText(filename);
            ui->DumpFileName->setCursorPosition(ui->DumpFileName->text().size()-4);

            if(filename.length()-4==0){
                ui->DumpFileName->clear();
            }

            if(filename.count(extension) > 1){
                filename.truncate(filename.length()-4);
                ui->DumpFileName->setText(filename);
                ui->DumpFileName->setCursorPosition(ui->DumpFileName->text().size()-4);
            }
        }
        else{
            ui->DumpFileName->clear();
        }
    }
}

//################################################################################################
// (6/3/13) Validation function for filename. This function was originally part of SqliteJournalPage
// but was retrofitted for ExportCreateDump class on 10/6/13. -- Will Kraft
bool ExportCreateDump::FilenameValid(QString filename){

    if(filename.isEmpty()){
        return false;
    }

    QRegExp spaces("\\s+");
    if(spaces.exactMatch(filename)){
        return false;
    }

    return true;
}

//################################################################################################
bool ExportCreateDump::HarvestData(){

    // Abort if the Output filename is bad (i.e. if it fails a regexp test). This prevents filenames with spaces
    // or forbidden characters from being created (10/6/13).
    bool good_filename=Verify_Output_FileName();
    if(!good_filename)
        return false;

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
bool ExportCreateDump::Create_SQL_Dump(QString filename, QString mysqldump_path)
{
    using namespace std;

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

    // Set dump parameters-- have the process write to the file located at $filename.
    dump->setStandardOutputFile(filename,QIODevice::ReadWrite);

#ifdef unix
    QStringList args;
    args << "-u root ";
    args << "-p" + root_pass;
    args << " " + database;

    mysqldump_path = mysqldump_path + " " + args.join("");
#endif

    // Windows requires the command to be structured differently (9/23/13). The absolute path to mysqldump.exe must be
    // enclosed within quotation marks to keep Windows from breaking up the string (thereby causing the command to fail).
#ifdef _WIN32
    QStringList args;
    args << "-uroot ";
    args << "-p" + root_pass;
    args << " " + database;

    mysqldump_path = + "\"" + mysqldump_path+ "\" " + args.join("");
#endif

    // Run the dump operation and wait for it to finish. We need to force the app to wait or the finished()
    // signal never gets triggered, causing the window to close without displaying any output (10/9/13).
    dump->start(mysqldump_path, QIODevice::ReadWrite);
    dump->waitForFinished();

    // Prevent the window from closing if the current dump operation fails (10/9/13).
    if(output.size()==0)
        return false;

    // Close the Export Content window only if the backup operation succeeded (10/9/13).
    return true;
}

//################################################################################################

void ExportCreateDump::PrimaryConfig(){

    // Declare dump as a global QProcess because we need to assign slots to it; CheckOutputFile needs
    // to run when the process is finished. --Will Kraft (10/9/13).
    dump=new QProcess();
    connect(dump, SIGNAL(finished(int)), this, SLOT(CheckOutputFile()));

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
        ui->DumpFileName->clear();
        ui->DumpFileName->setFocus();
    }
    else{
        ui->DumpFileName->setReadOnly(true);
        ui->DumpFileName->clearFocus();
        ui->DumpFileName->clear();
        // Reset original filename (10/6/13).
        ui->DumpFileName->setText(setFilename());
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

//################################################################################################
// slot that makes sure that a custom filename always has a .sql extension (10/6/13).
void ExportCreateDump::on_DumpFileName_textChanged(const QString &arg1)
{
    if(ui->AllowCustomName->isChecked()){
        QString filename=arg1;
        filename=filename.trimmed();
        filename=filename.toLower();

        bool valid=FilenameValid(filename);
        ProcessFilename(filename,valid);
    }
}

//################################################################################################
// This slot is used to see if the output operation was successful by checking the output file size.
// This MUST be triggered only after the QProcess terminates or QFile::Size() will always return 0.
// --Will Kraft (10/9/13).
void ExportCreateDump::CheckOutputFile(){

    QMessageBox n;
    QFile output(export_path);

    if(output.size() > 0){
        n.information(this,"RoboJournal","RoboJournal has successfully backed up the current contents of <b>" +
                      Buffer::database_name + "</b>.");
    }
    else{
        n.critical(this,"RoboJournal","The backup operation failed. Make sure you entered the correct "
                   "MySQL/MariaDB root password and try again.");
    }
}

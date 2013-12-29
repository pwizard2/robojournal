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

    6/23/13: This class contains/manages the joural creator page classes
    and communicates with the various SQL core classes.

*/

#include "newjournalcreator.h"
#include "ui_newjournalcreator.h"
#include <QStackedWidget>
#include "ui/journalcreatorcoverpage.h"
#include "ui_journalcreatorcoverpage.h"
#include "ui/mysqljournalpage.h"
#include "ui_mysqljournalpage.h"
#include "ui/sqlitejournalpage.h"
#include "ui_sqlitejournalpage.h"
#include <QPushButton>
#include "core/buffer.h"
#include "sql/mysqlcore.h"
#include "sql/sqlitecore.h"
#include <QCloseEvent>
#include <QMessageBox>
#include "core/settingsmanager.h"
#include "ui/firstrun.h"
#include <QDir>
#include "core/favoritecore.h"


// MySQL data
QString NewJournalCreator::journal_name;
QString NewJournalCreator::username;
QString NewJournalCreator::password;
QString NewJournalCreator::hostname;
QString NewJournalCreator::port;
QString NewJournalCreator::root_password;

// SQLite data
QString NewJournalCreator::sqlite_journal_name;
QString NewJournalCreator::sqlite_journal_path;
bool NewJournalCreator::sqlite_is_favorite;

NewJournalCreator::NewJournalCreator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewJournalCreator)
{
    ui->setupUi(this);
    PrimaryConfig();
}

//#########################################################################################################

NewJournalCreator::~NewJournalCreator()
{
    delete ui;
}

//#########################################################################################################

void NewJournalCreator::PrimaryConfig(){

    QListWidgetItem *coversheet= ui->DatabaseType->item(0);
    QListWidgetItem *sqlite_db= ui->DatabaseType->item(1);
    QListWidgetItem *mysql_db= ui->DatabaseType->item(2);

    coversheet->setToolTip("<p>Display the cover sheet/introduction page.</p>");
    sqlite_db->setToolTip("<p>Create an empty SQLite-based journal.</p>");
    mysql_db->setToolTip("<p>Create an empty MySQL-based journal.</p>");


    QPushButton *okButton=ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(false);

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Create stacked widget
    stack=new QStackedWidget(this);

    stack->addWidget(c=new JournalCreatorCoverPage(this));
    stack->addWidget(s=new SQLiteJournalPage(this));
    stack->addWidget(m=new MySQLJournalPage(this));

    ui->PageArea->setWidget(stack);
    ui->PageArea->setWidgetResizable(true);
    ui->PageArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->PageArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);


    stack->setCurrentIndex(0);
    ui->DatabaseType->setCurrentRow(0);

    // Force the new journal to be set as default if RoboJournal is in First Run mode. This guarantees that
    // there will always be a default journal no matter what.
    if(Buffer::firstrun){
        ui->SetAsDefault->setChecked(true);
        ui->SetAsDefault->setEnabled(false);
    }

    // limit how much the form width can be compressed. New for 0.5, 7/2/13.
    ui->PageArea->setMinimumWidth(s->width());

    // Certain events on the MySQL and SQLite pages muct be controlled by signals/slots because that is
    // the best way to communicate across classes.

    //The MySQL page tells this class to unlock the OK button
    connect(m, SIGNAL(unlockOK()), this, SLOT(unlockOKButton()));

    //The MySQL page tells this class to [re]lock the OK button
    connect(m, SIGNAL(unlockNotOK()), this, SLOT(lockOKButton()));

    // Clicking the Restore Defaults button in this class tells the MySQL page to reset the form if MySQL page is active
    connect(this, SIGNAL(Clear_MySQL()), m, SLOT(ClearForm()));

    //The SQLite page tells this class to unlock the OK button
    connect(s, SIGNAL(unlockOK()), this, SLOT(unlockOKButton()));

    //The SQLite page tells this class to [re]lock the OK button
    connect(s, SIGNAL(unlockNotOK()), this, SLOT(lockOKButton()));

    // Clicking the Restore Defaults button in this class tells the SQLite page to reset the form if SQLite page is active
    connect(this, SIGNAL(Clear_SQLite()), s, SLOT(ClearForm()));

    ui->buttonBox->setContentsMargins(9,3,9,9);
    ui->SetAsDefault->setContentsMargins(9,0,0,0);
}

//#########################################################################################################

//Create new SQLite journal (6/9/13)
bool NewJournalCreator::Create_SQLite_Database(){

    QFile database(sqlite_journal_path + QDir::separator() + sqlite_journal_name);
    QString path=database.fileName();
    path=QDir::toNativeSeparators(path);
    bool proceed=true;

    if(database.exists()){
        QMessageBox j;
        int choice=j.question(this,"RoboJournal","SQLite database <b>" + path + "</b> already exists. "
                              "Do you want to replace it? <br><br><b>Warning:</b> If you choose to replace the "
                              "database, the current contents will be overwritten. This action cannot be undone!",
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        switch(choice){
        case QMessageBox::Yes:
            proceed=true;
            break;

        case QMessageBox::No:
            proceed=false;
            break;
        }
    }

    bool success;

    if(proceed){
        SQLiteCore sqlite;
        success=sqlite.CreateDB(path);
    }
    else{
        return false;
    }

    QMessageBox m;
    FavoriteCore f;

    if(success){

        m.information(this,"RoboJournal","The new journal was successfully created at <b>" + path + "</b>.");
        f.SQLite_Add_to_DB(path,sqlite_is_favorite);

    }
    else{
        m.critical(this,"RoboJournal","Journal creation attempt failed.");
    }

    return success;
}

//#########################################################################################################

// Create a new mysql journal (6/1/13)
bool NewJournalCreator::Create_MySQL_Database(){

    MySQLCore mysql;
    bool success=mysql.CreateDatabase(hostname,root_password,journal_name,port,username,password);

    QMessageBox msg;
    if(success){

        int choice=msg.question(this,"RoboJournal", "RoboJournal has successfully created a new journal called  <b>" +
                                journal_name + "</b> on <b>" + hostname + "</b>.<br><br>Do you want to add <b>" +
                                journal_name + "</b> to your list of favorites?", QMessageBox::Yes | QMessageBox::No,
                                QMessageBox::Yes);

        // Post-setup: Create configuration file with new values if option is checked
        if(ui->SetAsDefault->isChecked()){

            SettingsManager sm;
            sm.NewConfig(hostname,journal_name,port,username);
        }

        // clear firstrun flag if this is the first run and it is active
        if(Buffer::firstrun){
            Buffer::firstrun=false;
        }

        // Add the new database to the list of known databases.
        FavoriteCore f;
        f.Add_to_DB(journal_name,username,hostname);

        //Set the new journal as a favorite if the user indicated so in the choice question box (8/18/13).
        if(choice==QMessageBox::Yes){
            f.setFavoritebyName(journal_name,true);
        }

        return true;
    }

    // journal creation failed...
    else{
        msg.critical(this,"RoboJournal", "Journal creation attempt on <b>" + hostname + "</b> failed. Make sure "
                     "the host is able to accept incoming connections with the settings you provided (the port and/or "
                     "root password may be incorrect).");
        return false;
    }
}

//#########################################################################################################
// This method gets called whenever the user clicks the "Restore Defaults"
// button. This sends a signal to a slot on a specific page depending on
// which item is selected in the DatabaseType list.
void NewJournalCreator::RestoreDefaults(){

    switch(ui->DatabaseType->currentRow()){
    case 0:
        // Do nothing; This never gets called because the Restore Defaults
        // button is disabled while row 0 is selected in DatabaseType.
        break;

    case 1:
        // sqlite reset
        emit Clear_SQLite();
        break;


    case 2: // MySQL reset
        emit Clear_MySQL();
        break;
    }
}

//#########################################################################################################
void NewJournalCreator::on_DatabaseType_currentRowChanged(int currentRow)
{
    stack->setCurrentIndex(currentRow);

    QPushButton *resetbutton=ui->buttonBox->button(QDialogButtonBox::RestoreDefaults);

    if(currentRow==0){
        resetbutton->setEnabled(false);
    }
    else{
        resetbutton->setEnabled(true);
    }
}

//#########################################################################################################
// Unlock the OK button to allow the user to submit the form. This also locks the
// DatabaseType object to prevent the user from switching journal types before clicking OK.
void NewJournalCreator::unlockOKButton(){

    QPushButton *okButton=ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(true);

    ui->DatabaseType->setEnabled(false);
}

//#########################################################################################################
// Lock the OK button to allow the user to prevent the form from being submitted  This also unlocks the
// DatabaseType object so the user can switch the journal type if necessary.
void NewJournalCreator::lockOKButton(){
    QPushButton *okButton=ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(false);
    ui->DatabaseType->setEnabled(true);
}

//#########################################################################################################
void NewJournalCreator::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button->text()=="Restore Defaults"){
        RestoreDefaults();
    }
}

//#########################################################################################################
// Only accept the form if everything passes validation
void NewJournalCreator::accept(){

    // MySQL
    if(ui->DatabaseType->currentRow()==2){
        bool valid=m->Validate();

        if(valid){

            //Harvest Data from MySQL Page
            m->HarvestData();

            // Create journal
            bool successful=Create_MySQL_Database();

            // Close the form
            if(successful){
                close();
            }
        }
    }

    // SQLite
    if(ui->DatabaseType->currentRow()==1){
        bool valid=s->HarvestData();

        bool successful2=false;
        if(valid){
            successful2=Create_SQLite_Database();
        }

        // Close the form
        if(successful2){
            close();
        }
    }
}

//#########################################################################################################
void NewJournalCreator::on_buttonBox_rejected()
{
    // If this is the firstrun, return to the FirstRun class. It doesn't exist anymore at this
    //point, so create a new one. No one will ever know.
    if(Buffer::firstrun){
        this->reject();
        FirstRun f;
        f.exec();
    }
    else{
        this->reject();
    }
}

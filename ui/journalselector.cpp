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

#include "ui/journalselector.h"
#include "ui_journalselector.h"
#include "sql/mysqlcore.h"
#include <QMessageBox>
#include <QStringList>
#include <QTreeWidgetItem>
#include <QAbstractButton>
#include "core/settingsmanager.h"
#include "core/buffer.h"
#include "ui/firstrun.h"
#include "ui_firstrun.h"
#include "ui/permissionmanager.h"
#include "sql/sqlshield.h"
#include "core/favoritecore.h"


JournalSelector::JournalSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JournalSelector)
{
    ui->setupUi(this);

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    int width=this->width();
    int height=this->height();
    this->setMaximumSize(width,height);
    this->setMinimumSize(width,height);

    //Disable ok button until we select something
    QPushButton *okbutton=ui->buttonBox->button(QDialogButtonBox::Ok);
    okbutton->setDisabled(true);

    ui->Browse->setDisabled(true); // Browse should only be enabled for SQLite databases
    ui->Username->setFocus();

    ui->buttonBox->setContentsMargins(9,0,9,9);

    // Select the use defaults button when the class is instantiated.
    // Bugfix 12/8/12: It should always be checked if we're in firstrun mode.
    if((Buffer::alwaysusedefaults) || (Buffer::firstrun)){
        ui->Host->setPlaceholderText("localhost");
        ui->Port->setPlaceholderText("3306");
    }

    //hide SQLite db option in version 0.5. Remove the next line during 0.6 development --Will Kraft (2/16/14).
    ui->DBType->removeItem(1);

}

//################################################################################################
// Allow the user to easily reset the form to default state. New for 0.5, 6/29/13.
void JournalSelector::ResetForm(){

    ui->DBType->setCurrentIndex(0);
    ui->Username->clear();
    ui->Password->clear();
    ui->JournalList->clear();

    ui->Host->clear();
    ui->Port->clear();

    if(Buffer::alwaysusedefaults){
        ui->Host->setPlaceholderText("localhost");
        ui->Port->setPlaceholderText("3306");
    }
}

//################################################################################################
// Validate user/pass and allows the connection to be made. Allowing blank username/pass breaks
//other database connections so it should be stopped here.
bool JournalSelector::Validate(){

    // Use placeholder text for default values in version 0.5 --Will Kraft (1/19/13).
    QString host, port;

    // Use placeholder text as default values.
    if(Buffer::alwaysusedefaults){

        host=ui->Host->text();
        if(ui->Host->text().isEmpty())
            host=ui->Host->placeholderText();

        port=ui->Port->text();

        if(ui->Port->text().isEmpty())
            port=ui->Port->placeholderText();
    }
    else{
        host=ui->Host->text();
        port=ui->Port->text();
    }

    QMessageBox b;

    bool no_Username=ui->Username->text().isEmpty();
    bool no_Password=ui->Password->text().isEmpty();
    bool no_host=host.isEmpty();
    bool no_port=port.isEmpty();

    if((no_Username) && (no_Password)){
        b.critical(this,"RoboJournal","You must enter a username and password!");
        return false;
    }
    else{
        if(no_Username){
            b.critical(this,"RoboJournal","You must enter a username!");
            ui->Username->setFocus();
            return false;
        }

        if(no_Password){

            b.critical(this,"RoboJournal","You must enter a password!");
            ui->Password->setFocus();
            return false;
        }
    }

    if(no_host){

        b.critical(this,"RoboJournal","You must enter a hostname!");
        ui->Host->setFocus();
        return false;
    }

    if(no_port){
        b.critical(this,"RoboJournal","You must enter the port that <b>Host</b> should use!");
        ui->Port->setFocus();
        return false;
    }

    return true;
}

//################################################################################################
// Forward the settings to the config file. This function has NO validation so it must be validated before it gets called!
// If not, bad things will probably happen. 
void JournalSelector::SetPreferences(){

    // get values
    QString user=ui->Username->text();
    QString host=ui->Host->text();
    QString port=ui->Port->text();

    // Break potential SQL injections so an attacker won't be able to nuke the database.
    // 0.5 Bugfix -- Will Kraft, 6/23/13
    SQLShield s;
    user=s.Break_Injections(user);
    host=s.Break_Injections(host);
    port=s.Break_Injections(port);

    QTreeWidgetItem *selected=ui->JournalList->currentItem();
    QString database=selected->text(0);

    // get confirmarmation if this !firstrun and warnings are enabled.
    if((Buffer::showwarnings) && (!Buffer::firstrun)){
        QMessageBox a;
        int choice=a.question(this,"RoboJournal","You are about to make <b>" + database + "</b> your default journal.<br><br>"
                              "This action replaces part of your current configuration with default settings."
                              " Are you sure you want to proceed?",QMessageBox::Yes | QMessageBox::No,QMessageBox::No);
        if(choice==QMessageBox::Yes){
            SettingsManager b;
            b.NewConfig(host,database,port,user);


            // Pass information to FavoriteCore --Will Kraft (1/11/14)
            FavoriteCore f;
            f.Add_to_DB(database,user,host);

            QMessageBox m;
            m.information(this,"RoboJournal", "<b>" + database + "</b> is now your default journal.");
            this->accept();
        }
        else{
            // do nothing
        }
    }

    // firstrun or blind (no confirm) mode
    else{
        SettingsManager sm;
        sm.NewConfig(host,database,port,user);
        QMessageBox m;
        m.information(this,"RoboJournal", "<b>" + database + "</b> is now your default journal.");
        this->accept();

    }
}

//################################################################################################
// Query the database to see what type of databases we have available, and return the query as a qstringlist
void JournalSelector::JournalSearch(){
    QString hostname;
    QString port;

    this->setCursor(Qt::WaitCursor);

    if(ui->Host->text().isEmpty()){
        hostname=ui->Host->placeholderText();
    }
    else{
        hostname=ui->Host->text();
    }

    if(ui->Port->text().isEmpty()){
        port=ui->Port->placeholderText();
    }
    else{
        port=ui->Port->text();
    }

    QString username=ui->Username->text();
    QString password=ui->Password->text();

    // Break potential SQL injections so an attacker won't be able to nuke the database.
    // 0.5 Bugfix -- Will Kraft, 6/23/13
    SQLShield s;
    hostname=s.Break_Injections(hostname);
    port=s.Break_Injections(port);
    username=s.Break_Injections(username);
    password=s.Break_Injections(password);

    MySQLCore m;

    QStringList journals=m.GetDatabaseList(hostname,port,username,password,false);
    journals.sort();

    CreateTree(journals);

    // (7/18/13): Add the list of databases to the list of known databases. This is so the
    // user can choose favorite databases later. New for 0.5.
    FavoriteCore f;

    for(int i=0; i < journals.size(); i++){
        QString database=journals.at(i);
        f.Add_to_DB(database,username,hostname);
    }

    this->setCursor(Qt::ArrowCursor);

}
//################################################################################################
// Create Tree
void JournalSelector::CreateTree(QStringList journals){

    ui->JournalList->clear();

    ui->JournalList->setRootIsDecorated(false);

    ui->JournalList->setHeaderLabel("Available Journals");

    QIcon server(":/icons/server.png");
    QIcon db(":/icons/database.png");

    QString parent=ui->Host->text();
    if(parent==""){
        parent=ui->Host->placeholderText();
    }

    QString user=ui->Username->text();

    QTreeWidgetItem *root=new QTreeWidgetItem(ui->JournalList);
    root->setText(0, user + "@" + parent);
    root->setIcon(0,server);
    ui->JournalList->addTopLevelItem(root);

    // Add each entry in the QStringList to the tree
    for(int i=0; i < journals.size(); i++){
        QTreeWidgetItem *item=new QTreeWidgetItem(root);
        item->setIcon(0,db);
        item->setText(0,journals[i]);
    }

    ui->JournalList->expandAll();

    if(journals.size()==0){
        ui->JournalList->clear();
    }

}
//################################################################################################

JournalSelector::~JournalSelector()
{
    delete ui;
}
//################################################################################################


void JournalSelector::on_SearchButton_clicked()
{
    bool valid=Validate();

    if(valid){
        JournalSearch();
    }
}

//################################################################################################
void JournalSelector::on_UseDefaults_clicked(bool checked)
{
    if(checked){
        ui->Host->setText("localhost");
        ui->Port->setText("3306");
        ui->Host->setReadOnly(true);
        ui->Port->setReadOnly(true);
    }
    else{
        ui->Host->setReadOnly(false);
        ui->Port->setReadOnly(false);
        ui->Port->clear();
        ui->Host->clear();
    }
}

//################################################################################################
void JournalSelector::on_JournalList_itemClicked(QTreeWidgetItem *item, int column)
{
    // only allow the OK button to be clicked when we are on a DB node
    if(item->text(column)!= ui->Username->text() + "@" + ui->Host->text()){

        QAbstractButton *ok=ui->buttonBox->button(QDialogButtonBox::Ok);
        ok->setEnabled(true);
    }
    // do not allow the user to proceed if the root node is selected
    else{
        QAbstractButton *ok=ui->buttonBox->button(QDialogButtonBox::Ok);
        ok->setDisabled(true);
    }
}

//################################################################################################
void JournalSelector::on_buttonBox_accepted()
{
    SetPreferences();
}
//################################################################################################
void JournalSelector::on_buttonBox_rejected()
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

void JournalSelector::on_JournalList_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(item->text(column)!= ui->Username->text() + "@" + ui->Host->text()){
        SetPreferences();
    }
}

void JournalSelector::on_ManagePermissions_clicked()
{
    PermissionManager m(this);

    PermissionManager::backend=ui->DBType->currentIndex();
    PermissionManager::username=ui->Username->text();
    PermissionManager::password=ui->Password->text();

    m.exec();
}

void JournalSelector::on_DBType_currentIndexChanged(int index)
{
    switch(index){
    case 0:
        ui->Browse->setEnabled(false);
        ui->ConnectionSettings->setEnabled(true);
        break;

    case 1:
        ui->Browse->setEnabled(true);
        ui->ConnectionSettings->setEnabled(false);
        break;
    }
}

//#########################################################################################################
void JournalSelector::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button->text()=="Restore Defaults"){
        ResetForm();
    }
}

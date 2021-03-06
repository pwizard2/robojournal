/*
    This file is part of RoboJournal.
    Copyright (c) 2014 by Will Kraft <pwizard@gmail.com>.


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


#include "helpviewer.h"
#include "ui_helpviewer.h"
#include <QUrl>
#include <QDir>
#include <iostream>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QToolBar>
#include "core/buffer.h"
#include <QCloseEvent>
#include <QDebug>
#include "core/settingsmanager.h"
#include <QPushButton>


//###########################################################################################################
HelpViewer::HelpViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpViewer)
{
    ui->setupUi(this);
    PrimaryConfig();

}

//###########################################################################################################
HelpViewer::~HelpViewer()
{
    delete ui;
}

//###########################################################################################################
// New for 0.5: Load documentation as raw HTML instead of using the convoluted Qt Assistant thingy.
// This was necessary because Qt Assistant was just too unreliable --Will Kraft (6/22/14).
bool HelpViewer::LoadDoc(bool force_toc){
    using namespace std;

    QString path;

#ifdef _WIN32
    path=QDir::currentPath() + "/doc/toc.html";
#endif

#ifdef unix
    path="/usr/share/doc/robojournal/toc.html";
#endif


    QFile doc(path);

    if(!doc.exists()){
        QMessageBox::critical(this,"RoboJournal","RoboJournal could not find the documentation files. If this problem persists,"
                              " try reinstalling RoboJournal. Linux users should make sure the <i>robojournal-doc</i> package "
                              "is installed correctly.");

        ui->BackButton->setDisabled(true);
        ui->forwardButton->setDisabled(true);
        ui->HomeButton->setDisabled(true);
        ui->ChangelogButton->setDisabled(false);
        return false;

    }

    else{

        // Load the last page stored in the buffer.
        if((!Buffer::helpdoc.isEmpty()) && (Buffer::helpdoc != "about:blank") && (!force_toc)){
            ui->webView->setUrl(QUrl::fromLocalFile(Buffer::helpdoc));
        }
        else{ // otherwise, load the default.
            ui->webView->setUrl(QUrl::fromLocalFile(path));
        }

        ui->BackButton->setDisabled(false);
        ui->forwardButton->setDisabled(false);
        ui->HomeButton->setDisabled(false);
        ui->ChangelogButton->setDisabled(false);
    }
    return true;
}

//###########################################################################################################
void HelpViewer::on_BackButton_clicked()
{
    ui->webView->back();
}

//###########################################################################################################


void HelpViewer::on_HomeButton_clicked()
{
    // Bugfix: Force the TOC to be shown by passing the force_toc variable as true --Will Kraft (8/10/14).
    LoadDoc(true);
}

//###########################################################################################################

void HelpViewer::on_forwardButton_clicked()
{
    ui->webView->forward();
}
//###########################################################################################################

void HelpViewer::PrimaryConfig(){

    ui->buttonBox->setContentsMargins(0,0,6,6);

    // Create Exit Documentation Button (7/20/14).
    QIcon plug(":/icons/prohibition-button.png");
    QPushButton *exit=ui->buttonBox->addButton(tr(" Exit Documentation "),QDialogButtonBox::RejectRole);
    exit->setIcon(plug);
    exit->setDefault(true);

    ui->scrollArea->setWidget(ui->webView);

    QToolBar* bar=new QToolBar(this);
    bar->setContentsMargins(0,0,0,0);
    bar->setStyleSheet("QToolBar { border: 0px }");
    bar->setIconSize(QSize(16,16));

    if(Buffer::show_icon_labels){
        ui->BackButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->forwardButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->HomeButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->ChangelogButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        QFont toolbarFont("Sans",7);
        bar->setFont(toolbarFont);
    }

    bar->addWidget(ui->BackButton);
    bar->addWidget(ui->forwardButton);
    bar->addWidget(ui->HomeButton);
    bar->addSeparator();
    bar->addWidget(ui->ChangelogButton);


    ui->webView->setContentsMargins(0,0,0,10);
    ui->line->setContentsMargins(0,10,0,10);

    QVBoxLayout* layout=new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(1);
    layout->addWidget(bar);
    //layout->addWidget(ui->webView);
    layout->addWidget(ui->scrollArea);
    layout->addWidget(ui->line);
    layout->addWidget(ui->buttonBox);
    setLayout(layout);

    bool doc_exists=LoadDoc(false);

    if(!doc_exists){
        close();
    }

    showMaximized();
}

//###########################################################################################################
void HelpViewer::on_ChangelogButton_clicked()
{

    QString log;
#ifdef _WIN32
    log=QDir::currentPath() + QDir::separator() + "changelog.xhtml";
#endif

#ifdef unix
    log="/usr/share/doc/robojournal/changelog.xhtml";
#endif

    QFile logfile(log);

    if(!logfile.exists()){
        QMessageBox m;
        m.critical(this,"RoboJournal","RoboJournal could not find the changelog file in the expected location.");
        return;
    }

    ui->webView->setUrl(QUrl::fromLocalFile(log));
}

//###########################################################################################################
void HelpViewer::closeEvent(QCloseEvent* event){

    // Save the most recently-viewed page to the config. --Will Kraft (7/20/14).
    QString current=ui->webView->url().toLocalFile();
    qDebug() << "Last viewed help: " << current;

    if((current !="about:blank")){
        SettingsManager sm;
        sm.Save_HelpDoc(current);
    }

    Buffer::helpdoc=current;

    event->accept();
}

//###########################################################################################################
void HelpViewer::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button->text()==" Exit Documentation ")
        close();

}

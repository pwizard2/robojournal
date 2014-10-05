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


#include "ui/firstrun.h"
#include "ui_firstrun.h"
#include "core/buffer.h"
#include "ui/newjournalcreator.h"
#include "ui_newjournalcreator.h"
#include "ui/journalselector.h"
#include "ui_journalselector.h"
#include <iostream>
#include <QMessageBox>
#include "core/helpcore.h"
#include <QAbstractButton>


FirstRun::FirstRun(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstRun)
{
    using namespace std;
    ui->setupUi(this);

#ifdef unix
    QIcon unixicon(":/icons/robojournal-icon.png");
    this->setWindowIcon(unixicon);

#endif

    int width=this->width();
    int height=this->height();
    this->setMaximumSize(width,height);
    this->setMinimumSize(width,height);



    // make the new journal option default
    ui->NewJournal->click();

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // 0.5 Bugfix: Change OK button to Proceed because that makes more sense in the current context  --Will Kraft (6/21/14).
    QPushButton *proceed=ui->buttonBox->addButton(tr("Proceed"),QDialogButtonBox::AcceptRole);
    proceed->setToolTip("Click here to start the selected operation.");

    // Add version number to window title. --Will Kraft (6/21/14).
    //setWindowTitle("Welcome to RoboJournal " + Buffer::version);

    //Show help button
    ui->buttonBox->addButton(QDialogButtonBox::Help);

}

FirstRun::~FirstRun()
{
    delete ui;
}




void FirstRun::Launcher(){

    this->close();

    if(ui->NewJournal->isChecked()){
        this->close();
        NewJournalCreator j;
        j.exec();
    }
    else{
        this->close();
        JournalSelector s;
        s.exec();

    }
}

void FirstRun::on_buttonBox_accepted()
{
    Launcher();
}

void FirstRun::on_buttonBox_rejected()
{
    if(Buffer::firstrun){
        // get confirmation about quitting
        QMessageBox g;
        int choice=g.question(this,"RoboJournal","You have not finished the RoboJournal configuration process. "
                              "Do you really want to quit?", QMessageBox::Yes|QMessageBox::No,QMessageBox::No);

        if(choice==QMessageBox::Yes){
            exit(0);
        }
        else{
            // do nothing
        }

    }
    else{
        this->close();
    }
}

void FirstRun::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button->text()=="Help"){
       Helpcore h;
       //h.Show_Documentation();
       h.Show_Documentation_New();
    }
}

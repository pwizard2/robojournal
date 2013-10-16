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


#include "ui/aboutrj.h"
#include "ui_aboutrj.h"
#include "core/buffer.h"


AboutRJ::AboutRJ(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutRJ)
{
    ui->setupUi(this);

#ifdef unix
    QIcon unixicon(":/icons/robojournal-icon-big.png");
    this->setWindowIcon(unixicon);

#endif

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->label_2->setText("Version " + Buffer::version);

    char my_date[] = "Build Date: " __DATE__ " "  __TIME__;

    // New for 0.4.1: set build date
    ui->BuildDate->setText(my_date);


}

AboutRJ::~AboutRJ()
{
    delete ui;
}

void AboutRJ::on_buttonBox_accepted()
{
    close();

}

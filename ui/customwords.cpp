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

    The CustomWords class provides a user interface for managing user-defined dictionary words.
    RoboJournal stores the list of these words in ~/.robojournal. --Will Kraft (11/3/13).
*/

//###################################################################################################

#include "customwords.h"
#include "ui_customwords.h"
#include <QFile>
#include <QMessageBox>
#include <QDir>
#include <QTextStream>

CustomWords::CustomWords(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomWords)
{
    ui->setupUi(this);
    PrimaryConfig();
}

//###################################################################################################
CustomWords::~CustomWords()
{
    delete ui;
}

//###################################################################################################
// Window primary config and setup (11/3/13).
void CustomWords::PrimaryConfig(){

    file_path=QDir::homePath() + QDir::separator() + ".robojournal" + QDir::separator() + "user_defined_words.txt";

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // 6/5/13: Will Kraft. (new for version 0.5): Use the big icon on Windows too. That little 16x16 icon looks like hell when
    // stretched to fit the Win 7 taskbar and alt+tab list. The shortcut icon usually compensates for that but not always; this
    // way works for sure no matter what.
    QIcon unixicon(":/icons/robojournal-icon-big.png");
    this->setWindowIcon(unixicon);

    ui->buttonBox->setContentsMargins(9,0,9,9);

    setWindowTitle("Manage User-Defined Words");

    ui->WordList->setAlternatingRowColors(true);

    // Get the list of words and load them into the form.
    QStringList words=Load_Words();

    if(words.isEmpty()){
        QMessageBox m;
        m.information(this,"RoboJournal","You have not defined any custom dictionary words yet.");
        ui->DeleteWord->setDisabled(true);
        ui->Modify->setDisabled(true);
    }

    ui->WordList->addItems(words);
}

//###################################################################################################
// Get the list of words from the user-defined word list file and return them as a QStringList (11/3/13).
QStringList CustomWords::Load_Words(){

    QStringList words;

    QFile custom_words(file_path);

    if (custom_words.open(QIODevice::ReadOnly)){

        QTextStream scan(&custom_words);
        while (!scan.atEnd())
        {
            QString line = scan.readLine();
            words << line;
        }
        custom_words.close();
    }
    return words;
}

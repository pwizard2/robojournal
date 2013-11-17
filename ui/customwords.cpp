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
#include <iostream>
#include <QInputDialog>
#include <QMessageBox>

//###################################################################################################
CustomWords::CustomWords(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomWords)
{
    ui->setupUi(this);
    PrimaryConfig();
}

//###################################################################################################
// Refresh the word list by reloading the dictionary --Will Kraft (11/17/13).
void CustomWords::RefreshWordList(){

    ui->WordList->clear();

    QStringList words=Load_Words();
    words.sort();

    if(!words.isEmpty()){
        ui->WordList->addItems(words);
        ui->WordList->setCurrentRow(0);
    }
}


//###################################################################################################
// Add a new word to the user dictionary (11/17/13).
void CustomWords::Add_Word(){
    using namespace std;

    QTemporaryFile scratch;

    bool proceed=false;
    QString new_word=QInputDialog::getText(this,"RoboJournal","Enter the new word:",QLineEdit::Normal);

    if(new_word.isEmpty()){
        return;
    }
    else{

        QMessageBox m;

        QStringList existing=Load_Words();

        if(existing.contains(new_word,Qt::CaseInsensitive)){
            m.critical(this,"RoboJournal","Your custom dictionary already contains \""+ new_word + "\".");
            return;
        }

        int choice=m.question(this,"RoboJournal","Do you really want to add \"" + new_word
                              + "\" to your dictionary?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        switch(choice){
        case QMessageBox::Yes:
            proceed=true;
            break;

        case QMessageBox::No:
            return;
            break;
        }

        // Write out a temp file with a the new word and then replace the current dictionary with the new file.
        // We have to replace the whole file because Qt doesn't offer a way to stick a single line in the middle
        // of a file.
        if(proceed){

            existing.append(new_word);
            existing.sort();

            if (scratch.open()){

                QTextStream out(&scratch);

                for(int i=0; i < existing.size(); i++){
                    QString next=existing.at(i) + "\n";
                    out << next;
                }

                scratch.close();
            }

            // overwrite the old dictionary file. Delete old file first so the copy function works.
            QFile old(file_path);
            old.remove();

            bool overwrite=scratch.copy(old.fileName());

            if(overwrite){
                cout << "OUTPUT: Successfully updated user dictionary" << endl;
            }
            else{
                QMessageBox n;
                n.critical(this, "RoboJournal","RoboJournal could not update your custom dictionary.");
            }

            RefreshWordList();
            return;
        }
    }
}

//###################################################################################################
// Remove the "selected" word from the user dictionary (11/17/13).
void CustomWords::Delete_Word(QString selected){

}

//###################################################################################################
// Change the "selected" word in the user dictionary to something else (11/17/13).
void CustomWords::Modify_Word(QString selected){

}

//###################################################################################################
CustomWords::~CustomWords()
{
    delete ui;
}

//###################################################################################################
// Window primary config and setup (11/3/13).
void CustomWords::PrimaryConfig(){
    using namespace std;

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
        cout << "OUTPUT: Your custom dictionary is empty." << endl;
        return;
    }

    ui->WordList->addItems(words);
    ui->WordList->setCurrentRow(0);
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

//###################################################################################################
void CustomWords::on_AddWord_clicked()
{
    Add_Word();
}

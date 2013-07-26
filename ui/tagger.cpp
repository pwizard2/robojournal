/*
    This file is part of RoboJournal.
    Copyright (c) 2012 by Will Kraft <pwizard@gmail.com>.
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

    CLASS DESCRIPTION: This class is basically legacy code since a built-in tagger
    was added to the Editor class in version 0.5. However, this class still has
    its uses when teamed up with the TagReminder class (where launching the full
    Editor is impractical). In version 0.5 I gave this module a facelift to
    make it look like the EditorTagManager class to prevent user confusion.
    --Will Kraft, 6/14/13

  */

#include "ui/tagger.h"
#include "ui_tagger.h"
#include <QStringList>
#include <QListIterator>
#include <QMessageBox>
#include "sql/mysqlcore.h"
#include "core/buffer.h"
#include <QListWidgetItem>
#include <QIcon>
#include <iostream>
#include <QAbstractButton>
#include <QInputDialog>
#include "core/taggingshared.h"
#include <QToolBar>
#include <QVBoxLayout>
#include <QAction>
#include <QAbstractButton>
#include <QPushButton>
#include "ui/editortagmanager.h"

QString Tagger::id_num;

//#########################################################################################################
Tagger::Tagger(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Tagger)
{
    ui->setupUi(this);
    PrimaryConfig();


}

//#########################################################################################################
Tagger::~Tagger()
{
    delete ui;
}



//#########################################################################################################
// (6/14/13): New PrimaryConfig for 0.5 and later completely changes the appearance of the Tagger
// from previous versions.

// (7/12/13): The Tagger is now just a window frame that contains an EditorTagManager object. The tagger still
// looks like it did before and this method decreases code redundancy.

void Tagger::PrimaryConfig(){

    QWidget* spacer1 = new QWidget();
    spacer1->setMinimumHeight(9);
    spacer1->setMaximumHeight(9);
    spacer1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    et=new EditorTagManager(ui->TagWidget);
    EditorTagManager::standalone_tagger=true; // true b/c this EditorTagManager is contained in the standalone Tagger interface

    QVBoxLayout *layout=new QVBoxLayout(this);

    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(ui->TagWidget);
    layout->addWidget(spacer1);
    layout->addWidget(ui->line);
    layout->addWidget(ui->buttonBox);

    this->setLayout(layout);

    int widget_height=this->height()-ui->line->height()-ui->buttonBox->height()-14;

    et->setMinimumWidth(this->width());
    et->setMaximumHeight(widget_height);
    et->setMinimumHeight(widget_height);
    et->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    int width=this->width();
    int height=this->height();
    this->setMaximumSize(width,height);
    this->setMinimumSize(width,height);

    QAbstractButton *ok=ui->buttonBox->button(QDialogButtonBox::Ok);
    ok->setDisabled(true);

    // lock the Apply button because we haven't done anything yet
    QAbstractButton *apply=ui->buttonBox->button(QDialogButtonBox::Apply);
    apply->setDisabled(true);

    // Load Tags
    et->LoadTags(Tagger::id_num);


    ui->buttonBox->setContentsMargins(9,9,9,9);

    // Allow communication between Tagger container class and EditorTagManager child widget
    connect(et, SIGNAL(Sig_UnlockTaggerApplyButton()), this, SLOT(UnlockApply()));
    connect(et,SIGNAL(Sig_LockTaggerApplyButton()), this, SLOT(LockApply()));

}

//#########################################################################################################
// Slot that unlocks the Apply button whenever the Sig_UnlockTaggerApplyButton() is emitted from EditorTagManager.
// -- Will Kraft, 7/12/13
void Tagger::UnlockApply(){
    QAbstractButton *apply=ui->buttonBox->button(QDialogButtonBox::Apply);
    apply->setEnabled(true);
}

//#########################################################################################################
// Slot that Locks the Apply button whenever the Sig_LockTaggerApplyButton() is emitted from EditorTagManager.
// -- Will Kraft, 7/26/13
void Tagger::LockApply(){
    QAbstractButton *apply=ui->buttonBox->button(QDialogButtonBox::Apply);
    apply->setEnabled(false);

    QAbstractButton *cancel=ui->buttonBox->button(QDialogButtonBox::Cancel);
    cancel->setEnabled(true);

    QAbstractButton *ok=ui->buttonBox->button(QDialogButtonBox::Ok);
    ok->setEnabled(false);
}

//#########################################################################################################
void Tagger::on_buttonBox_clicked(QAbstractButton *button)
{
    using namespace std;
    if(button==ui->buttonBox->button(QDialogButtonBox::Apply)){
        QString tag_data=et->HarvestTags();

        // unlock OK button
        QAbstractButton *ok=ui->buttonBox->button(QDialogButtonBox::Ok);
        ok->setEnabled(true);

        // lock the Apply button once we are finished with it
        QAbstractButton *apply=ui->buttonBox->button(QDialogButtonBox::Apply);
        apply->setDisabled(true);

        // Lock the Cancel button once we Apply b/c it would make no sense to cancel anymore
        QAbstractButton *cancel=ui->buttonBox->button(QDialogButtonBox::Cancel);
        cancel->setDisabled(true);

        TaggingShared ts;
        ts.SaveTags(tag_data,Tagger::id_num);
    }
}

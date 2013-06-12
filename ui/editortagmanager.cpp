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

    6/10/13: This class is the built-in tag interface for the Editor window.

*/

#include "editortagmanager.h"
#include "ui_editortagmanager.h"
#include <QToolBar>
#include <QVBoxLayout>
#include "core/taggingshared.h"
#include <QIcon>
#include <QPalette>
#include <QBrush>
#include <QColor>
#include <QListWidgetItem>
#include <QMessageBox>

EditorTagManager::EditorTagManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditorTagManager)
{
    ui->setupUi(this);
    PrimaryConfig();
}

int EditorTagManager::tag_count;

EditorTagManager::~EditorTagManager()
{
    delete ui;
}

QString EditorTagManager::GetTags(){

    QString tags;
    return tags;
}

// 6/10/13: Create toolbar and layout for this class.
void EditorTagManager::PrimaryConfig(){

    QWidget* spacer1 = new QWidget();
    spacer1->setMinimumWidth(7);
    spacer1->setMaximumWidth(7);
    spacer1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    QWidget* spacer2 = new QWidget();
    spacer2->setMinimumWidth(15);
    spacer2->setMaximumWidth(15);
    spacer2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    QWidget* spacer3 = new QWidget();
    spacer3->setMinimumWidth(7);
    spacer3->setMaximumWidth(7);
    spacer3->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    // set background and stylesheet for TagList element
    QPalette pal;
    QBrush bg=pal.light();
    QColor bgcolor=bg.color();
    ui->TagList->setStyleSheet("padding: 8px; background-color: "+ bgcolor.name() + ";");

    ui->TagCount->setText("0 tags");

    // Create toolbar.
    QToolBar *bar = new QToolBar(this);
    bar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    bar->setLayoutDirection(Qt::LeftToRight);
    bar->setContextMenuPolicy(Qt::DefaultContextMenu);

    // Populate toolbar with loose UI elements.
    bar->addWidget(ui->NewTag);
    bar->addWidget(ui->RemoveTag);
    bar->addSeparator();
    bar->addWidget(ui->AddTag);
    bar->addWidget(spacer1);
    bar->addWidget(ui->SelectTag);
    bar->addWidget(spacer3);
    bar->addWidget(ui->AvailableTags);
    bar->addWidget(spacer2);
    bar->addWidget(ui->TagCount);

    // Force everything into a vboxlayout so the tag interface stretches to fill all available space.
    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(bar,0);
    layout->addWidget(ui->TagList,0);


    this->setLayout(layout);

    CreateTagList();
}

//6/11/13: Create drop-down tag list. Use the new TaggingShared class.
void EditorTagManager::CreateTagList(){

    TaggingShared ts;
    QStringList tags=ts.TagAggregator();
    QIcon tagicon(":/icons/tag_red.png");
    int count=0;

    for(int z=1; z < tags.size(); z++){
        QString text=tags[z];
        ui->AvailableTags->addItem(tagicon,text);
        count++;
    }
}

// (6/11/13) Add currently-selected tag in ui > AvailableTags to tag list.
void EditorTagManager::AddTag(QString newtag){
    using namespace std;

    // get rid of semicolons since that is how tags are delimited in the database
    newtag=newtag.replace(";","");

    bool add_entry=true;

    int count=ui->TagList->count();

    for(int i=0; i < count; i++){

        QListWidgetItem *c=ui->TagList->item(i);
        //cout << "Loop: " << i << endl;
        if(c->text()== newtag){
            add_entry=false;
            break;
        }
    }

    if(!add_entry){
        QMessageBox m;
        m.critical(this,"RoboJournal", "This entry has already been tagged with <b>"
                   + newtag + "</b>.");
        ui->AvailableTags->clearEditText();
        ui->AvailableTags->setFocus();

    }
    else{
        // Create new ListwidgetItem
        QIcon tagicon(":/icons/tag_orange.png");
        QListWidgetItem *entry=new QListWidgetItem(tagicon,newtag);
        ui->TagList->addItem(entry);
        ui->TagList->setCurrentItem(entry);
        ui->RemoveTag->setEnabled(true);
        ui->AvailableTags->setFocus();
        ui->AvailableTags->clearEditText();
        bool add_to_list=true;

        for(int a=0; a < ui->AvailableTags->count(); a++){
            if(ui->AvailableTags->itemText(a)==newtag){
                add_to_list=false;
                break;
            }
        }

        if(add_to_list){
            ui->AvailableTags->addItem(newtag);
        }

        tag_count=ui->TagList->count();
        QString taglabel;
        if(tag_count==1){
            taglabel=" tag";
        }
        else{
            taglabel=" tags";
        }
        ui->TagCount->setText(QString::number(tag_count) + taglabel);
    }
}

// (6/11/13) Remove currently-selected tag from Tag list
void EditorTagManager::RemoveTag(){

}

void EditorTagManager::on_AddTag_clicked()
{
    QString newtag=ui->AvailableTags->currentText();
    AddTag(newtag);
}

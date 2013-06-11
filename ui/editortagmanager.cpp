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

EditorTagManager::EditorTagManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditorTagManager)
{
    ui->setupUi(this);
    PrimaryConfig();
}

EditorTagManager::~EditorTagManager()
{
    delete ui;
}

// 6/10/13: Create toolbar and layout for this class.
void EditorTagManager::PrimaryConfig(){

    QToolBar *bar = new QToolBar(this);
    bar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    bar->setLayoutDirection(Qt::LeftToRight);
    bar->setContextMenuPolicy(Qt::NoContextMenu);

    bar->addWidget(ui->NewTag);
    bar->addWidget(ui->RemoveTag);

    bar->addSeparator();

    bar->addWidget(ui->AddTag);
    bar->addWidget(ui->AvailableTags);

    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(bar,0);
    layout->addWidget(ui->TagList,0);

    this->setLayout(layout);
}

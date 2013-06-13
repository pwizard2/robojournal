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

#ifndef EDITORTAGMANAGER_H
#define EDITORTAGMANAGER_H

#include <QWidget>
#include <QMenu>

namespace Ui {
class EditorTagManager;
}

class EditorTagManager : public QWidget
{
    Q_OBJECT
    
public:
    explicit EditorTagManager(QWidget *parent = 0);
    ~EditorTagManager();
    QString GetTags();

    static int tag_count;
    
private slots:
    void on_AddTag_clicked();

    void s_addTag();

    void s_removeTag();

    void s_newTag();

    void showPopup();

private:
    Ui::EditorTagManager *ui;
    void PrimaryConfig();
    void CreateTagList();

    void AddTag(QString newtag);
    void RemoveTag();

    QMenu *contextmenu;
};

#endif // EDITORTAGMANAGER_H

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
    As of 7/12/13, it has become the universal tag interface for the entire
    application. The Tagger class used to contain a redundant copy of this
    code because it was the original tag management interface. Now, The
    Tagger is just a window frame that contains an instance of this object.
*/

#ifndef EDITORTAGMANAGER_H
#define EDITORTAGMANAGER_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QListWidgetItem>
#include <QToolBar>
#include <QTreeWidgetItem>

namespace Ui {
class EditorTagManager;
}

class EditorTagManager : public QWidget
{
    Q_OBJECT
    
public:
    explicit EditorTagManager(QWidget *parent = 0);
    ~EditorTagManager();
    QString HarvestTags();
    void LoadTags(QString id);
    static int tag_count;
    static bool standalone_tagger;

signals:

    void Sig_UnlockTaggerApplyButton();
    
private slots:

    void s_newTag();

    void s_revertTag();

    void on_NewTag_clicked();

    void on_RevertTags_clicked();

    void on_AvailableTags_itemClicked(QTreeWidgetItem *item);

private:
    Ui::EditorTagManager *ui;
    void PrimaryConfig();
    void CreateTagList();
    void DefineTag();
    void RevertTags();

    bool no_tags;
};

#endif // EDITORTAGMANAGER_H

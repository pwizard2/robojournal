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

#ifndef TAGGER_H
#define TAGGER_H

#include <QDialog>
#include <QAbstractButton>
#include "ui/editortagmanager.h"

namespace Ui {
class Tagger;
}

class Tagger : public QDialog
{
    Q_OBJECT
    
public:
    explicit Tagger(QWidget *parent = 0);
    ~Tagger();

    static QString id_num;

private slots:

    void on_buttonBox_clicked(QAbstractButton *button);
    void UnlockApply();
    void LockApply();

private:

    Ui::Tagger *ui;

    void PrimaryConfig();
    bool no_tags;
    EditorTagManager *et;

  signals:

    void Lock_Revert();

};

#endif // TAGGER_H

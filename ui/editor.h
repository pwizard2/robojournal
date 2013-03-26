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
  */

#ifndef EDITOR_H
#define EDITOR_H

#include <QDialog>
#include <QTextEdit>
#include "ui/SpellTextEdit.h"
#include "ui/highlighter.h"

namespace Ui {
    class Editor;
}

class Editor : public QDialog
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = 0);
    ~Editor();

    bool NewEntry();

    void LoadEntry(QString entry);

    void InitiateEditMode();

    bool UpdateEntry();


    static QString title;
    static QString body;
    static int month;
    static int day;
    static int year;

    SpellTextEdit *spell;
    Highlighter *high;
    QTextEdit *regular;






private slots:
    void DocumentStats();


    void on_EntryTitle_textChanged(const QString &arg1);

    void on_EntryPost_textChanged();

    void on_UndoButton_clicked();

    void on_RedoButton_clicked();



    void on_Cancel_clicked();

    void on_CutButton_clicked();

    void on_CopyButton_clicked();

    void on_PasteButton_clicked();

    void resizeEvent(QResizeEvent *);

    void on_PostEntry_clicked();

    void on_spell_textChanged();

    void on_ShowErrors_toggled(bool checked);

private:
    Ui::Editor *ui;

    bool startup;

    void SetDate();
    void setTitle(QString title);
    void PrimaryConfig();
    void Set_Editor_Fonts();
    QString WordCount(QString data);
    void ConfirmExit();
    QString Do_Post_Processing(QString rawtext, int wordcount);
    void reject();

};

#endif // EDITOR_H

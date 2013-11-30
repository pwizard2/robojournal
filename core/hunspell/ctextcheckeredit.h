/*
    This file is part of RoboJournal.
    Copyright (c) 2014 by Will Kraft <pwizard@gmail.com>.

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


#ifndef CTEXTCHECKEREDIT_H
#define CTEXTCHECKEREDIT_H

#include <QTextEdit>

#include "ctextsyntaxhighlighter.h"

class CTextCheckerEdit : public QTextEdit
{
    Q_OBJECT

public:
    CTextCheckerEdit(QWidget *parent = 0);
protected:
    void contextMenuEvent(QContextMenuEvent *event);
private slots:
    void replaceWord();
    void addToDictionary();
    void updateTextSpeller();
private:
    CTextSyntaxHighlighter *MyTextHighlighter;

};

#endif // CTEXTCHECKEREDIT_H

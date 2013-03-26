/*
    This file is part of RoboJournal.
    Copyright (c) 2013 by Will Kraft <pwizard@gmail.com>.
    MADE IN USA

    Will Kraft (11/27/12): This code was originally Jan Sundermeyer's work
    (sunderme@web.de). I made some minor changes and adapted it for
    RoboJournal >= 0.4.

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

#ifndef SPELLTEXTEDIT_H_
#define SPELLTEXTEDIT_H_

#include <QTextEdit>
#include <QAction>
#include <QContextMenuEvent>
#include "ui/hunspell/hunspell.hxx"
#include "ui/highlighter.h"



class SpellTextEdit : public QTextEdit
{
	Q_OBJECT

public:
    SpellTextEdit(QWidget *parent = 0,QString SpellDic="");
	~SpellTextEdit();
	QStringList getWordPropositions(const QString word);
	bool setDict(const QString SpellDic);

signals:
	void addWord(QString word);

protected:
	void createActions();
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
	void correctWord();
	void slot_addWord();
	void slot_ignoreWord();

private:
    enum { MaxWords = 8 };
    QAction *misspelledWordsActs[MaxWords];

    QString spell_dic;
    Hunspell *pChecker;

    QPoint lastPos;

    QStringList addedWords;






};

#endif /*SPELLTEXTEDIT_H_*/

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


#include "ctextcheckeredit.h"
#include "spellchecker.h"

#include <QMenu>
#include <QContextMenuEvent>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QCoreApplication>
#include <QMessageBox>
#include <QDir>


SpellChecker *spellChecker;
QList<QAction *> suggestionWordsList;
QAction *suggestedWord,
        *addToDict;
QString selectedWord;

CTextCheckerEdit::CTextCheckerEdit(QWidget *parent)
    : QTextEdit(parent)
{

    QIcon dict(":/icons/spellcheck_add.png");

    QString dictPath = "/usr/share/hunspell/en_US";
    QString userDict= QDir::homePath() + QDir::separator() + ".robojournal/user_defined_words.txt";
    spellChecker = new SpellChecker(dictPath, userDict);

    addToDict = new QAction(tr("Add to custom dictionary"),this);
    addToDict->setIcon(dict);

    connect(addToDict,SIGNAL(triggered()),this,SLOT(addToDictionary()));

    for (int i = 0; i < suggestionWordsList.count(); ++i)
    {
        suggestionWordsList[i] = new QAction(this);
        suggestionWordsList[i]->setVisible(false);
        connect(suggestionWordsList[i], SIGNAL(triggered()),this, SLOT(replaceWord()));
    }

    MyTextHighlighter= new CTextSyntaxHighlighter(this->document());
}

void CTextCheckerEdit::contextMenuEvent(QContextMenuEvent *event)
{
    selectedWord.clear();

    QTextCursor cursor= textCursor();
    cursor.setPosition(cursorForPosition(event->pos()).position());
    cursor.select(QTextCursor::WordUnderCursor);
    setTextCursor(cursor);

    selectedWord=cursor.selection().toPlainText();


    QMenu *editContextMenu = QTextEdit::createStandardContextMenu();

    editContextMenu->addSeparator();
    QStringList suggestions = spellChecker->suggest(selectedWord);
    QIcon replace(":/icons/spell_swap.png");

    suggestionWordsList.clear();

    for(int i=0;i<suggestions.count();i++)
    {
        suggestedWord = new QAction(this);
        suggestedWord->setText(suggestions.at(i));
        suggestedWord->setIcon(replace);
        connect(suggestedWord, SIGNAL(triggered()),this, SLOT(replaceWord()));
        suggestionWordsList.append(suggestedWord);
    }
    editContextMenu->addActions(suggestionWordsList);

    if(!cursor.selection().isEmpty())
    {
        editContextMenu->addSeparator();
        editContextMenu->addAction(addToDict);
    }

    editContextMenu->exec(event->globalPos());
}

void CTextCheckerEdit::replaceWord()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        insertPlainText(action->text());
}

void CTextCheckerEdit::addToDictionary()
{
    spellChecker->addToUserWordlist(selectedWord);
    insertPlainText(selectedWord);
}

void CTextCheckerEdit::updateTextSpeller()
{
    QTextCharFormat highlightFormat;
    highlightFormat.setUnderlineColor(QColor("red"));
    highlightFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

    // save the position of the current cursor
    QTextCursor oldCursor = textCursor();

    // create a new cursor to walk through the text
    QTextCursor cursor(document());

    // Don't call cursor.beginEditBlock(), as this prevents the rewdraw after changes to the content
    // cursor.beginEditBlock();
    while(!cursor.atEnd()) {
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
        QString word = cursor.selectedText();

        // Workaround for better recognition of words
        // punctuation etc. does not belong to words
        while(!word.isEmpty() && !word.at(0).isLetter() && cursor.anchor() < cursor.position())
        {
            int cursorPos = cursor.position();
            cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
            cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
            word = cursor.selectedText();
        }

        if(!word.isEmpty() && !spellChecker->spell(word))
        {
            QTextCursor tmpCursor(cursor);
            tmpCursor.setPosition(cursor.anchor());
            setTextCursor(tmpCursor);
            ensureCursorVisible();

            // highlight the unknown word
            QTextEdit::ExtraSelection es;
            es.cursor = cursor;
            es.format = highlightFormat;

            QList<QTextEdit::ExtraSelection> esList;
            esList << es;
            setExtraSelections(esList);

            // reset the word highlight
//            esList.clear();
            setExtraSelections(esList);

                    }
        cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
    }
    //cursor.endEditBlock();
    setTextCursor(oldCursor);
//    QMessageBox::warning(this,"wait","press OK!");
}

/*
    This file is part of RoboJournal.
    Copyright (c) 2014 by Will Kraft <pwizard@gmail.com>.

    This code was originally written by someone named "Edogen" over on
    http://www.qtcentre.org/threads/38227-QTextEdit-with-hunspell-spell-checker-and-syntax-highlighter.
    Unfortunately, there was no copyright header in the provided code.
    If you know how to get in touch with edogen (whoever he/she is), please let
    me know so I can give appropriate credit --Will Kraft (11/30/13).

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


#include "ctextsyntaxhighlighter.h"
#include "spellchecker.h"
 #include <QTextBoundaryFinder>

extern SpellChecker *spellChecker;

CTextSyntaxHighlighter::CTextSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{

}
void CTextSyntaxHighlighter::highlightBlock(const QString &text)
{
    QTextCharFormat highlightFormat;
    highlightFormat.setUnderlineColor(QColor("red"));
    highlightFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

//    qDebug(text.toAscii());

    QTextBoundaryFinder wordFinder(QTextBoundaryFinder::Word,text);
    int wordStart = 0;
    int wordLength = 0;
    QString wordValue = "";
    while (wordFinder.position() < text.length())
    {
        if (wordFinder.position()==0)
        {
            wordStart=0;
        }
        else
        {
            wordStart=wordFinder.position();
        }
        wordLength=wordFinder.toNextBoundary()-wordStart;

        wordValue=text.mid(wordStart,wordLength);

//        qDebug("pos_before:%d",wordFinder.position());
//        qDebug("->"+wordValue.toAscii()+"<-count:%d",wordValue.length());

        if(!spellChecker->spell(text.mid(wordStart,wordLength)))
            setFormat(wordStart, wordLength, highlightFormat);
    }

}

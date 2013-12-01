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


#include "spellchecker.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>

#include "hunspell.hxx"

SpellChecker::SpellChecker(const QString &dictionaryPath, const QString &userDictionary)
{
    _userDictionary = userDictionary;

    QString dictFile = dictionaryPath + ".dic";
    QString affixFile = dictionaryPath + ".aff";
    QByteArray dictFilePathBA = dictFile.toLocal8Bit();
    QByteArray affixFilePathBA = dictFile.toLocal8Bit();
    _hunspell = new Hunspell(dictFilePathBA.constData(), affixFilePathBA.constData());

    if(!_userDictionary.isEmpty()) {
        QFile userDictonaryFile(_userDictionary);
        if(userDictonaryFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&userDictonaryFile);
            for(QString word = stream.readLine(); !word.isEmpty(); word = stream.readLine())
                put_word(word);
            userDictonaryFile.close();
        } else {
            qWarning() << "User dictionary in " << _userDictionary << "could not be opened";
        }
    } else {
        qDebug() << "User dictionary not set.";
    }
}


SpellChecker::~SpellChecker()
{
    delete _hunspell;
}


bool SpellChecker::spell(const QString &word)
{
    // TODO UTF-8
    return _hunspell->spell(word.toLatin1().constData()) != 0;
}


QStringList SpellChecker::suggest(const QString &word)
{
    char **suggestWordList;

    // TODO UTF-8
    int numSuggestions = _hunspell->suggest(&suggestWordList, word.toLatin1().constData());
    QStringList suggestions;
    for(int i=0; i < numSuggestions; ++i) {
        suggestions << QString::fromLatin1(suggestWordList[i]);
        free(suggestWordList[i]);
    }
    return suggestions;
}

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

void SpellChecker::ignoreWord(const QString &word)
{
    put_word(word);
}


void SpellChecker::put_word(const QString &word)
{
    _hunspell->add(word.toLatin1().constData());
}


void SpellChecker::addToUserWordlist(const QString &word)
{
    put_word(word);
    if(!_userDictionary.isEmpty()) {
        QFile userDictonaryFile(_userDictionary);
        if(userDictonaryFile.open(QIODevice::Append)) {
            QTextStream stream(&userDictonaryFile);
            stream << word << "\n";
            userDictonaryFile.close();
        } else {
            qWarning() << "User dictionary in " << _userDictionary << "could not be opened for appending a new word";
        }
    } else {
        qDebug() << "User dictionary not set.";
    }
}

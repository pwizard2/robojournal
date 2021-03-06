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


#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QString>

class Hunspell;

class SpellChecker
{
public:
    SpellChecker(const QString &dictionaryPath, const QString &userDictionary);
    ~SpellChecker();

    bool spell(const QString &word);
    QStringList suggest(const QString &word);
    void ignoreWord(const QString &word);
    void addToUserWordlist(const QString &word);

private:
    void put_word(const QString &word);
    Hunspell *_hunspell;
    QString _userDictionary;
};

#endif // SPELLCHECKER_H

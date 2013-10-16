/*
    This file is part of RoboJournal.
    Copyright (c) 2013 by Will Kraft <pwizard@gmail.com>.
    

    Will Kraft (11/27/12): This code was originally Jan Sundermeyer's work
    (sunderme@web.de) I made some minor changes and adapted it for
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



#include "ui/SpellTextEdit.h"
#include "core/buffer.h"
#include <QTextCursor>
#include <QTextBlock>
#include <QMenu>
#include <QContextMenuEvent>
#include <QFileInfo>
#include <QTextCodec>
#include <QSettings>
#include <QTextStream>
#include <iostream>
#include <QIcon>


SpellTextEdit::SpellTextEdit(QWidget *parent,QString SpellDic)
    : QTextEdit(parent)
{
    createActions();
    // create misspell actions in context menu
    spell_dic=SpellDic.left(SpellDic.length()-4);

    QByteArray aff=Buffer::current_dictionary_aff.toLatin1();
    QByteArray dic=Buffer::current_dictionary.toLatin1();

    pChecker = new Hunspell(aff,dic);

    QFileInfo fi(SpellDic);
    if (!(fi.exists() && fi.isReadable())){
        delete pChecker;
        pChecker=0;
    }

    // get user config dictionary
    QString filePath=Buffer::current_dictionary;


    fi=QFileInfo(filePath);
    if (fi.exists() && fi.isReadable()){
        pChecker->add_dic(filePath.toLatin1());
    }
    else filePath="";


    addedWords.clear();
}



SpellTextEdit::~SpellTextEdit()
{
    using namespace std;

    QString fileName=Buffer::current_dictionary.toLatin1();

    // only scan the dictionary if the user added words.
    if(!addedWords.isEmpty()){
        Buffer::updating_dictionary=true;
        cout << "OUTPUT: Updating dictionary with new words......";


        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);

            /* Will Kraft 11/16/12
             * Use QTextStream::ReadAll because reading a dictionary line by line is excruciatingly **SLOW** and often makes
             * the app hang/stop responding long enough to make people think it crashed. It's worth it to just use the extra
             * RAM to cache the whole file. Modern computers have plenty of it anyway and the .dic is usually < 600 Kb.
             */

            QString line=in.readAll();

            if(!addedWords.contains(line)){
                addedWords << line;
            }

            file.close();
        }

        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            //std::cout << "write" << std::endl;

            QTextStream out(&file);
            QByteArray encodedString;
            QString spell_encoding=QString(pChecker->get_dic_encoding());
            QTextCodec *codec = QTextCodec::codecForName(spell_encoding.toLatin1());
            out << addedWords.count() << "\n";
            foreach(QString elem, addedWords){
                encodedString = codec->fromUnicode(elem);
                out << encodedString.data() << "\n";

            }
        }
        cout << "DONE!" << endl;
        Buffer::updating_dictionary=false;
    }
}

bool SpellTextEdit::setDict(const QString SpellDic)
{
    if(SpellDic!=""){
        //mWords.clear();
        spell_dic=SpellDic.left(SpellDic.length()-4);
        delete pChecker;

        QByteArray aff=Buffer::current_dictionary_aff.toLatin1();
        QByteArray dic=Buffer::current_dictionary.toLatin1();

        pChecker = new Hunspell(aff,dic);

    }
    else spell_dic="";

    QFileInfo fi(SpellDic);
    if (!(fi.exists() && fi.isReadable())){
        delete pChecker;
        pChecker=0;
    }

    // get user config dictionary
    QString filePath=Buffer::current_dictionary;


    fi=QFileInfo(filePath);
    if (fi.exists() && fi.isReadable()){
        pChecker->add_dic(filePath.toLatin1());
    }
    else filePath="";

    return (spell_dic!="");
}


void SpellTextEdit::createActions() {
    for (int i = 0; i < MaxWords; ++i) {
        misspelledWordsActs[i] = new QAction(this);
        misspelledWordsActs[i]->setVisible(false);
        connect(misspelledWordsActs[i], SIGNAL(triggered()), this, SLOT(correctWord()));
    }
}

void SpellTextEdit::correctWord() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString replacement = action->text();
        QTextCursor cursor = cursorForPosition(lastPos);
        //QTextCursor cursor = textCursor();
        QString zeile = cursor.block().text();
        cursor.select(QTextCursor::WordUnderCursor);
        cursor.deleteChar();
        cursor.insertText(replacement);
    }
}

void SpellTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QIcon add(":/icons/add.png");
    QIcon ignore(":/icons/delete.png");
    QIcon swap(":/icons/spell_swap.png");

    QMenu *menu = createStandardContextMenu();
    lastPos=event->pos();
    QTextCursor cursor = cursorForPosition(lastPos);
    QString zeile = cursor.block().text();
    int pos = cursor.columnNumber();
    int end = zeile.indexOf(QRegExp("\\W+"),pos);
    int begin = zeile.lastIndexOf(QRegExp("\\W+"),pos);
    zeile=zeile.mid(begin+1,end-begin-1);
    QStringList liste = getWordPropositions(zeile);



    if (!liste.isEmpty())
    {
        menu->addSeparator();
        QAction *a;
        a = menu->addAction(add, tr("Add word"), this, SLOT(slot_addWord()));
        a = menu->addAction(ignore, tr("Ignore all"), this, SLOT(slot_ignoreWord()));
        menu->addSeparator();
        //menu->addAction(tr("Suggestions:"));
        for (int i = 0; i < qMin(int(MaxWords),liste.size()); ++i) {
            misspelledWordsActs[i]->setText(liste.at(i).trimmed());
            misspelledWordsActs[i]->setVisible(true);
            misspelledWordsActs[i]->setIcon(swap);
            menu->addAction(misspelledWordsActs[i]);
        }

    }

//    else
//    {
//        // let the user add the word if it's mispelled but not already in dict. ***BROKEN, DO NOT USE***
//       QByteArray encodedString;
//        QString spell_encoding=QString(pChecker->get_dic_encoding());
//       QTextCodec *codec = QTextCodec::codecForName(spell_encoding.toLatin1());
//       encodedString = codec->fromUnicode(cursor.block().text());
//       int check = pChecker->spell(zeile.toLatin1());

//       bool pass(check);

//        if(pass){
//            QAction *a;
//            menu->addSeparator();
//            a = menu->addAction(add, tr("Add word"), this, SLOT(slot_addWord()));
//            a = menu->addAction(ignore, tr("Ignore all"), this, SLOT(slot_ignoreWord()));
//            menu->addSeparator();
//            menu->addAction("No spelling suggestions");
//        }

//    }

    // if  misspelled
    menu->exec(event->globalPos());
    delete menu;

}

QStringList SpellTextEdit::getWordPropositions(const QString word)
{
    QStringList wordList;
    if(pChecker){
        QByteArray encodedString;
        QString spell_encoding=QString(pChecker->get_dic_encoding());
        QTextCodec *codec = QTextCodec::codecForName(spell_encoding.toLatin1());
        encodedString = codec->fromUnicode(word);
        bool check = pChecker->spell(encodedString.data());
        if(!check){
            char ** wlst;
            int ns = pChecker->suggest(&wlst,encodedString.data());
            if (ns > 0)
            {
                for (int i=0; i < ns; i++)
                {
                    wordList.append(codec->toUnicode(wlst[i]));
                    //free(wlst[i]);
                }
                //free(wlst);
                pChecker->free_list(&wlst, ns);
            }// if ns >0
        }// if check
    }
    return wordList;
}

void SpellTextEdit::slot_addWord()
{
    QTextCursor cursor = cursorForPosition(lastPos);
    QString zeile = cursor.block().text();
    int pos = cursor.columnNumber();
    int end = zeile.indexOf(QRegExp("\\W+"),pos);
    int begin = zeile.left(pos).lastIndexOf(QRegExp("\\W+"),pos);
    zeile=zeile.mid(begin+1,end-begin-1);

    QByteArray encodedString;
    QString spell_encoding=QString(pChecker->get_dic_encoding());
    QTextCodec *codec = QTextCodec::codecForName(spell_encoding.toLatin1());
    encodedString = codec->fromUnicode(zeile);
    pChecker->add(encodedString.data());
    addedWords.append(zeile);

    emit addWord(zeile);
}

void SpellTextEdit::slot_ignoreWord()
{
    QTextCursor cursor = cursorForPosition(lastPos);
    QString zeile = cursor.block().text();
    int pos = cursor.columnNumber();
    int end = zeile.indexOf(QRegExp("\\W+"),pos);
    int begin = zeile.left(pos).lastIndexOf(QRegExp("\\W+"),pos);
    zeile=zeile.mid(begin+1,end-begin-1);
    QByteArray encodedString;
    QString spell_encoding=QString(pChecker->get_dic_encoding());
    QTextCodec *codec = QTextCodec::codecForName(spell_encoding.toLatin1());
    encodedString = codec->fromUnicode(zeile);
    pChecker->add(encodedString.data());
    emit addWord(zeile);

}



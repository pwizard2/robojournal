/*
    This file is part of RoboJournal.
    Copyright (c) 2013 by Will Kraft <pwizard@gmail.com>.
    MADE IN USA

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



/****************************************************************************
 ** ORIGINAL HEADER:
 **
 ** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
 **
 ** This file is part of the example classes of the Qt Toolkit.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by Trolltech ASA
 ** (or its successors, if any) and the KDE Free Qt Foundation. In
 ** addition, as a special exception, Trolltech gives you certain
 ** additional rights. These rights are described in the Trolltech GPL
 ** Exception version 1.1, which can be found at
 ** http://www.trolltech.com/products/qt/gplexception/ and in the file
 ** GPL_EXCEPTION.txt in this package.
 **
 ** Please review the following information to ensure GNU General
 ** Public Licensing requirements will be met:
 ** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
 ** you are unsure which license is appropriate for your use, please
 ** review the following information:
 ** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
 ** or contact the sales department at sales@trolltech.com.
 **
 ** In addition, as a special exception, Trolltech, as the sole
 ** copyright holder for Qt Designer, grants users of the Qt/Eclipse
 ** Integration plug-in the right for the Qt/Eclipse Integration to
 ** link to functionality provided by Qt Designer and its related
 ** libraries.
 **
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
 ** granted herein.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

// Will Kraft (11/27/12): This code was originally Jan Sundermeyer's work. I made some minor changes
// and adapted it for RoboJournal > =0.4.

#include <QtGui>
#include <iostream>
#include "ui/highlighter.h"
#include "core/buffer.h"

Highlighter::Highlighter(QTextDocument *parent,QString SpellDic,bool spellCheckState) :
	QSyntaxHighlighter(parent) {
	HighlightingRule rule;


	spellCheckFormat.setUnderlineColor(QColor(Qt::red));
	spellCheckFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

	//Settings for online spellchecking
    if(!SpellDic.isEmpty()){
		//mWords.clear();
		spell_dic=SpellDic.left(SpellDic.length()-4);

                pChecker = new Hunspell(Buffer::current_dictionary_aff.toLatin1(),Buffer::current_dictionary.toLatin1());
		spell_encoding=QString(pChecker->get_dic_encoding());
		codec = QTextCodec::codecForName(spell_encoding.toLatin1());

		QFileInfo fi(SpellDic);
		if (fi.exists() && fi.isReadable()) spellCheckActive=true;
		else spellCheckActive=false;
		// get user config dictionary

        QString filePath=Buffer::current_dictionary;

        //std::cout << qPrintable(filePath) << std::endl;
		fi=QFileInfo(filePath);
		if (fi.exists() && fi.isReadable()){
			pChecker->add_dic(filePath.toLatin1());
		}
                else filePath="";
	}
	else spellCheckActive=false;
	spellerError=!spellCheckActive;
	spellCheckActive=spellCheckActive && spellCheckState;
}

Highlighter::~Highlighter() {
	delete pChecker;
}


void Highlighter::highlightBlock(const QString &text) {

	 spellCheck(text);
}

void Highlighter::enableSpellChecking(const bool state)
{
	bool old=spellCheckActive;
	if(!spellerError) spellCheckActive=state;
	if(old!=spellCheckActive) rehighlight();
}

void Highlighter::spellCheck(const QString &text)
{
	if (spellCheckActive) {
		// split text into words
		QString str = text.simplified();
		if (!str.isEmpty()) {

            // Will Kraft 11/27/12: this is Jan's old regex for Checkliste: ([^\\w,^\\\\]|(?=\\\\))+
            // I had to change the regexp because it was flagging contractions (didn't, couldn't, etc.)
            QStringList Checkliste = str.split(QRegExp("([^\\w,^\\\\,']|(?=\\\\))+"),
					QString::SkipEmptyParts);
			int l,number;
			// check all words
			for (int i=0; i<Checkliste.size(); ++i) {
				str = Checkliste.at(i);
				if (str.length()>1 &&!str.startsWith('\\'))
				{
                                        if (!checkWord(str)) {
						number = text.count(QRegExp("\\b" + str + "\\b"));
						l=-1;
						// underline all incorrect occurences of misspelled word
						for (int j=0;j < number; ++j)
						{
							l = text.indexOf(QRegExp("\\b" + str + "\\b"),l+1);
							if (l>=0)
								setFormat(l, str.length(), spellCheckFormat);



						} // for j
					} // if spell chek error
				} // if str.length > 1
			} // for
		} // if str.isEmpty
	} // initial check
}

bool Highlighter::checkWord(QString word)
{
	int check;
	/*switch(check=mWords.value(word,-1)){
	case -1:
    {
		QByteArray encodedString;
		encodedString = codec->fromUnicode(word);
		check = pChecker->spell(encodedString.data());
		mWords[word]=check;
		break;
    }
	default:
		break;
	}
        */
        QByteArray encodedString;
	encodedString = codec->fromUnicode(word);
	check = pChecker->spell(encodedString.data());
	return bool(check);
}

bool Highlighter::setDict(const QString SpellDic)
{
	bool spell;
	if(SpellDic!=""){
		//mWords.clear();
		spell_dic=SpellDic.left(SpellDic.length()-4);
		delete pChecker;
                pChecker = new Hunspell(Buffer::current_dictionary_aff.toLatin1(),Buffer::current_dictionary.toLatin1());

		spell_encoding=QString(pChecker->get_dic_encoding());
		codec = QTextCodec::codecForName(spell_encoding.toLatin1());

		QFileInfo fi(SpellDic);

		if (fi.exists() && fi.isReadable()) spell=true;
		else spell=false;

		// get user config dictionary

        QString filePath=Buffer::current_dictionary;

        //std::cout << qPrintable(filePath) << std::endl;
		fi=QFileInfo(filePath);
		if (fi.exists() && fi.isReadable()){
			pChecker->add_dic(filePath.toLatin1());
		}
		else filePath="";

		spellCheckFormat.setForeground(Qt::red);//faster Cursoroperation ...
		//spellCheckFormat.setUnderlineColor(QColor(Qt::red));
		//spellCheckFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
	}
	else spell=false;
	spellerError=!spell;
	spellCheckActive=spellCheckActive && spell;
	rehighlight();
	return spell;
}

void Highlighter::slot_addWord(QString word)
{
    //std::cout << qPrintable(word) << std::endl;
    QByteArray encodedString;
    QString spell_encoding=QString(pChecker->get_dic_encoding());
    QTextCodec *codec = QTextCodec::codecForName(spell_encoding.toLatin1());
    encodedString = codec->fromUnicode(word);
    pChecker->add(encodedString.data());
    rehighlight();
}

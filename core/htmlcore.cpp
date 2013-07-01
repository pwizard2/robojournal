/*
    This file is part of RoboJournal.
    Copyright (c) 2013 by Will Kraft <pwizard@gmail.com>.
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

    Class description/purpose (6/13/13), --Will Kraft:
    The HTMLCore class contains all methods that are required to turn raw Editor
    class output into a viable HTML-based entry that can be stored in a database.
    Once processed, the HTMLCore output is ready to be fed to MySQLCore or SQLiteCore.
*/

#include "htmlcore.h"
#include <QRegExp>
#include <QStringList>
#include "core/buffer.h"

HTMLCore::HTMLCore()
{
}

//#############################################################################################################
// This function takes the raw output from the Editor and strips out the extra HTML so it can be stored
// in the database. The part that needs to be stripped out is everything except what is between the <body> tags.
// (6/30/13).
QString HTMLCore::ProcessEntryFromEditor(QString rawtext){

}

//#############################################################################################################
// This function applies Post-processing features to the body text. This function used to be part of the Editor
// class but it got moved to the HTMLCore in version 0.5. (6/13/13)
QString HTMLCore::Do_Post_Processing(QString rawtext, int wordcount){
    using namespace std;

    QString body=rawtext;


    // REMINDER: Escape all regexp character classes (\\w) because Qt uses backslash-style regexp;
    // the compiler always eats the first backslash so stuff like "\w" won't work.

    // replace -- with &mdash;

    if(Buffer::use_html_hyphens){

        // replace 2 >= adjacent dashes with a clean mdash
        body=body.replace(QRegExp("\\s*-{2,}\\s*"),"&mdash;");



        //use En Dash for digits
        QRegExp shortdash("((\\d+)-(\\d+)(-+)(\\d+))|((\\d+)-(\\d+))");
        QStringList digits;

        int d_count = 0;
        int d_pos = 0;
        while ((d_pos = shortdash.indexIn(body, d_pos)) != -1) {
            ++d_count;
            d_pos += shortdash.matchedLength();
            digits << shortdash.cap(0);
        }

        for(int d=0; d < digits.size(); d++){
            QString orig=digits.at(d);
            //cout << "Match: " << open_match.at(i).toStdString() << endl;
            QString digit=orig.replace("-","&ndash;");
            //cout << fixedquote.toStdString() << endl;
            body=body.replace(digits.at(d),digit);
        }
    }

    // replace straight quotes with curvy quotes...this just looks better.
    // This works by using Regular Expressions to extract quotes from the body text and replace them with
    // HTML-friendly quotes.

    if(Buffer::use_smart_quotes){

        QRegExp openquotes("\"\\w", Qt::CaseInsensitive);
        QStringList open_match;

        int o_count = 0;
        int o_pos = 0;
        while ((o_pos = openquotes.indexIn(body, o_pos)) != -1) {
            ++o_count;
            o_pos += openquotes.matchedLength();
            open_match << openquotes.cap(0);

        }

        for(int i=0; i < open_match.size(); i++){
            QString orig=open_match.at(i);
            //cout << "Match: " << open_match.at(i).toStdString() << endl;
            QString fixedquote=orig.replace("\"","&ldquo;");
            //cout << fixedquote.toStdString() << endl;
            body=body.replace(open_match.at(i),fixedquote);
        }

        QRegExp closequotes("\\w[?!.:,;]?\"", Qt::CaseInsensitive);
        QStringList close_match;

        int c_count = 0;
        int c_pos = 0;
        while ((c_pos = closequotes.indexIn(body, c_pos)) != -1) {
            ++c_count;
            c_pos += closequotes.matchedLength();
            close_match << closequotes.cap(0);
        }

        for(int j=0; j < close_match.size(); j++){
            QString orig=close_match.at(j);
            //cout << "Match: " << close_match.at(j).toStdString() << endl;
            QString fixedquote=orig.replace("\"","&rdquo;");
            //cout << fixedquote.toStdString() << endl;
            body=body.replace(close_match.at(j),fixedquote);
        }


        //Do apostrophes and single quotes in the middle of words.
        QRegExp apos("(\\w+'\\w+'?\\w*)|(\\w+')",Qt::CaseInsensitive);
        QStringList apos_match;

        int a_count = 0;
        int a_pos = 0;
        while ((a_pos = apos.indexIn(body, a_pos)) != -1) {
            ++a_count;
            a_pos += apos.matchedLength();
            apos_match << apos.cap(0);
        }

        for(int b=0; b < apos_match.size(); b++){
            QString orig=apos_match.at(b);
            //cout << "Match: " << apos_match.at(b).toStdString() << endl;
            QString fixedquote=orig.replace("\'","&rsquo;");
            //cout << fixedquote.toStdString() << endl;
            body=body.replace(apos_match.at(b),fixedquote);
        }
    }



    // clean up extra whitespace if the feature is enabled in config AND wordcount is less than 1000. It's a totally arbitrary
    //  amount that may be too low but the program hangs if the entry is too long.
    if((Buffer::trim_whitespace) && (wordcount < 1000)){

        // clean up extra linebreaks between paragraphs. QTextEdit usually uses \n (even on windows) but look for \r\n too
        // just to be safe.
        QRegExp linebreak("[\\r\\n]+|[\\n]+", Qt::CaseInsensitive);
        QStringList break_match;
        int w_count = 0;
        int w_pos = 0;
        while ((w_pos = linebreak.indexIn(body, w_pos)) != -1) {
            ++w_count;
            w_pos += linebreak.matchedLength();
            break_match << linebreak.cap(0);
        }

        for(int b=0; b < break_match.size(); b++){

            body=body.replace(break_match.at(b),"\n\n");
        }

        // Remove redundant spaces between words. This isn't noticible in HTML mode but it does interfere
        // with the editor and plain-text exporting. We're using a Regex and not QString::simplified b/c
        // in addition to spaces QString::simplified() also eats the line breaks we want to keep.
        QRegExp spaces("[ ]+[ ]+");
        QStringList space_match;

        int s_count = 0;
        int s_pos = 0;
        while ((s_pos = spaces.indexIn(body, s_pos)) != -1) {
            ++s_count;
            s_pos += spaces.matchedLength();
            space_match << spaces.cap(0);
        }

        for(int c=0; c < space_match.size(); c++){
            body=body.replace(space_match.at(c)," "); // replace the regexp matches with a single space
        }


        // Remove trailing whitespace from the end of the body text.
        while(body.endsWith("\n")){
            body=body.remove(body.length()-2,2);
        }
    }


    // Apply miscellaneous formatting options if config specifies it. Mostly this is cleaning up after sloppy writers.
    if(Buffer::use_misc_processing){

        // get rid of extra periods (.. or . .) but NOT those intended to be ellipsis. DO NOT USE--- BROKEN!
        //body=body.replace(QRegExp("(( )?(\\.)( )*(\\.))|(( )(\\.))"),". ");

        // replace 3 >= adjacent dots with ellipsis char.
        body=body.replace(QRegExp("(\\.)(\\.+)(\\s*)"),"&hellip;");

        // apply superscript to the "st" in 1st, etc.
        QRegExp notation("\\d+(st|nd|rd|th)",Qt::CaseInsensitive);
        QStringList notation_match;

        QRegExp notation_th("th",Qt::CaseInsensitive);
        QRegExp notation_rd("rd",Qt::CaseInsensitive);
        QRegExp notation_nd("nd",Qt::CaseInsensitive);
        QRegExp notation_st("st",Qt::CaseInsensitive);

        int n_count = 0;
        int n_pos = 0;
        while ((n_pos = notation.indexIn(body, n_pos)) != -1) {
            ++n_count;
            n_pos += notation.matchedLength();
            notation_match << notation.cap(0);
            //cout << "Found match: " << notation.cap(0).toStdString() << endl;
        }

        for(int n=0; n < notation_match.size(); n++){
            QString next=notation_match.at(n);
            next=next.toLower();
            //cout << "Next: " << next.toStdString() << endl;
            QString fixed;

            if(next.contains(notation_st)){
                fixed=next.section("",0,next.length()-2);
                fixed=fixed + "<sup>st</sup>";
            }

            if(next.contains(notation_nd)){
                fixed=next.section("",0,next.length()-2);
                fixed=fixed + "<sup>nd</sup>";
            }

            if(next.contains(notation_rd)){
                fixed=next.section("",0,next.length()-2);
                fixed=fixed + "<sup>rd</sup>";
            }

            if(next.contains(notation_th)){
                fixed=next.section("",0,next.length()-2);
                fixed=fixed + "<sup>th</sup>";
            }

            body=body.replace(next,fixed);
        }
    }

    return body;
}

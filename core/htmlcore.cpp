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
    The HTMLCore also converts raw database output into a HTML document each time
    an entry needs to be displayed.

    Update 7/30/13: The HTMLCore now contains the code that outputs HTML or plain text
    from the database.
*/

#include "htmlcore.h"
#include <QRegExp>
#include <QStringList>
#include "core/buffer.h"
#include "sql/mysqlcore.h"
#include <QFile>
#include <QPalette>
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QMessageBox>

HTMLCore::HTMLCore()
{
}


//############################################################################################################
// This public function takes the raw database output array (QList<QString>) and builds a full html document to use
// in the new WebKit-based Entry Output Pane. Whenever RJ needs to display an entry, this function should be
// called INSTEAD of invoking any of the SQL cores directly. QString id corresponds to DB row number. Planned for 0.6. (7/4/13).
QString HTMLCore::AssembleEntry(QString id){

    QString title="title goes here";
    QString background="000000";
    QString foreground="FFFFFF";
    QString author=Buffer::full_name;
    QString date="today";
    QString time="12:00 pm";


    QStringList assembler;
    assembler.append("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">");
    assembler.append("<html>");
    assembler.append("<head>");
    assembler.append("<title>" + title + "</title>");
    assembler.append("</head>");
    assembler.append("<body>");
    assembler.append("<h1>" + title + "</h1>");
    assembler.append("<div style=\"width: 100%; background: #" + background + "; color:" +
                     foreground + "; border: solid 1px black; margin-bottom: 10px;\">");

    assembler.append("<small>On " + date + " at " + time + ", " + author + " wrote:</small>");
    assembler.append("</div>");

    assembler.append("<div style=\"width: 100%; background: #" + background + "; color:" +
                     foreground + "; border: solid 1px black; margin-bottom: 10px;\">");
    assembler.append("<small>Tags:</small>");
    assembler.append("</div>");
    assembler.append("</body>");
    assembler.append("</html>");

    QString entry=assembler.join("\n");

    return entry;
}

//#############################################################################################################
// This function takes the raw HTML output from the Editor and strips out the extra code, leaving only what
// is between the <body> tags intact. Return the clean entry as a QString so another function can feed it into the
// database. New for 0.5. (7/3/13). UPDATE 7/4/13: Delay this to version 0.6.
QString HTMLCore::ProcessEntryFromEditor(QString rawtext){

    QString body=rawtext;

    while(!body.startsWith("<p", Qt::CaseInsensitive)){
        body=body.remove(0,1);
    }

    body=body.remove("</body></html>");

    body=body.trimmed();

    return body;
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

//################################################################################################
// Define CSS parameters from buffer data. This function used to be in EntryExporter but
// it got moved to HTMLCore for version 0.5. --Will Kraft (7/30/13).
void HTMLCore::Setup_Export_CSS(){

    header_font=Buffer::header_font;
    body_font=Buffer::body_font;

    if(Buffer::header_use_em){
        header_font_size=Buffer::header_font_size + "em";
    }
    else{
        header_font_size=Buffer::header_font_size + "pt";
    }



    if(Buffer::body_use_em){
        body_font_size=Buffer::body_font_size + "em";
    }
    else{
        body_font_size=Buffer::body_font_size + "pt";
    }


    if(Buffer::use_system_colors){

        // use system colors for datebox
        QPalette pal;
        QColor bg=pal.highlight().color();
        QColor fg=pal.highlightedText().color();
        datebox_bgcolor=bg.name();
        datebox_color=fg.name();
    }
    else{
        datebox_bgcolor="#e2e2e2";
        datebox_color="#000000";
    }
}

//##########################################################################################
// Do the single entry export. This function used to be part of EntryExporter but it got
// moved to HTMLCore in version 0.5. --Will Kraft (7/30/13).
void HTMLCore::Do_Export(QString path, QString id, bool use_html)
{
    using namespace std;

    // create an empty file object
    QFile file(path);

    QStringList entry;

    if(Buffer::backend=="MySQL"){
        MySQLCore my;
        entry=my.Get_Entry_New(id);
    }

    QString title=entry.at(0);
    QString date=entry.at(1);
    QString timestamp=entry.at(2);
    QString tags=entry.at(3);
    QString body=entry.at(4);

    // get current date and timestamp (NOT the ones from the entry!). This is used in the "this file was generated
    // by RoboJournal v.X" tagline at the bottom of the document.
    QDateTime t=QDateTime::currentDateTime();
    QString exportdt=t.toString("dddd, MMMM d, yyyy (h:mm ap).");

    // check to see if file already exists at location before writing it

    if(use_html){
        // replace newlines with HTML line breaks
        body=body.replace("\n","<br>");
    }
    else{
        // strip HTML tags
        body.replace("<br>","\n");

    }

    if(file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);

        if(use_html){

            QString entry_tags=tags;

            // turn tags into bullet list
            if((tags=="null") || (tags=="Null") || (tags.isEmpty())){
                entry_tags="<p style=\"margin-left: 3.5em;\"><b>Tags:</b> No tag data</p>";
            }
            else{
                entry_tags=entry_tags.replace(";","<li class=\"tag\">");
                entry_tags="<ul><li style=\"list-style-type: none\"><b>Tags:</b><li class=\"tag\">" + entry_tags + "</ul>";

            }

            // Get values for CSS fields
            Setup_Export_CSS();

            stream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\""
                      "\"http://www.w3.org/TR/html4/strict.dtd\">" << endl;
            stream << "<html>" << endl;
            stream << "<head>" << endl;
            stream << "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\">" << endl;
            stream << "<title>"<< title << "</title>" << endl;
            stream << "<style type=\"text/css\">" << endl;
            stream << "\tbody{\n\t\tbackground-color: #ffffff;\n\t}\n\n" << endl;
            stream << "\tp{\n\t\tfont-family: "+ body_font + "; \n\t\tfont-size: "+ body_font_size +" ; \n\t\tmargin-bottom: 1em;\n\t}\n\n" << endl;
            stream << "\th1{\n\t\tfont-family: " + header_font + "; \n\t\tfont-size: " + header_font_size + ";\n\t}\n\n" << endl;
            stream << "\t.datebox{\n\t\tbackground-color: " + datebox_bgcolor + "; \n\t\tcolor: " + datebox_color + ";\n\t\tpadding: 5px; \n\t\tborder-radius: 10px; \n\t\tborder: solid 1px gray;\n\t}\n\n" << endl;
            stream << "\thr{\n\t\tbackground-color:silver;\n\t\tcolor: silver;\n\t\theight: 1px;\n\t\twidth:100%;\n\t\tborder: 0px;\n\t}\n\n" << endl;
            stream << "\t.tag{\n\t\tcolor:black;\n\t\tbackground-color:#FFFEE0;\n\t\tpadding: 3px;\n\t\tborder: solid 1px #BEBE8F;"
                      "\n\t\tlist-style-type: none;\n\t\tfont-family: calibri,verdana,sans-serif;\n\t\tfont-size: 0.7em;\n\t}\n\n" << endl;
            stream << "\tli{\n\t\tlist-style-type:square;\n\t\tpadding-right: 1em;\n\t\tfloat: left;\n\t\tmargin-left: 1.5em;\n\t}\n\n" << endl;
            stream << "</style>" << endl;
            stream << "</head>" << endl;
            stream << "<body>" << endl;
            stream << "<h1>" << title << "</h1>" << endl;
            stream << "<div class=\"datebox\">&nbsp;&nbsp;On " <<
                      date << " at " << timestamp << ", " << Buffer::username << " wrote:</div>" << endl;
            stream << "<p>" << body << "</p>" << endl;
            stream << "\t\t<p>" << entry_tags << "</p><br><br>" << endl;
            stream << "<hr>" << endl;
            stream << "<p><i><small>This file was generated by RoboJournal " <<
                      Buffer::version << " on " << exportdt << "</small></i></p>" <<  endl;
            stream << "</body>" << endl;
            stream << "</html>" << endl;
        }

        // do plain text export
        else{
            // Convert HTML Hyphens and dashes to plain text

            body=body.replace(QRegExp("&mdash;"),"--");

            body=body.replace(QRegExp("&ndash;"),"-");

            body=body.replace(QRegExp("&hellip;"),"...");

            body=body.replace(QRegExp("&rsquo;"),"\'");

            body=body.replace(QRegExp("</?sup>"),"");

            // format tags correctly for plain text
            tags=tags.replace(";", " *");


#ifdef _WIN32
            // convert line breaks to windows-style
            body=body.replace("\n", "\r\n");

            stream << "\r\n" << title.toUpper() << "\r\n\r\n" << endl;
            stream << "On " << date << " at " << timestamp << ", " << Buffer::username << " wrote:\r\n" << endl;
            stream << "################################################################################" << endl;
            stream << "\r\n\r\n" << endl;
            stream << body << endl;
            stream << "\r\n\r\n" << endl;
            stream << "Tags: *" << tags << endl;
            stream << "\r\n" << endl;
            stream << "################################################################################" << endl;
            stream << "\r\nThis file was generated by RoboJournal " << Buffer::version << " on " << exportdt <<  endl;
#endif

#ifdef unix
            stream << "\n" << title.toUpper() << "\n" << endl;
            stream << "On " << date << " at " << timestamp << ", " << Buffer::username << " wrote:\n" << endl;
            stream << "################################################################################" << endl;
            stream << "\n" << endl;
            stream << body << endl;
            stream << "\n\n" << endl;
            stream << "Tags: *" << tags << endl;
            stream << "\n" << endl;
            stream << "################################################################################" << endl;
            stream << "\nThis file was generated by RoboJournal " << Buffer::version << " on " << exportdt <<  endl;
#endif
        }
    }

    file.close();

    QMessageBox b;
    b.information(NULL,"RoboJournal", "<b>" + path + "</b> was successfully exported.");
}


//################################################################################################
// Do mass (full journal) export. This function used to be part of EntryExporter but it got moved
// to HTMLcore during 0.5 development. --Will Kraft (7/30/13).
void HTMLCore::Export_Multi(QString path, bool use_html, bool sort_asc){
    using namespace std;
    //this->setCursor(Qt::WaitCursor);

    //QString path=ui->ExportLocation->text() + QDir::separator() + ui->FileName_2->text();

    // create an empty file object
    QFile file(path);
    QList<QStringList> journals;

    if(Buffer::backend=="MySQL"){
        // Get database dump
        MySQLCore a;
        journals=a.DumpDatabase(sort_asc);
    }

    QString dumptitle;

    if(use_html){
        dumptitle="Contents of&nbsp;<em>" + Buffer::database_name + "</em>";
    }
    else{
        dumptitle="Contents of \"" + Buffer::database_name + "\"";
    }

    // get current date and timestamp (NOT the ones from the entry!). This is used in the "this file was generated
    // by RoboJournal v.X" tagline at the bottom of the document.
    QDateTime t=QDateTime::currentDateTime();
    QString exportdt=t.toString("dddd, MMMM d, yyyy (h:mm ap).");

    QListIterator<QStringList> j(journals);

    int count=0;

    QString intro;

    if(sort_asc){
        intro="<p>All entries have been arranged from oldest to newest.</p>";
    }
    else{
        intro="<p>All entries have been arranged from newest to oldest.</p>";
    }

    if(file.open(QIODevice::ReadWrite)) {

        QTextStream stream(&file);

        // HTML Export
        if(use_html){

            // Prepare CSS values
            Setup_Export_CSS();

            stream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\""
                      "\"http://www.w3.org/TR/html4/strict.dtd\">" << endl;
            stream << "<html>" << endl;
            stream << "<head>" << endl;
            stream << "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\">" << endl;
            stream << "<title>Contents of "<< Buffer::database_name << "</title>" << endl;
            stream << "<style type=\"text/css\">" << endl;
            stream << "\tbody{\n\t\tbackground-color: #ffffff;\n\t}\n\n" << endl;
            stream << "\tp{\n\t\tfont-family: "+ body_font + "; \n\t\tfont-size: "+ body_font_size +" ; \n\t\tmargin-bottom: 1em;\n\t}\n\n" << endl;
            stream << "\th1{\n\t\tfont-family: " + header_font + "; \n\t\tfont-size: " + header_font_size + ";\n\t}\n\n" << endl;

            // calculate size of H2 font relative to H1 size:
            QString header2_font_size;

            if(Buffer::header_use_em){
                float h2_size=Buffer::header_font_size.toInt();
                h2_size=h2_size-1.5;
                header2_font_size=QString::number(h2_size);
                header2_font_size = header2_font_size + "em";
            }
            else{
                int h2_size=Buffer::header_font_size.toInt();
                h2_size=h2_size-6;
                header2_font_size=QString::number(h2_size);
                header2_font_size = header2_font_size + "pt";
            }

            stream << "\th2{\n\t\tfont-family: " + header_font + "; \n\t\tfont-size: " + header2_font_size + ";\n\t\tmargin-left: 0.5em;\n\t}\n\n" << endl;
            stream << "\t.datebox{\n\t\tbackground-color: " + datebox_bgcolor + "; \n\t\tcolor: " + datebox_color + ";\n\t\tpadding: 5px; \n\t\tborder: solid 1px gray; ";

            if(Buffer::use_rounded_corners){
                stream << "\n\t\tborder-radius: 10px; \n\t}\n\n" << endl;
            }
            else{
                stream << "\n\t}\n\n" << endl;
            }

            stream << "\thr{\n\t\tbackground-color:silver;\n\t\tcolor: silver;\n\t\theight: 1px;\n\t\twidth:100%;\n\t\tborder: 0px;\n\t}\n\n" << endl;
            stream << "\tli{\n\t\tlist-style-type:square;\n\t\tpadding-right: 1em;\n\t\tfloat: left;\n\t\tmargin-left: 1.5em;\n\t}\n\n" << endl;
            stream << "\t.number{\n\t\tcolor:gray;\n\t\tfont-weight: lighter;\n\t}\n\n" << endl;
            stream << "\t.tag{\n\t\tcolor:black;\n\t\tbackground-color:#FFFEE0;\n\t\tpadding: 3px;\n\t\tborder: solid 1px #BEBE8F;"
                      "\n\t\tlist-style-type: none;\n\t\tfont-family: calibri,verdana,sans-serif;\n\t\tfont-size: 0.7em;\n\t}\n\n" << endl;
            stream << "</style>" << endl;
            stream << "</head>" << endl;
            stream << "<body>" << endl;
            stream << "<h1>" << dumptitle << "</h1>" << endl;
            stream << intro << endl;

            while(j.hasNext()){

                count++;

                QString entrynumber=QString::number(count);

                QStringList thisentry=j.next();

                QString entry_title=thisentry.at(0);
                QString entry_month=thisentry.at(2);
                QString entry_day=thisentry.at(1);
                QString entry_year=thisentry.at(3);
                QString entry_tags=thisentry.at(4);
                QString entry_body=thisentry.at(5);
                QString entry_time=thisentry.at(6);

                // convert newlines to linebreaks
                entry_body=entry_body.replace("\n","<br>");

                // turn tags into bullet list

                if((entry_tags=="null") || (entry_tags=="Null") || (entry_tags.isEmpty())){
                    entry_tags="<p style=\"margin-left: 3.5em;\"><b>Tags:</b> No tag data</p>";
                }
                else{
                    entry_tags=entry_tags.replace(";","<li class=\"tag\">");
                    entry_tags="<ul><li style=\"list-style-type: none\"><b>Tags:</b><li class=\"tag\">" + entry_tags + "</ul>";

                }



                if(entry_time.isEmpty()){
                    entry_time="[Unknown Time]";
                }

                QString datestamp= entry_day + "/" + entry_month + "/" + entry_year;

                stream << "\t<h2>" << entrynumber << ". " << entry_title << "</h2>" << endl;
                stream << "\t\t<div class=\"datebox\">&nbsp;&nbsp;On "
                       << datestamp << " at " << entry_time << ", " << Buffer::username << " wrote:</div>" << endl;
                stream << "\t\t<p>" << entry_body << "</p>" << endl;
                stream << "\t\t<p>" << entry_tags << "</p><br><br>" << endl;
                stream << "\t\t<hr>" << endl;
            }


            stream << "\t<p><i><small>This file was generated by RoboJournal " <<
                      Buffer::version << " on " << exportdt << "</small></i></p>" <<  endl;
            stream << "</body>" << endl;
            stream << "</html>" << endl;

            file.close();
        }

        // plain text export
        else{

            QString br="\n";

#ifdef _WIN32
            // Use Windows style linebreaks if necessary. A real text editor knows what to do with \n but
            // we have to assume most people will use Notepad. Why they don't get anything better is something
            // I'll never understand.
            br="\r\n";
#endif

            // remove HTML from intro
            intro=intro.replace("</?\\w+>","");



            // add title and header to text file
            stream << dumptitle << br << br << endl;
            stream << intro << br << br << endl;
            stream << "##########################################################" << br << br << endl;

            while(j.hasNext()){

                count++;

                QString entrynumber=QString::number(count);

                QStringList thisentry=j.next();

                QString entry_title=thisentry.at(0);
                QString entry_month=thisentry.at(2);
                QString entry_day=thisentry.at(1);
                QString entry_year=thisentry.at(3);
                QString entry_tags=thisentry.at(4);
                QString entry_body=thisentry.at(5);
                QString entry_time=thisentry.at(6);

                entry_body=entry_body.replace("\n",br);

                // Convert HTML Hyphens and dashes to plain text
                entry_body=entry_body.replace(QRegExp("&mdash;"),"--");

                entry_body=entry_body.replace(QRegExp("&ndash;"),"-");

                entry_body=entry_body.replace(QRegExp("&hellip;"),"...");

                entry_body=entry_body.replace(QRegExp("&rsquo;"),"\'");

                entry_body=entry_body.replace(QRegExp("</?sup>"),"");

                //strip HTML from entry body
                entry_body=entry_body.replace("</?\\w+>","");

                if(entry_tags.isEmpty()){
                    entry_tags="No tag data.";
                }
                else{
                    // turn tags into bullet list
                    QString tagdiv=br + "*";
                    entry_tags=entry_tags.replace(";",tagdiv);
                    entry_tags= br + "Tags: " + br + br + "*" + entry_tags;

                }



                if(entry_time.isEmpty()){
                    entry_time="[Unknown Time]";
                }

                QString datestamp= entry_day + "/" + entry_month + "/" + entry_year;

                stream <<  entrynumber << ". " << entry_title  << br << br << endl;
                stream << "On " << datestamp << " at " << entry_time << ", " << Buffer::username << " wrote:" << br << br<<  endl;
                stream <<  entry_body << br << br << endl;
                stream << entry_tags << br << br << endl;
                stream << "##########################################################" << br << br << endl;
            }


            stream << "This file was generated by RoboJournal " <<
                      Buffer::version << " on " << exportdt <<  endl;

            file.close();
        }

        QMessageBox b;
        b.information(NULL,"RoboJournal", "<nobr><b>" + path + "</b><br> was successfully exported.");
        //this->setCursor(Qt::ArrowCursor);

    }
}

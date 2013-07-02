/*
    This file is part of RoboJournal.
    Copyright (c) 2012 by Will Kraft <pwizard@gmail.com>.
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
  */


#include "ui/entryexporter.h"
#include "ui_entryexporter.h"
#include <QDir>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>
#include <QStringList>
#include <QStringListIterator>
#include "core/buffer.h"
#include <iostream>
#include <QDateTime>
#include "sql/mysqlcore.h"
#include <QListIterator>
#include <QRegExp>
#include <QPalette>

#ifdef _WIN32
#include <windows.h>
#endif



EntryExporter::EntryExporter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EntryExporter)
{
    ui->setupUi(this);

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    int width=this->width();
    int height=this->height();
    this->setMaximumSize(width,height);
    this->setMinimumSize(width,height); 

    // do primary config
    PrimaryConfig();
}

QString EntryExporter::title;
QString EntryExporter::body;
QString EntryExporter::date;
QString EntryExporter::timestamp;


EntryExporter::~EntryExporter()
{
    delete ui;
}

//################################################################################################
// Set locakl values for css from Buffer data
void EntryExporter::SetupCSS(){

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

//################################################################################################
// Validation function that checks to see if the file exists before doing an export operation.
// This function also serves as a splitter...the current tab determines which export function gets called
void EntryExporter::Validate(){

    // Do single export
    if(ui->tabWidget->currentIndex()==0){

        QMessageBox c;

        QString path=ui->ExportLocation->text() + QDir::separator() + ui->FileName->text();

        // create an empty file object. This is just a dummy, we don't actually do anything with this here other than see if it exists.
        QFile file(path);

        //check to see if file exists already. if so, confirm before overwriting it
        if(file.exists()){

            int choice=c.question(this,"RoboJournal","<b>" + ui->FileName->text() + "</b><br> already exists in <b>" +
                                  ui->ExportLocation->text() + "</b>.<br><br>Do you want to replace the existing file?",
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

            switch(choice){
            case QMessageBox::Yes:
                Do_Export();
                break;

            case QMessageBox::No: // do nothing
                break;
            }

        }
        // $filename does not exist so just export data as $filename.
        else{
            Do_Export();
        }
    }

    // Do mass export
    else{

        QMessageBox m;

        QString path=ui->ExportLocation->text() + QDir::separator() + ui->FileName_2->text();

        // create an empty file object. This is just a dummy, we don't actually do anything with this here other than see if it exists.
        QFile file(path);

        //check to see if file exists already. if so, confirm before overwriting it
        if(file.exists()){

            int choice=m.question(this,"RoboJournal","<b>" + ui->FileName_2->text() + "</b><br> already exists in <b>" +
                                  ui->ExportLocation->text() + "</b>.<br><br>Do you want to replace the existing file?",
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

            switch(choice){
            case QMessageBox::Yes:
                Mass_Export();
                break;

            case QMessageBox::No: // do nothing
                break;
            }
        }

        // file does not exist
        else{
            Mass_Export();
        }
    }
}

//################################################################################################
// add line breaks to the body text so everything won't be on one huge line. This is not very important
//for HTML export but it is critical for text files.
QStringList EntryExporter::Do_Word_Wrap(QString body){
    using namespace std;

    // Wrap lines that have blocks of <=120 chars followed by punctuation or whitespace.
    QRegExp wrapper("[.|]|[|?|!|,|\\s|\\b|\\n)]?.{0,100}[.|]|[|?|!|,|\\s|\\b|)|\\n]+",Qt::CaseInsensitive);


    QStringList match;

    int o_count = 0;
    int o_pos = 0;
    while ((o_pos = wrapper.indexIn(body, o_pos)) != -1) {
        ++o_count;
        o_pos += wrapper.matchedLength();
        match << wrapper.cap(0);
        //cout << "found match: " << wrapper.cap(0).toStdString();

    }

    QStringList output;

    for(int i=0; i < match.size(); i++){
        QString nextline=match.at(i);

#ifdef unix
        nextline=nextline+"\n";
#endif

#ifdef _WIN32
        // Use windows-style linebreaks if building on win32.
        nextline=nextline+"\r\n";
#endif
        cout << nextline.toStdString();
        output.append(nextline);

    }

    return output;
}

//################################################################################################
// Do the export
void EntryExporter::Do_Export(){
    using namespace std;

    QString path=ui->ExportLocation->text() + QDir::separator() + ui->FileName->text();

    // create an empty file object
    QFile file(path);

    //remove date from title
    title=title.section(":",1);

    // Get current date and timestamp

    // get current date and timestamp (NOT the ones from the entry!). This is used in the "this file was generated
    // by RoboJournal v.X" tagline at the bottom of the document.
    QDateTime t=QDateTime::currentDateTime();
    QString exportdt=t.toString("dddd, MMMM d, yyyy (h:mm ap).");

    // check to see if file already exists at location before writing it

    //cout << "User accepted" << endl;
    bool use_html=ui->HTML->isChecked();

    if(use_html){
        // replace newlines with HTML line breaks
        body=body.replace("\n","<br>");
    }
    else{
        // strip HTML tags
        body.replace("<br>","\n");

    }


//    // turn tags into bullet list

//    if((entry_tags=="null") || (entry_tags=="Null") || (entry_tags.isEmpty())){
//        entry_tags="<p style=\"margin-left: 3.5em;\"><b>Tags:</b> No tag data</p>";
//    }
//    else{
//        entry_tags=entry_tags.replace(";","<li class=\"tag\">");
//        entry_tags="<ul><li style=\"list-style-type: none\"><b>Tags:</b><li class=\"tag\">" + entry_tags + "</ul>";

//    }



    if(file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);

        if(use_html){

            // Get values for CSS fields
            SetupCSS();

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
            stream << "</style>" << endl;
            stream << "</head>" << endl;
            stream << "<body>" << endl;
            stream << "<h1>" << title << "</h1>" << endl;
            stream << "<div class=\"datebox\">&nbsp;&nbsp;On " <<
                      date << " at " << timestamp << ", " << Buffer::username << " wrote:</div>" << endl;
            stream << "<p>" << body << "</p>" << endl;
            //stream << "\t\t<p>" << entry_tags << "</p><br><br>" << endl;
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


            // word wrapping is still broken, don't use it.
            /*

            QString formatted_body;
            QStringList body_wrap=Do_Word_Wrap(body);
            for(int i=0; i<body_wrap.size(); i++){
                QString nextline=body_wrap.at(i);
                formatted_body.append(nextline);
                //cout << "appending: " <<  nextline.toStdString();
            }
            */


#ifdef _WIN32
            // convert line breaks to windows-style
            body=body.replace("\n", "\r\n");

            stream << "\r\n" << title.toUpper() << "\r\n\r\n" << endl;
            stream << "On " << date << " at " << timestamp << ", " << Buffer::username << " wrote:\r\n" << endl;
            stream << "################################################################################" << endl;
            stream << "\r\n\r\n" << endl;
            stream << body << endl;
            stream << "\r\n\r\n" << endl;
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
            stream << "################################################################################" << endl;
            stream << "\nThis file was generated by RoboJournal " << Buffer::version << " on " << exportdt <<  endl;
#endif


        }
    }
    file.close();
    QMessageBox b;
    b.information(this,"RoboJournal", "<b>" +
                  ui->FileName->text() + "</b> was successfully exported.");


}


//################################################################################################
// Updates static variables. This runs whenever MainWindow:: GetEntry() or MainWindow::Recent() is called.
void EntryExporter::UpdateValues(QString new_title, QString new_date, QString new_body, QString new_timestamp){
    title=new_title;
    date=new_date;
    body=new_body;
    timestamp=new_timestamp;
}


//################################################################################################
// Set filename for Mass export
void EntryExporter::Set_Mass_Name(){

    QString extension;

    if(ui->HTML_2->isChecked()){
        extension=".html";
    }
    else{
        extension=".txt";
    }

    QDateTime t=QDateTime::currentDateTime();
    QString datestamp;
    switch(Buffer::date_format){
    case 0:
        datestamp=t.toString("dd-MM-yyyy");
        break;

    case 1:
        datestamp=t.toString("MM-dd-yyyy");
        break;

    case 2:
        datestamp=t.toString("yyyy-MM-dd");
        break;

    }

    QString filename;

    if(ui->IncludeExportDate->isChecked()){
        filename=Buffer::database_name + "_" + datestamp + extension;
    }
    else{
        filename=Buffer::database_name + extension;
    }

    ui->FileName_2->setText(filename);

}

//################################################################################################
// process filename. This function gets called on load or whenever the filename flags get changed.
void EntryExporter::SetName(){

    ui->FileName->clear();

    QString filename=EntryExporter::title;

    // if we are sorting by month get rid of The month, day, and colon
    if(!Buffer::sortbyday){
        filename=filename.section(":",1);
    }

    // get rid of colons in the filename-- they cause problems with Windows because of that idiotic drive letter naming scheme
    // that is still around even after 25+ years
    filename=filename.replace(":","-");

    //do initial processing... remove spaces and convert to lowercase

    filename=filename.simplified();


    if(ui->IncludeJournalName->isChecked()){
        filename=Buffer::database_name + "_" + filename;
    }

    if(ui->IncludeDate->isChecked()){
        filename=filename + "_" + EntryExporter::date;
    }


    if(ui->HTML->isChecked()){
        filename=filename+".html";
    }
    else{
        filename=filename+".txt";
    }

    // Bugfix for 0.4: get rid of any commas, brackets, or any other undesired characters
    filename=filename.replace(QRegExp("(\\,)+|(\\()+|(\\))+|(\\[)+|(\\])+|(\\{)+|(\\})+|(#)+|(%)+"),"");
    filename=filename.replace("/","-"); // get rid of date slashes
    filename=filename.replace(QRegExp("\\s"),"_");

#ifdef _WIN32

    // Get rid of illegal filename characters on Windows.
    filename=filename.replace(QRegExp("(:)+|(<)+|(>)+|(:)+|(\")+|(/)+|(\\\\)+|(\\|)+|(\\?)+|(\\*)+"),"");

#endif

    // set the value
    filename=filename.toLower();
    ui->FileName->setText(filename);
}


//################################################################################################
// Do mass export
void EntryExporter::Mass_Export(){
    using namespace std;
    this->setCursor(Qt::WaitCursor);

    QString path=ui->ExportLocation->text() + QDir::separator() + ui->FileName_2->text();

    // create an empty file object
    QFile file(path);
    QList<QStringList> journals;

    bool use_html=ui->HTML_2->isChecked();
    bool asc=ui->asc->isChecked();

    if(Buffer::backend=="MySQL"){
        // Get database dump
        MySQLCore a;
        journals=a.DumpDatabase(asc);
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

    if(asc){
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
            SetupCSS();

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
        b.information(this,"RoboJournal", "<nobr><b>" +
                      ui->FileName_2->text() + "</b><br> was successfully exported.");
        this->setCursor(Qt::ArrowCursor);

    }
}

//################################################################################################
// Show folder selection dialog when user chooses output directory
void EntryExporter::Browse(){
    QString outputdir=QFileDialog::getExistingDirectory(this,
                                                        "Select Directory",ui->ExportLocation->text(),QFileDialog::ShowDirsOnly);

    // only copy the directory to the ExportLocation field if the size is greater than 0.
    // if the user cancels the QFileDialog, the size WILL be 0 and we don't want this to be blank.
    if(outputdir.length() != 0){

        QFileInfo p(outputdir);
        if(p.isWritable()){
            ui->ExportLocation->clear();
            ui->ExportLocation->setText(outputdir);
        }
        else{
            QMessageBox m;
            m.critical(this,"RoboJournal","You are not allowed to save files in <b>" +
                       outputdir +"</b>! Please select a different location and try again.");

            //  Browse for another directory
            outputdir.clear();
            Browse();
        }

    }
}

//################################################################################################
// setup form when PrimaryConfig::exec() is called
void EntryExporter::PrimaryConfig(){
    using namespace std;
    // Set export db tab to disabled for now, hopefully i'll get this done by 0.4
    //ui->tabWidget->setTabEnabled(1,false);


    // set defaults
    ui->HTML->click();
    ui->IncludeDate->click();
    ui->IncludeJournalName->click();
    ui->IncludeExportDate->click();

    // mass export tab
    ui->HTML_2->click();
    ui->desc->click();

    ui->tabWidget->setContentsMargins(0,6,0,9);
    ui->label->setContentsMargins(9,0,0,0);
    ui->buttonBox->setContentsMargins(9,0,9,9);

    // add filename and journal name
    QString homepath=QDir::homePath();

    // check for the presence of a ~/Documents folder on Unix. If we find one, use it.
#ifdef unix

    QString documents=homepath+"/Documents";
    QDir docs(documents);

    if(docs.exists()){
        homepath=documents;
    }

#endif

    // add special Windows pathname instructions for Documents folder. Get windows version b/c XP doesn't have a Documents folder.
#ifdef _WIN32

    DWORD dwVersion = GetVersion();
    DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));

    homepath=homepath.replace("/","\\");

    //Windows XP and older
    if(dwMajorVersion < 6){
        homepath=homepath+QDir::separator()+"My Documents";
    }
    // Windows Vista & newer
    else{
        homepath=homepath+QDir::separator()+"Documents";
    }


#endif

    ui->ExportLocation->setText(homepath);
    ui->ExportLocation->setReadOnly(true);

    // set filename
    SetName();
}



void EntryExporter::on_BrowseButton_clicked()
{
    Browse();
}

void EntryExporter::on_PlainText_clicked()
{
    SetName();
}

void EntryExporter::on_HTML_clicked()
{
    SetName();
}

void EntryExporter::on_IncludeDate_clicked()
{
    SetName();
}

void EntryExporter::on_IncludeJournalName_clicked()
{
    SetName();
}

void EntryExporter::on_buttonBox_accepted()
{
    Validate();

}



void EntryExporter::on_HTML_2_clicked()
{
    Set_Mass_Name();

}
void EntryExporter::on_PlainText_2_clicked()
{
    Set_Mass_Name();
}


void EntryExporter::on_IncludeExportDate_clicked()
{
    Set_Mass_Name();
}

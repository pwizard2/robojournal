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

#include "ui/mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_editor.h"
#include "ui/editor.h"
#include <iostream>
#include "core/buffer.h"
#include "ui/dblogin.h"
#include "ui_dblogin.h"
#include <QtSql/QSqlDatabase>
#include "sql/psqlcore.h"
#include <QMessageBox>
#include "sql/mysqlcore.h"
#include "sql/sqlitecore.h"
#include "ui/aboutrj.h"
#include "ui_aboutrj.h"
#include <QTreeWidgetItem>
#include <QListIterator>
#include <QTreeWidgetItem>
#include <QList>
#include <QSqlError>
#include "ui/firstrun.h"
#include "ui_firstrun.h"
#include <QPalette>
#include <QInputDialog>
#include <QCloseEvent>
#include <QPrinter>
#include <QPrintDialog>
#include <QAbstractButton>
#include "ui/tagger.h"
#include "ui_tagger.h"
#include "ui/journalcreator.h"
#include "ui_journalcreator.h"
#include "ui/journalselector.h"
#include "ui_journalselector.h"
#include "ui/entryexporter.h"
#include "ui_entryexporter.h"
#include "ui_firstrun.h"
#include "ui/firstrun.h"
#include "ui/newconfig.h"
#include "ui_newconfig.h"
#include "core/settingsmanager.h"
#include <QRegExp>
#include <QScrollBar>
#include <QDate>
#include "ui/tagreminder.h"
#include "ui_tagreminder.h"
#include <QProcess>
#include <QDir>
#include "ui/newjournalcreator.h"
#include "ui_newjournalcreator.h"
#include "core/taggingshared.h"


QWidget* MainWindow::mw;

// 0.4.1: Consolidate the clear search results code into its own function since it needs to be called
// more than once.
//################################################################################################
void MainWindow::ClearSearchResults(){
    has_search_results=false;
    ui->SearchTerm->clear();
    ui->SearchList->clear();
    ui->SearchList->setColumnWidth(0,100);
    ui->SearchCount->clear();
    ui->Output->clear();

    ui->SearchTerm->setFocus();
    ui->ClearButton->setDisabled(true);

    // Clean the buffer
    Buffer::SearchText.clear();
    Buffer::LastSearchTerm.clear();
}

// This function buffers/restores the text in the output pane depending on which tab is active.
//################################################################################################
void MainWindow::SwitchTab(int index){

    QScrollBar *vbar = ui->Output->verticalScrollBar();

    switch(index){

    case 0: // Entries Tab is active

        Buffer::On_Search=false;
        ui->Output->setText(Buffer::EntryText);
        vbar->setValue(Buffer::entry_scroll_pos);

        //re-enable buttons based on whether they were enabled before we switched to search mode
        ui->TodayButton->setEnabled(Buffer::latest_enabled);
        ui->LastEntry->setEnabled(Buffer::previous_enabled);
        ui->NextEntry->setEnabled(Buffer::next_enabled);
        ui->WriteButton->setEnabled(Buffer::write_enabled);
        ui->EditEntryButton->setEnabled(Buffer::edit_enabled);
        ui->DeleteEntry->setEnabled(Buffer::delete_enabled);
        ui->ExportEntry->setEnabled(Buffer::export_enabled);
        ui->Tag->setEnabled(Buffer::tag_enabled);
        ui->DisconnectButton->setEnabled(Buffer::disconnect_enabled);

        ui->actionLatest_Entry->setEnabled(Buffer::latest_enabled);
        ui->actionNext_Entry->setEnabled(Buffer::next_enabled);
        ui->actionPrevious_Entry->setEnabled(Buffer::previous_enabled);
        ui->actionDelete_Current_Entry->setEnabled(Buffer::delete_enabled);
        ui->actionEdit_Selected_Entry->setEnabled(Buffer::edit_enabled);
        ui->actionWrite->setEnabled(Buffer::write_enabled);
        ui->actionManage_Tags_2->setEnabled(Buffer::tag_enabled);
        ui->actionExport_Entry->setEnabled(Buffer::export_enabled);
        ui->actionDisconnect->setEnabled(Buffer::disconnect_enabled);

        break;

    case 1: // Search Tab is active

        Buffer::On_Search=true;
        ui->Output->setText(Buffer::SearchText);
        vbar->setValue(Buffer::search_scroll_pos);

        ui->SearchButton->setDefault(true);
        ui->SearchTerm->setFocus();

        // remember what toolbar buttons were enabled and disable them in search mode.
        Buffer::latest_enabled=ui->TodayButton->isEnabled();
        Buffer::previous_enabled=ui->LastEntry->isEnabled();
        Buffer::next_enabled=ui->NextEntry->isEnabled();
        Buffer::write_enabled=ui->WriteButton->isEnabled();
        Buffer::edit_enabled=ui->EditEntryButton->isEnabled();
        Buffer::delete_enabled=ui->DeleteEntry->isEnabled();
        Buffer::export_enabled=ui->ExportEntry->isEnabled();
        Buffer::tag_enabled=ui->Tag->isEnabled();
        Buffer::disconnect_enabled=ui->DisconnectButton->isEnabled();

        // Disable the buttons while in search mode.
        ui->TodayButton->setDisabled(true);
        ui->LastEntry->setDisabled(true);
        ui->NextEntry->setDisabled(true);
        ui->WriteButton->setDisabled(true);
        ui->EditEntryButton->setDisabled(true);
        ui->DeleteEntry->setDisabled(true);
        ui->ExportEntry->setDisabled(true);
        ui->Tag->setDisabled(true);
        ui->DisconnectButton->setDisabled(true);

        ui->actionLatest_Entry->setDisabled(true);
        ui->actionNext_Entry->setDisabled(true);
        ui->actionPrevious_Entry->setDisabled(true);
        ui->actionDelete_Current_Entry->setDisabled(true);
        ui->actionEdit_Selected_Entry->setDisabled(true);
        ui->actionWrite->setDisabled(true);
        ui->actionManage_Tags_2->setDisabled(true);
        ui->actionExport_Entry->setDisabled(true);
        ui->actionDisconnect->setDisabled(true);

        break;
    }
}


// Process highlighting in search result active entry
//################################################################################################
QStringList MainWindow::HighlightResults(QString entry, QString title, QString term, int index){
    using namespace std;

    term=term.trimmed();

    // allow the search to highlight matches that contain HTML hyphens
    if(Buffer::use_html_hyphens){

        QRegExp longdash("(--)|(---)");
        term=term.replace(longdash,"&mdash;");
    }
    
    QString color;

    // assign a color based on stored int value.
    switch(Buffer::highlight_color){
    case 0: //yellow
        color="#ffff00";
        break;

    case 1: // orange
        color="#ff9933";
        break;

    case 2: // pink
        color="#ff99cc";
        break;

    case 3: // lime
        color="#99ff99";
        break;

    case 4: // cyan
        color="#00ffff";
        break;

    case 5: //purple
        color="#cc99ff";
        break;
    }

    // decide what gets highlighted according to search filter int value.

    if(ui->WholeWords->isChecked()){

        // Use a regular expression to find occurrences of the term in the body text. The regex should match
        // word bounds (white space) before the term *and* word bounds OR punctuation after the term.

        QRegExp reg("(\\b)(" + term + ")(\\b)|(.,:;'\"\')");
        reg.setCaseSensitivity(Qt::CaseSensitive);

        /*
        QStringList matches=reg.capturedTexts();


        // View Regexp results. (for debugging purposes only... this block should be commented out in final builds.)
        cout << "Capture Count: " << reg.captureCount() << endl;
        for(int i=0; i < matches.count(); i++){
            cout << "Match " << i <<": " << matches.at(i).toStdString() << endl;
            cout << reg.cap(i).toStdString() << endl;
        }
        */

        switch(index){

        case 0: //do nothing... no search term because tag list is enabled.
            break;

        case 1: // highlight title only
            title=title.replace(reg,"<span style=\"background-color: " + color + "\">" + term + "</span>");
            break;

        case 2: // highlight body only
            entry=entry.replace(reg,"<span style=\"background-color: " + color + "\">" + term + "</span>");
            break;

        case 3: // highlight title and body
            title=title.replace(reg,"<span style=\"background-color: " + color + "\">" + term  + "</span>");
            entry=entry.replace(reg,"<span style=\"background-color: " + color + "\">" + term  + "</span>");
            break;

        }
    }
    // not whole words.
    else{

        switch(index){

        case 0: //do nothing... no search term because tag list is enabled.
            break;

        case 1: // highlight title only
            title=title.replace(term,"<span style=\"background-color: " + color + "\">" + term + "</span>");
            break;

        case 2: // highlight body only
            entry=entry.replace(term,"<span style=\"background-color: " + color + "\">" + term + "</span>");
            break;

        case 3: // highlight title and body
            title=title.replace(term,"<span style=\"background-color: " + color + "\">" + term + "</span>");
            entry=entry.replace(term,"<span style=\"background-color: " + color + "\">" + term + "</span>");
            break;

        }
    }

    // build a stringlist of highlighted values and export them back out.
    QStringList highlighted;
    highlighted << entry << title;

    return highlighted;
}


// Function that searches the database and shows it in the new Search tab. New for 0.4.
//################################################################################################
void MainWindow::SearchDatabase(){
    using namespace std;
    if(Buffer::backend=="MySQL"){

        int count=0;

        int index=ui->SearchFilter->currentIndex();
        QString tags=ui->TagList->currentText();
        QString searchterm=ui->SearchTerm->text();

        if((searchterm.isEmpty()) && (ui->SearchFilter->currentIndex() !=0)){

            QMessageBox m;
            m.critical(this,"RoboJournal","Please enter one or more words to search for.");
            ui->SearchTerm->setFocus();
        }
        else{

            //Prevent multiple consecutive searches of the same term to reduce unnecessary database overhead.
            if((Buffer::LastSearchTerm != searchterm) || (ui->SearchFilter->currentIndex() ==0)){

                this->setCursor(Qt::WaitCursor);

                // prevent crash by telling the app we have no search results before clearing the form. Otherwise, the app
                // will call GetEntry() on a null id number. The reason for this is the SearchList::currentitemChanged
                // slot is auto-triggered every time SearchList is changed. (clearing counts as a change)
                has_search_results=false;

                // allow the search to return matches that contain HTML hyphens.
                if((Buffer::use_html_hyphens) || (Buffer::use_smart_quotes)){

                    // replace 2 >= adjacent dashes with a clean mdash
                    searchterm=searchterm.replace(QRegExp("-{2,}"),"&mdash;");

                    // replace 2 >= adjacent dots with ellipsis char.
                    searchterm=searchterm.replace(QRegExp("\\.{2,}\\s*?"),"&hellip;");

                    // This also helps to break SQL injection.
                    searchterm=searchterm.replace(QRegExp("\'"),"&rsquo;");
                }

                // clean up stuff from last search
                ui->SearchList->clear();
                ui->Output->clear();
                Buffer::SearchText.clear();
                ui->SearchCount->clear();
                ui->ClearButton->setEnabled(true);

                // run the next search
                MySQLCore n;
                QList<QStringList> results=n.SearchDatabase(searchterm,index,tags,ui->WholeWords->isChecked());
                QListIterator<QStringList> a(results);


                QString iconpath;

                switch(Buffer::entry_node_icon){
                case 0:
                    iconpath=":/icons/pencil.png";
                    break;

                case 1:
                    iconpath=":/icons/pencil2.png";
                    break;

                case 2:
                    iconpath=":/icons/pencil-small.png";
                    break;

                case 3:
                    iconpath=":/icons/balloon-quotation.png";
                    break;

                case 4:
                    iconpath=":/icons/node.png";
                    break;

                case 5:
                    iconpath=":/icons/bullet_black.png";
                    break;

                case 6:
                    iconpath=":/icons/bullet_blue.png";
                    break;
                }

                while(a.hasNext()){

                    QTreeWidgetItem *item = new QTreeWidgetItem(ui->SearchList);
                    QStringList nextitem=a.next();

                    QIcon entryicon(iconpath);

                    QString tooltip;

                    if(index == 0){
                        tooltip="The number of hits for current search term is non-applicable in tag search mode.";
                    }
                    else{
                        tooltip="Entry contains " + nextitem.at(3) + " occurence(s) of the search term \"" + searchterm + "\".";
                    }

                    // entry title + a small bit of space to pad it out.
                    // 0.5 Bugfix (8/4/13): Add trimmed to remove any hidden line breaks or other text debris that can affect presentation.
                    item->setText(0,nextitem.at(1).trimmed() + "  ");

                    // Convert QString date to QDate to make it sort correctly/
                    // QDate automatically inherits system date settings so we should let it do the work
                    // instead of manually reformatting the date.
                    QDate entrydate;
                    QString id=nextitem.at(2);

                    QStringList date=id.split("/");

                    int month=0, day=0, year=0;

                    switch(Buffer::date_format){
                    case 0: // European
                        month=date.at(1).toInt();
                        day=date.at(0).toInt();
                        year=date.at(2).toInt();
                        break;

                    case 1: // USA Style
                        month=date.at(0).toInt();
                        day=date.at(1).toInt();
                        year=date.at(2).toInt();
                        break;

                    case 2: // ISO
                        month=date.at(1).toInt();
                        day=date.at(2).toInt();
                        year=date.at(0).toInt();
                        break;
                    }

                    // add to date column
                    entrydate.setYMD(year,month,day);

                    item->setData(1,Qt::DisplayRole,entrydate);
                    item->setTextAlignment(1,Qt::AlignLeft);

                    if(index==0){
                        // use an icon to represent n/a if we are in tag search mode.
                        QIcon na(":/icons/na.png");
                        //item->setData(2,Qt::DisplayRole,na);
                        item->setIcon(2,na);
                        item->setToolTip(2,tooltip);
                    }
                    else{
                        // hits column. value should be an int instead of a string so it sorts correctly.
                        int hits=nextitem.at(3).toInt();
                        item->setData(2,Qt::DisplayRole,hits);
                        item->setToolTip(2,tooltip);
                        item->setTextAlignment(2,Qt::AlignLeft);
                    }


                    // invisible ID column
                    item->setText(3,nextitem.at(0));

                    item->setIcon(0,entryicon);

                    // add one to the count
                    count++;
                }

                if(count==1){
                    ui->SearchCount->setText("<img src=\":/icons/information.png\">&nbsp;Search complete: " +
                                             QString::number(count) + " result.");

                    has_search_results=true;
                    this->setCursor(Qt::ArrowCursor);
                }
                else{
                    ui->SearchCount->setText("<img src=\":/icons/information.png\">&nbsp;Search complete: " +
                                             QString::number(count) + " results.");

                    has_search_results=true;
                    this->setCursor(Qt::ArrowCursor);

                    // Show notification if search returns null. That way, people won't be scratching their heads wondering
                    // what happened.
                    if((count==0) && (ui->SearchFilter->currentIndex() != 0)){

                        this->setCursor(Qt::ArrowCursor);

                        QMessageBox j;
                        j.information(this,"RoboJournal","No results for search term \"" + searchterm + "\".");
                        ui->SearchTerm->setFocus();
                        ui->SearchList->setColumnWidth(0,100);
                        has_search_results=false;
                    }

                    // This block gets executed if there are no matches and the user is in tag search mode.
                    // This normally won't happen but is possible if someone removes a specific tag from all entries that
                    // have it WITHOUT refreshing the tag list afterward. If triggered, this block forces an automatic tag refresh to fix the problem.
                    if((count==0) && (ui->SearchFilter->currentIndex() == 0)){

                        this->setCursor(Qt::ArrowCursor);

                        QMessageBox j;
                        if(ui->TagList->count()==0){
                            j.critical(this,"RoboJournal","RoboJournal could not locate any tag data! You must declare at least two tags before you can use the Tag Search feature.");

                        }
                        else{

                            j.information(this,"RoboJournal","No entries are tagged with \"" + ui->TagList->currentText() + "\"."
                                          " This only happens if the tag list is out of date and needs to be refreshed (or you've just found a bug).");

                        }

                        // force tag refresh to fix the situation
                        ui->TagList->clear();
                        GetTagList();

                        ui->Output->clear();
                        Buffer::SearchText.clear();

                        has_search_results=false;
                    }
                }

                if(count > 0){

                    //Resize columns to eliminate horizontal scroll if possible.
                    for(int i=0; i < ui->SearchList->columnCount(); i++){
                        ui->SearchList->resizeColumnToContents(i);
                    }
                }
            }
        }

        // update last search term with current term
        Buffer::LastSearchTerm=searchterm;
    }
}

// function that gets current tag list for search pane.
// * 6/11/13: Use the new TaggingShared class for version 0.5.
//################################################################################################
void MainWindow::GetTagList(){

    TaggingShared ts;
    QStringList tag_list=ts.TagAggregator();

    QIcon tagicon(":/icons/tag_red.png");
    for(int z=0; z < tag_list.size(); z++){
        QString text=tag_list[z];

        if(!text.isEmpty()){
            ui->TagList->addItem(tagicon,text);
        }
    }
}

// Launch Entry Exporter
//################################################################################################
void MainWindow::ExportEntry(){
    if(CurrentID!="-1"){
        QTreeWidgetItem *selected=ui->EntryList->currentItem();

        EntryExporter e(this);
        EntryExporter::title=selected->text(0);
        EntryExporter::id=selected->text(1); // capture current id number (new for 0.5, 7/30/13).

        e.setWindowTitle("Export Content");
        e.exec();
    }
}

// feed in raw tag data from the database and return the formatted string
//################################################################################################
QString MainWindow::FormatTags(QString tags, QString color1, QString color2){

    QStringList tags_array=tags.split(";");

    // group tags by alphabetical order. Bugfix for 0.5 (7/31/13).
    tags_array.sort();

    QString output_tags;
    bool has_tags=true;

    for(int i=0; i<tags_array.size(); i++){
        QString nexttag=tags_array.at(i);

        nexttag="<small><nobr><img src=\":/icons/tag_orange.png\">&nbsp;" + nexttag + "</nobr></small>&nbsp;&nbsp;";

        output_tags=output_tags+nexttag;

        if((tags_array.at(0)=="Null") || (tags_array.at(0)=="null")){
            has_tags=false;
        }

    }

    if((Buffer::use_custom_theme) && (!Buffer::datebox_override)){
        color1=Buffer::text_hexcolor;
        color2=InvertColor(Buffer::text_hexcolor);
    }

    QString div="<div style=\"background-color: " + color1 + "; color: " +
            color2 + "; width=100%;\"><small>&nbsp;&nbsp;Tags</small></div><br>";

    if(has_tags){
        // add HR
        output_tags= div + "&nbsp;&nbsp;" + output_tags;
    }
    else{
        output_tags= div + "&nbsp;&nbsp;<small>This entry has not been tagged yet.</small>";
    }

    if(tags_array.at(0).isEmpty()){
        output_tags= div + "&nbsp;&nbsp;<small>No tags for this post.</small>";
    }


    return output_tags;
}

// Launch Tagger function
//################################################################################################
void MainWindow::Tag(){
    using namespace std;
    QTreeWidgetItem *selected=ui->EntryList->currentItem();

    // fixes crash that occurs when a null selection is passed to Tagger
    if(selected !=NULL){

        QString title=selected->text(0);

        // sort by day causes problems with using regex
        if(Buffer::sortbyday){
            // so we do nothing because title should stay as it is
        }
        else{
            // truncate date from front of title
            int startpoint=title.indexOf(":")+2;
            title=title.section("",startpoint,title.size());
            title=title.trimmed();
        }

        // pass title and id to Tagger class
        Tagger::id_num=selected->text(1);

        // prevent Tagger from opening when we are positioned on a non-entry or DB title
        // after all, we don't want to tag those things.
        if((CurrentID != "-1") && (title != Buffer::database_name)){

            // pass id to Tagger class
            Tagger::id_num=selected->text(1);

            Tagger t(this);
            t.setWindowTitle("Manage Tags");
            t.exec();

            // refresh entry
            GetEntry(CurrentID,false);
        }
        else{
            cout << "OUTPUT: Tagging not allowed on invalid record!" << endl;
        }
    }


}

//################################################################################################
// Confirm the user's intention to quit the program. This function replaces Quit();
void MainWindow::closeEvent(QCloseEvent *event){
    using namespace std;
    event->ignore();

    // Make RoboJournal show confirmation dialogs if config says so
    if(Buffer::showwarnings){

        // dialog that gets shown if user is connected to a journal
        // bugfix (12/4/12) for 0.4: show warning if the user is connected but is currently on the search tab.
        if((ui->DisconnectButton->isEnabled()) || (ui->tabWidget->currentIndex()==1)){
            QMessageBox q(this);
            q.setText("Currently connected to <b>" + Buffer::database_name + "@" + Buffer::host + "</b>!");
            q.setWindowTitle("RoboJournal");
            q.setInformativeText("Do you really want to quit?");
            q.setStandardButtons(QMessageBox::Cancel);

            QPushButton *quit=q.addButton("&Quit",QMessageBox::AcceptRole);

            q.setIcon(QMessageBox::Question);
            q.setDefaultButton(QMessageBox::Cancel);

            q.exec();

            if(q.clickedButton() == quit){
                cout << "OUTPUT: User quit the program." << endl;
                event->accept();
            }
            else{
                //do nothing
                event->ignore();
            }
        }

        // dialog that gets shown if user is NOT connected to a journal
        // update: Don't show dialog if there is no journal active b/c it can get annoying after awhile
        else{
            cout << "OUTPUT: User quit the program." << endl;
            event->accept();
        }
    }

    // just quit silently if user has turned off confirm dialogs
    else{
        cout << "OUTPUT: User quit the program." << endl;
        event->accept();
    }
}

//################################################################################################
// Private function to auto-update Currentid whenever new function is selected
void MainWindow::UpdateSelectedEntry(QString id){
    using namespace std;

    // CurrentID is called by several different functions so it should ALWAYS be up
    // to date so you don't accidentally change the wrong entry

    CurrentID=id;

    //cout << "Current index: " << CurrentID.toStdString() << endl;
}

//################################################################################################

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    using namespace std;
    ui->setupUi(this);

    // Do initial UI configuration (set up toolbar, statusbar, etc.) before showing the main window
    PrimaryConfig();
}

//################################################################################################
// Create Stylesheets for Decorate_GUI() to allow the user to apply a custom
// background and font to RoboJournal GUI. This feature was requested by Sandra Goodhew
// because she thought people would want to use their own backgrounds and fonts.

QString MainWindow::CreateStyleSheet(bool for_entrylist, bool is_TextEdit){
    using namespace std;
    QList<QString> stylelist;

    // if the user wants to use a background image, apply it
    if(Buffer::use_background){

        // apply style to entire QTextEdit class type
        if(is_TextEdit){
            stylelist.append("QTextEdit { ");
        }


        // create style for QTextEdit

        // get system colors for border/background.  That way, it will look
        //good no matter what scheme user has

        QPalette pal;
        QBrush bg=pal.window();
        QBrush border=pal.dark();

        QColor swatch=bg.color();
        QColor c_border=border.color();


        stylelist.append("background: " + swatch.name() + " url(" + Buffer::background_image + "); ");
        stylelist.append("border: solid 2px " + c_border.name() + ";  ");


        // don't tile background if  config says not to
        if(!Buffer::tile_bg){
            stylelist.append("background-repeat: no-repeat; ");
        }

        //bg should be fixed?
        if(Buffer::bg_is_fixed){
            stylelist.append("background-attachment: fixed; ");
        }
    }

    stylelist.append("color: " + Buffer::text_hexcolor + "; ");


    // New for 0.4: Use QFont instead of CSS. CSS unfortunately affects the  context menu font as well.
    QFont font;
    font.setFamily(Buffer::font_face);
    font.setPointSize(Buffer::font_size.toInt());

    // prevent font size/style from being applied to entrylist if user doesn't want it to be
    if(!for_entrylist){
        ui->Output->setFont(font);
    }
    else{
        ui->Output->setFont(font);
        ui->EntryList->setFont(font);
        ui->SearchList->setFont(font);
    }

    //close the QTextEdit class
    if(is_TextEdit){
        stylelist.append(" }");
    }

    QString stylesheet;
    QListIterator<QString> styleiterator(stylelist);

    while(styleiterator.hasNext()){
        stylesheet.append(styleiterator.next());
    }

    //cout << "Style: " << stylesheet.toStdString() << endl;

    return stylesheet;
}

//################################################################################################
// Call updater
void MainWindow::UpgradeJournal(){
    using namespace std;


    QInputDialog d(this);
    d.setWindowTitle("Authenticate");
    d.setLabelText("Provide root password for " + Buffer::host);
    d.setTextEchoMode(QLineEdit::Password);
    bool accept=d.exec();

    if(accept){
        QString root_pass=d.textValue();
        MySQLCore my;
        my.UpgradeJournal(root_pass);

        QMessageBox m;

        if(my.db.isOpenError()){
            m.critical(this,"RoboJournal","Could not connect! Please make sure the root password is correct and try again.");
            UpgradeJournal();
        }
        else{
            m.information(this,"RoboJournal", "<b>" + Buffer::database_name +
                          "</b> has been upgraded successfully. Click the <b>Connect</b> button (or press F2) to log in.");
            cout << "DONE!" << endl;
        }
    }
    else{
        cout << "FAILED!" << endl;
        d.close();
    }
}

//################################################################################################
// Print current entry
void MainWindow::Print(){
    QPrinter p;
    QPrintDialog *d=new QPrintDialog(&p, this);
    d->setWindowTitle("Print Current Entry");

    int choice=d->exec();

    switch(choice){

    case QPrintDialog::Accepted:
        ui->Output->print(&p);
        break;

    case QPrintDialog::Rejected:
        // do nothing
        break;
    }
}

//################################################################################################
// Save current entry as a text file
bool MainWindow::SaveEntry(){
    bool saved=true;
    return saved;
}


//################################################################################################
// Function that returns the inverse Hex color of the provided text color.
// Since the text color is used for the date bar bg if custom theme is used, we want to make sure
// that the text displayed  is always readable. Inverse colors provide the best contrast.

QString MainWindow::InvertColor(QString textcolor){

    using namespace std;
    QColor input;
    input.setNamedColor(textcolor);

    int ired, igreen, iblue;

    ired=input.red();
    igreen=input.green();
    iblue=input.blue();

    //cout << "Input RGB: " << ired << igreen << iblue << endl;

    int ored, ogreen, oblue;

    ored=255-ired;
    ogreen=255-igreen;
    oblue=255-iblue;

    //cout << "Output RGB: " << ored << ogreen << oblue << endl;

    QColor output=QColor::fromRgb(ored,ogreen,oblue);

    QString invertcolor=output.name();
    //cout << "Inverse hex: "  << invertcolor.toStdString() << endl;
    return invertcolor;
}

//################################################################################################
// Function to style GUI with custom background, font, etc.
void MainWindow::Decorate_GUI(){


    // Alternating Row colors can be very good or very bad depending on color scheme.
    //Users have a way to turn it off at long last
    if(!Buffer::alternate_rows){
        ui->EntryList->setAlternatingRowColors(false);
        ui->SearchList->setAlternatingRowColors(false);
    }
    else{
        ui->EntryList->setAlternatingRowColors(true);
        ui->SearchList->setAlternatingRowColors(true);
    }


    // remove icon labels if user doesn't want them
    if(!Buffer::show_icon_labels){
        ui->ConnectButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        ui->DisconnectButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        ui->LastEntry->setToolButtonStyle(Qt::ToolButtonIconOnly);
        ui->WriteButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        ui->EditEntryButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        ui->DeleteEntry->setToolButtonStyle(Qt::ToolButtonIconOnly);
        ui->NextEntry->setToolButtonStyle(Qt::ToolButtonIconOnly);
        ui->TodayButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        ui->ConfigButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        ui->Tag->setToolButtonStyle(Qt::ToolButtonIconOnly);
        ui->ExportEntry->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
    else{ // restore values in case user wishes to enable them at runime

        if(Buffer::toolbar_pos==1){
            ui->ConnectButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            ui->DisconnectButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            ui->LastEntry->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            ui->WriteButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            ui->EditEntryButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            ui->DeleteEntry->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            ui->NextEntry->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            ui->TodayButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            ui->ConfigButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            ui->Tag->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            ui->ExportEntry->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        }
        else{
            ui->ConnectButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            ui->DisconnectButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            ui->LastEntry->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            ui->WriteButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            ui->EditEntryButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            ui->DeleteEntry->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            ui->NextEntry->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            ui->TodayButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            ui->ConfigButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            ui->Tag->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            ui->ExportEntry->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        }


        QFont toolbarFont("Sans",7);
        ui->mainToolBar->setFont(toolbarFont);
    }

    if(Buffer::use_custom_theme){


        QString stylesheet=CreateStyleSheet(false,true);

        ui->Output->setStyleSheet(stylesheet);

        if(Buffer::set_tree_background){
            stylesheet=CreateStyleSheet(true,false);
            ui->EntryList->setStyleSheet(stylesheet);
        }

    }
    // clear all styles if the user disables them during runtime
    else{
        ui->Output->setStyleSheet(NULL);

        if(!Buffer::set_tree_background){
            ui->EntryList->setStyleSheet(NULL);
        }
    }

    //    QPalette winstyle;
    //    QBrush b=winstyle.dark();
    //    QColor color=b.color();

    //    winstyle.setColor(QPalette::ToolTipBase,color);
    //    ui->EntriesTab->setPalette(winstyle);
    //    ui->SearchTab->setPalette(winstyle);

}

//################################################################################################
// New for 0.4.1 (3/26/13): This displays the documentation by launching Qt Assistant as a QProcess. Qt Assistant
// is stored in different places depending on operating system; this code looks for it in the most
// likely places. Invoking robojournal.qhc causes Qt Assistant to override its default settings by showing ONLY
// the RoboJournal documentation.
void MainWindow::ShowHelp(){
    using namespace std;

    this->setCursor(Qt::WaitCursor);

    QString assistant, compiled_help_path, collection_path;


    /* Use separate #ifdef blocks to set the paths for Windows and Unix-like operating systems like
     * Linux b/c the file structure is completely different on the two OSes. RoboJournal on Windows
     * requires Qt Assistantto be installed locally (in the same directory) while Linux is able to
     * make use of global system resources.
     *
     * -- Will Kraft, 3/31/13.
     */
#ifdef _WIN32
    // Qt Assistant should be installed in the same folder as robojournal.exe on Win32.
    assistant=QDir::currentPath() + QDir::separator() + "assistant.exe";
    compiled_help_path=QDir::currentPath() + QDir::separator() + "robojournal.qch";
    collection_path=QDir::currentPath() + QDir::separator() + "robojournal.qhc";
#endif

#ifdef unix
    
    // post-release bugfix for Fedora users (4/26/13)
    // have RoboJournal use the Fedora path to Assistant if applicable.
    QFile fedoracheck("/usr/bin/assistant-qt4");
    
    if(fedoracheck.exists()){
        assistant="/usr/bin/assistant-qt4";
    }
    else{
        assistant="/usr/bin/assistant";
    }

    compiled_help_path="/usr/share/doc/robojournal/robojournal.qch";
    collection_path="/usr/share/doc/robojournal/robojournal.qhc";
#endif

    QFile assistant_exec(assistant);

    if(assistant_exec.exists()){
        QFile collection_file(collection_path);
        QFile documentation_file(compiled_help_path);

        cout << "OUTPUT: Attempting to find help collection file located at "
             << collection_file.fileName().toStdString() << endl;
        cout << "OUTPUT: Attempting to find compiled documentation file located at "
             << documentation_file.fileName().toStdString() << endl;

        if((collection_file.exists()) && (documentation_file.exists())){

            QProcess *p=new QProcess();
            QStringList args;

#ifdef _WIN32
            args << "-collectionFile" << "robojournal.qhc"
                 << "-enableRemoteControl";
#endif

#ifdef unix
            args << "-collectionFile" << collection_path
                 << "-enableRemoteControl";
#endif
            // Spawn the Assistant process with the OS-specific path-to-binary and args.
            QString exec=assistant + " " + args.join(" ");
            p->start(exec, QIODevice::ReadWrite);

            // Instruct Qt Assistant to completely expand the TOC immediately after launch. This saves
            // the user lots of pointless clicking.
            QByteArray input;
            input.append("expandToc -1\n");
            p->write(input);

            if (!p->waitForStarted()){
                return;
            }

            this->setCursor(Qt::ArrowCursor);
        }
        else{

            // Share this messagebox object between several conditionals... it's just more efficient.
            // In all cases, show appropriate errors if one or more files is missing.
            QMessageBox b;



            if((!collection_file.exists()) && (!documentation_file.exists())){

                QString location;
#ifdef _WIN32
                location=QDir::current().path();
#endif

#ifdef unix
                location="/usr/share/doc/robojournal";
#endif

                b.critical(this,"RoboJournal","RoboJournal could not locate the collection file"
                           " (robojournal.qhc) or the compiled help file (robojournal.qch). Please"
                           " locate these two files and copy them into <b>" + location + "</b>.");
                this->setCursor(Qt::ArrowCursor);

            }
            else{

                if(!collection_file.exists()){
                    b.critical(this,"RoboJournal","RoboJournal cannot display the documentation because the Collection File "
                               "needed by Qt Assistant is missing. Please copy the Collection File to <b>" +
                               collection_path + "</b> and try again.");
                    this->setCursor(Qt::ArrowCursor);
                }

                if(!documentation_file.exists()){
                    b.critical(this,"RoboJournal","RoboJournal cannot display the documentation because the Compiled Help File "
                               "needed by Qt Assistant is missing. Please copy the Compiled Help File to <b>" +
                               compiled_help_path + "</b> and try again.");
                    this->setCursor(Qt::ArrowCursor);
                }
            }
        }
    }

    // Show error if Qt assistant is not installed
    else{
        QMessageBox c;
        c.critical(this,"RoboJournal","RoboJournal cannot display the documentation because Qt Assistant"
                   " is not installed correctly. Please install (or move) the Qt Assistant executable to <b>" +
                   assistant + "</b> and try again.");
        this->setCursor(Qt::ArrowCursor);

    }
}

//################################################################################################
// Fetch entry based on ID
void MainWindow::GetEntry(QString id, bool fromSearch){
    using namespace std;
    // Current record should be the ID of the most recent selected entry
    Record=id;

    if(Buffer::backend=="MySQL"){
        MySQLCore my;
        QString entry=my.RetrieveEntry(id);
        QString datestamp=my.TimeStamp(id);

        // pass ddatestamp to global date variable. We need this for the EntryExporter class.
        Global_Datestamp=datestamp;

        // convert plain text linebreaks to html
        entry.replace("\n","<br>");

        // trim whitespace from entry
        entry=entry.simplified();

        // find and use system colors
        QPalette pal;
        QBrush bg=pal.highlight();
        QBrush fg=pal.highlightedText();

        QColor bgcolor=bg.color();
        QColor fgcolor=fg.color();

        QString color1=bgcolor.name();
        QString color2=fgcolor.name();

        // set masthead and display text
        QString text;

        // use timestamp?

        if(Buffer::keep_time){

            // fetch timestamp
            QString timestamp=my.GetTimestamp(Record);
            //cout << "Record: " + Record.toStdString() << endl;

            // get tags
            QString tags=my.GetTags(Record);

            //format tags
            QString output_tags=FormatTags(tags, color1,color2);

            QString username=Buffer::username;

            if(Buffer::use_full_name){
                username=Buffer::full_name;
            }


            // get title if buffer has it
            if(Buffer::show_title){

                QString title=my.GetTitle(id);


                title="<h2>" + title + "</h2>";

                //Highlight the search term if this is triggered from the search function.
                if((fromSearch) && Buffer::use_highlight){
                    QStringList highlighted=HighlightResults(entry,title,ui->SearchTerm->text(),ui->SearchFilter->currentIndex());
                    entry=highlighted.at(0);
                    title=highlighted.at(1);
                }

                if((Buffer::use_custom_theme) && (!Buffer::datebox_override)){
                    QString invertcolor=InvertColor(Buffer::text_hexcolor);

                    text= title + "<div style=\"background-color:" + Buffer::text_hexcolor + ";  color: " +
                            invertcolor +"; width=100%;\"><small>&nbsp;&nbsp;On " +
                            datestamp + " at "+ timestamp + ",  " +   username +
                            " wrote:</small></div><div><br>" + entry + "<br>" + output_tags + "</div>";

                }
                else{

                    text= title + "<div style=\"background-color:" + color1 + "; margin: -3px; color: " +
                            color2 +"; width=100%; \"><small>&nbsp;&nbsp; On " +
                            datestamp + " at "+ timestamp + ", " +  username +
                            " wrote:</small></div><div><br>" + entry + "<br>" + output_tags + "</div>";
                }
            }
            // do not show title
            else{
                if(Buffer::use_custom_theme){
                    QString invertcolor=InvertColor(Buffer::text_hexcolor);

                    text="<div style=\"background-color:" + Buffer::text_hexcolor + "; margin: -3px; color: " +
                            invertcolor +"; width=100%; padding: 20px 80px;\"><small>&nbsp;&nbsp; On " +
                            datestamp + " at "+ timestamp + ", " + username +
                            " wrote:</small></div><div><br>" + entry + "<br>" + output_tags +  "</div>";

                }
                else{

                    text="<div style=\"background-color:" + color1 + "; margin: -3px; color: " +
                            color2 +"; width=100%; padding: 20px 80px;\"><small>&nbsp;&nbsp; On " +
                            datestamp + " at "+ timestamp + ", " +  username +
                            " wrote:</small></div><div><br>" + entry + "<br>" + output_tags + "</div>";
                }
            }

            // pass values to entry exporter
            current_entry_date=datestamp;
            current_entry_body=entry;
            current_entry_time=timestamp;
        }

        //no timestamp



        else{
            // get title if buffer has it
            if(Buffer::show_title){

                // get tags
                QString tags=my.GetTags(Record);

                //format tags
                QString output_tags=FormatTags(tags,color1,color2);

                QString title=my.GetTitle(id);
                title="<h2>" + title + "</h2>";

                //Highlight the search term if this is triggered from the search function.
                if((fromSearch) && Buffer::use_highlight){
                    QStringList highlighted=HighlightResults(entry,title,ui->SearchTerm->text(),ui->SearchFilter->currentIndex());
                    entry=highlighted.at(0);
                    title=highlighted.at(1);
                }

                if((Buffer::use_custom_theme) && (!Buffer::datebox_override)){
                    QString invertcolor=InvertColor(Buffer::text_hexcolor);

                    text= title + "<div style=\"background-color:" + Buffer::text_hexcolor + "; margin: -3px; color: " +
                            invertcolor +"; width=100%; \"><small>&nbsp;&nbsp; On " +
                            datestamp + ", " +   Buffer::username +
                            " wrote:</small></div><div><br>" + entry + "<br>" + output_tags + "</div>";

                }
                else{

                    text= title + "<div style=\"background-color:" + color1 + "; margin: -3px; color: " +
                            color2 +"; width=100%; \"><small>&nbsp;&nbsp; On " +
                            datestamp + ", " +   Buffer::username +
                            " wrote:</small></div><div><br>" + entry + "<br>" + output_tags  + "</div>";
                }
            }
            // do not show title
            else{

                // get tags
                QString tags=my.GetTags(Record);

                //format tags
                QString output_tags=FormatTags(tags,color1,color2);

                //Highlight the search term if this is triggered from the search function.
                if((fromSearch) && Buffer::use_highlight){
                    QStringList highlighted=HighlightResults(entry,"NULL",ui->SearchTerm->text(),ui->SearchFilter->currentIndex());
                    entry=highlighted.at(0);
                }


                if((Buffer::use_custom_theme) && (!Buffer::datebox_override)){
                    QString invertcolor=InvertColor(Buffer::text_hexcolor);

                    text="<div style=\"background-color:" + Buffer::text_hexcolor + "; margin: -3px; color: " +
                            invertcolor +"; width=100%; \"><small>&nbsp;&nbsp; On " +
                            datestamp + ", " +   Buffer::username +
                            " wrote:</small></div><div><br>" + entry + "<br>" + output_tags  + "</div>";

                }
                else{

                    text="<div style=\"background-color:" + color1 + "; margin: -3px; color: " +
                            color2 +"; width=100%; \"><small>&nbsp;&nbsp; On " +
                            datestamp + ", " +   Buffer::username +
                            " wrote:</small></div><div><br>" + entry + "<br>" + output_tags  + "</div>";
                }

            }

            // pass values to entry exporter
            current_entry_date=datestamp;
            current_entry_body=entry;


        }

        // set entry text
        ui->Output->setText(text);

        // Decide which buffer to place the current text
        if(Buffer::On_Search){
            Buffer::SearchText=text;
        }
        else{
            Buffer::EntryText=text;
        }



        // Update EntryExporter class

        QTreeWidgetItem *selected=ui->EntryList->currentItem();

        // Bugfix 6/29/12: Prevent segfault after editing an entry. The reason for the bug
        //is that selected returned null.

        if(selected != NULL){
            QString entrytitle=selected->text(0);
            QString timestamp=my.GetTimestamp(Record);

            EntryExporter e;
            e.UpdateValues(entrytitle,datestamp,entry,timestamp);
        }
    }
}

//################################################################################################
// Function that sets up main window GUI when mainwindow is called at startup
void MainWindow::PrimaryConfig(){

    // New for 0.5 (8/27/13): create a static pointer to the current main window object. This is used elsewhere as a parent
    // to center QMessageBoxes and other elements that don't have a GUI of their own relative to the main window.
    mw=this->window();

    // New for 0.5 (9/12/13): Remember if we have set the most recent entry date already if date override is enabled.
    date_override_trigger_tripped=false;

    // 6/5/13: Will Kraft. (new for version 0.5): Use the big icon on Windows too. That little 16x16 icon looks like hell when
    // stretched to fit the Win 7 taskbar and alt+tab list. The shortcut icon usually compensates for that but not always; this
    // way works for sure no matter what.
    QIcon unixicon(":/icons/robojournal-icon-big.png");
    this->setWindowIcon(unixicon);

    // declare magic sizing algorithm for MainWindow splitter. For some unknown reason the TabWidget throws this off
    // so it is necessary to compensate.
    splittersize=this->width()/4+95; // previously width/4+30

#ifdef _WIN32
    //Windows needs a slightly smaller ratio
    splittersize=this->width()/4+60; // previously width/4+10
#endif

    // auto-select entry list mentu entry
    ui->actionEntry_List->setChecked(true);


    switch(Buffer::toolbar_pos){

    case 0:
        this->addToolBar(Qt::LeftToolBarArea,ui->mainToolBar);
        break;

    case 1:
        this->addToolBar(Qt::TopToolBarArea,ui->mainToolBar);
        break;

    case 2:
        this->addToolBar(Qt::RightToolBarArea,ui->mainToolBar);
        break;
    }


    // Setup Statusbar
    ui->TotalCount->clear();
    ui->statusBar->addWidget(ui->StatusMessage,1);

    //#############################################################################
    // New in 0.3: User notification is a separate statusbar object.
    ui->statusBar->addPermanentWidget(ui->Status_User,0);
    ui->Status_User->clear();

    ui->statusBar->addPermanentWidget(ui->TotalCount,0);


    // setup toolbars. Buttons will be added in the order listed here.
    ui->mainToolBar->addWidget(ui->ConnectButton);
    ui->mainToolBar->addWidget(ui->DisconnectButton);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(ui->TodayButton);
    ui->mainToolBar->addWidget(ui->LastEntry);
    ui->mainToolBar->addWidget(ui->NextEntry);
    ui->mainToolBar->addWidget(ui->Tag);
    ui->mainToolBar->addWidget(ui->ExportEntry);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(ui->WriteButton);
    ui->mainToolBar->addWidget(ui->EditEntryButton);
    ui->mainToolBar->addWidget(ui->DeleteEntry);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(ui->ConfigButton);

    //Set up Search Tab
    QIcon tag(":/icons/tag_orange.png");
    QIcon pat(":/icons/funnel.png");

    ui->SearchFilter->setItemIcon(0,tag);
    ui->SearchFilter->setItemIcon(1,pat);
    ui->SearchFilter->setItemIcon(2,pat);
    ui->SearchFilter->setItemIcon(3,pat);

    ui->SearchFilter->setCurrentIndex(3);
    ui->SearchTerm->setDisabled(true);
    ui->ClearButton->setDisabled(true);
    ui->SearchButton->setDisabled(true);
    ui->TagList->setDisabled(true);
    ui->SearchFilter->setDisabled(true);
    ui->WholeWords->setDisabled(true);
    //ui->HighlightTerm->setDisabled(true);
    ui->SearchCount->clear();

    // hide export entry feature for now... we need to include it in 0.3
    ui->actionSave_Current_Entry->setVisible(false);

    // Set startup toolbar config
    ui->WriteButton->setDisabled(true);
    ui->actionPrint->setDisabled(true);
    ui->Tag->setDisabled(true);
    ui->ExportEntry->setDisabled(true);
    ui->NextEntry->setDisabled(true);
    ui->LastEntry->setDisabled(true);
    ui->TodayButton->setDisabled(true);
    ui->SearchButton->setDisabled(true);
    ui->DeleteEntry->setDisabled(true);
    ui->EditEntryButton->setDisabled(true);
    ui->DisconnectButton->setDisabled(true);
    ui->actionManage_Tags_2->setDisabled(true);
    ui->actionLatest_Entry->setDisabled(true);
    ui->actionNext_Entry->setDisabled(true);
    ui->actionPrevious_Entry->setDisabled(true);
    ui->actionDisconnect->setDisabled(true);
    ui->actionWrite->setDisabled(true);
    ui->actionDelete_Current_Entry->setDisabled(true);
    ui->actionEdit_Selected_Entry->setDisabled(true);
    ui->actionExport_Entry->setDisabled(true);

    QPixmap noicon("qrc:///icons/database_error.png");
    ui->StatusMessage->setPixmap(noicon);
    ui->StatusMessage->setText("Click the <b>Connect</b> button (or press <b>F2</b>) to work with a journal database.");

    ui->EntryList->setColumnWidth(0,450);

    //Column 1 (Entry ID) is important since it contains the DB row ID, that we check the DB with
    // but it's not necessary for the user to see the column since it just
    // clutters the form
    ui->EntryList->setColumnWidth(1,70);
    ui->EntryList->setColumnHidden(1,true);

    // Deal with pesky splitter spacing issues by setting tooltip this way.
    ui->WholeWords->setToolTip("If checked, this option requires the search to return matches that consist only of whole words"
                               " (i.e. <b>cat</b> but <span style=\"text-decoration: underline\">not</span> <b>cat</b>egory or con<b>cat</b>enation).");

    ui->SearchTerm->setToolTip("<p>Enter the pattern (one or more words) you wish to search for in this field and click the <b>Search</b> button to continue.<br><br>FYI: The pattern search function is case-sensitive!</p>");

    // Set properties for search list
    ui->SearchList->setColumnHidden(3,true);
    ui->SearchList->setHeaderHidden(false);
    ui->SearchList->setColumnWidth(0,100);
    ui->SearchList->sortByColumn(1,Qt::DescendingOrder);

    // define header titles for search list. They are displayed in the order they are fed into the StringList.
    QStringList header;
    header << "Entry Title" << "Date" << "Hits" << "id";
    ui->SearchList->setHeaderLabels(header);

    // flip the connection active switch
    ConnectionActive=false;


    // keep the program from crashing if the user clicks Modify before selecting an entry
    CurrentID="-1";

    //Decorate the GUI
    Decorate_GUI();

    // Lock search tab prior to login. This is important because if we switch to the search tab before logging in
    // the toolbar buttons' enable/disable code doesn't work properly anymore. Furthermore, there's no practical reason to have
    // search enabled at startup anyway.
    ui->tabWidget->setTabEnabled(1,false);

    /*
    // Constructor for testing FirstRun, comment out this block in production builds. FirstRun doesn't need to start on
    // normal operations... that would just be weird.
    FirstRun f;
    f.exec();
    */

    // 6/17/13: Eliminate all padding around UI elements.
    ui->tabWidget->setContentsMargins(0,0,0,0);
    ui->splitter->setContentsMargins(0,0,0,0);
    ui->centralWidget->setContentsMargins(0,0,0,0);
    ui->EntryList->setContentsMargins(0,0,0,0);
    ui->gridLayout_2->setContentsMargins(0,0,0,0);
    ui->gridLayout_2->setSpacing(0);

#ifdef _WIN32
    // 6/16/13: Make the splitter easier to see. Windows 7 renders these things as flat by default so the idea
    // is to emulate the old-school raised splitter bar appearance so the user will know something is there.
    QSplitterHandle *handle=ui->splitter->handle(1);
    QVBoxLayout *h_layout = new QVBoxLayout(handle);
    h_layout->setSpacing(0);
    h_layout->setMargin(0);
    h_layout->setContentsMargins(0,0,0,0); // make sure the splitter goes all the way to the edge of the frame

    QFrame *line = new QFrame(handle);
    line->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
    line->setLineWidth(3);
    h_layout->addWidget(line);
#endif

    //#############################################################################
    // New for RoboJournal 0.4
    // force initial 50/50 ratio on splitter. This involves a weird hack that requires the right side to be set to a huge number
    // that is greater than 100. (the left side should always be 1) For some reason, The Tabbed interface wants to be a certain
    //size, so we have to grow the right side by a huge amount to compensate. Strange, but it seems to work.

    // New for 0.5: Force the splitter size ONLY IF a stored value does not exist.
    // Added 6/21/13. Backported to 0.4.2 on 9/13/13.
    if(!Buffer::mw_splitter_size.isEmpty()){
        ui->splitter->restoreState(Buffer::mw_splitter_size);
    }
    else{
        QList<int> size;
        size.append(1);
        size.append(splittersize); // was originally 225 but fixed values dont work very well
        ui->splitter->setSizes(size);
    }

    launched_editor=false;
    launched_config=false;


    // Restore window size from last session if available. If there is no stored data,
    // use default size.
    if(!Buffer::mainwindow_geometry.isEmpty()){
        this->restoreGeometry(Buffer::mainwindow_geometry);
    }

    //#############################################################################
    //new for RoboJournal 0.4.1:

    //set tooltips for mainwindow tabs. This way is less messy than doing it in the ui files.
    ui->tabWidget->setTabToolTip(0,"Display the Timeline.");
    ui->tabWidget->setTabToolTip(1,"Search for entries containing specific patterns or tags. "
                                 "<span style=\"color: red\">(requires active connection)</span>");

    // bugfix: make it so the search term field triggers the search button when the Enter key is pressed.
    // In 0.4 the user had to push the enter key twice to make it work.
    connect(ui->SearchTerm, SIGNAL(returnPressed()), ui->SearchButton,SIGNAL(clicked()));
    //connect(ui->TagList, SIGNAL(currentIndexChanged(QString)), ui->SearchButton,SIGNAL(clicked()));
    connect(ui->SearchFilter, SIGNAL(currentIndexChanged(QString)), ui->ClearButton,SIGNAL(clicked()));

    // Update for 0.4.1: Set titlebar text w/o version number (1/8/13)
    this->setWindowTitle("RoboJournal");

}

//################################################################################################
// Connect to database function
void MainWindow::Connect(){
    using namespace std;
    DBLogin l(this);
    l.setWindowTitle("New Connection [" + Buffer::backend + "]");

    bool is_connected=false; // check to see if connection is true


    l.Refresh();
    l.exec();

    // if user provided login data
    if(Buffer::login_succeeded){

        this->setCursor(Qt::WaitCursor);

        if(Buffer::backend=="MySQL"){

            MySQLCore my;

            bool success=my.Connect();
            bool is_sane=my.SanityCheck();

            if(!success){
                // allow user to try again
                l.ResetPassword();
                this->setCursor(Qt::ArrowCursor);
                Connect();

                return;
            }
            else{

                if(!is_sane){

                    // does the journal fail sanity check because it is old....
                    if(Buffer::needs_upgrade){

                        // show upgrade dialog
                        QMessageBox u(this);
                        u.setIcon(QMessageBox::Question);
                        u.setWindowTitle("RoboJournal");
                        u.setText("<b>" + Buffer::database_name + "</b> is a valid journal but it must be upgraded before RoboJournal "
                                  + Buffer::version + " can use it. After this journal has been upgraded to the new format, it will no longer be "
                                  "compatible with older versions of RoboJournal.<br><br> Do you want to upgrade this journal now?");

                        u.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                        u.setDefaultButton(QMessageBox::Yes);
                        int choice=u.exec();

                        switch(choice){
                        case QMessageBox::Yes:
                            // Update journal
                            cout << "OUTPUT: Starting journal upgrade process...";
                            UpgradeJournal();
                            break;
                        case QMessageBox::No:
                            // do nothing
                            break;
                        }

                        // reset upgrade flag
                        Buffer::needs_upgrade=false;
                    }

                    // or because it is just 100% invalid due to structure
                    else{
                        QMessageBox m;
                        m.critical(this,"RoboJournal","Database <b>" + Buffer::database_name +
                                   "</b> is not a valid journal!");

                        cout << "ERROR: Database " << Buffer::database_name.toStdString() <<
                                " FAILED sanity check, aborting load sequence!" << endl;

                    }
                }
                else{

                    ui->WriteButton->setEnabled(true);


                    ui->TodayButton->setEnabled(true);
                    ui->SearchButton->setEnabled(true);
                    ui->EditEntryButton->setEnabled(true);
                    ui->DisconnectButton->setEnabled(true);
                    ui->NextEntry->setEnabled(true);
                    ui->LastEntry->setEnabled(true);
                    ui->DeleteEntry->setEnabled(true);
                    ui->actionWrite->setEnabled(true);
                    ui->actionLatest_Entry->setEnabled(true);
                    ui->actionNext_Entry->setEnabled(true);
                    ui->actionPrevious_Entry->setEnabled(true);
                    ui->actionDisconnect->setEnabled(true);
                    ui->actionDelete_Current_Entry->setEnabled(true);
                    ui->actionEdit_Selected_Entry->setEnabled(true);
                    ui->actionPrint->setEnabled(true);
                    ui->Tag->setEnabled(true);
                    ui->ExportEntry->setEnabled(true);
                    ui->actionManage_Tags_2->setEnabled(true);
                    ui->actionConnect->setDisabled(true);
                    ui->ConnectButton->setDisabled(true);
                    ui->actionExport_Entry->setEnabled(true);

                    // prepare Search pane
                    ui->SearchTerm->setDisabled(false);
                    ui->ClearButton->setDisabled(false);
                    ui->SearchButton->setDisabled(false);
                    ui->SearchFilter->setDisabled(false);
                    ui->WholeWords->setDisabled(false);
                    ui->WholeWords->setChecked(true);
                    //ui->HighlightTerm->setDisabled(false);

                    // Get ID list
                    //int year_range=Buffer::entryrange.toInt();
                    IDList=my.Create_ID_List(Buffer::entryrange);

                    CreateTree();
                    ui->StatusMessage->setText("Connected to " + Buffer::backend  + " database <b>" +
                                               Buffer::database_name + "</b> on <b>" +  Buffer::host);

                    // provide user notification on statusbar (new for 0.3)
                    ui->Status_User->setTextFormat(Qt::RichText);
                    ui->Status_User->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

                    if(Buffer::is_male){
                        ui->Status_User->setText("&nbsp;<img src=\":/icons/user.png\">&nbsp;" +
                                                 Buffer::username + "&nbsp;&nbsp;");
                    }
                    else{
                        ui->Status_User->setText("&nbsp;<img src=\":/icons/user-female2.png\">&nbsp;" +
                                                 Buffer::username + "&nbsp;&nbsp;");
                    }

                    ui->Status_User->setToolTip("Logged in as \"" + Buffer::username + "\"");
                    is_connected=true;

                }

                //new for 0.4.1

                // Add journal name to title bar if config allows it
                if(Buffer::name_in_titlebar){
                    this->setWindowTitle(Buffer::database_name +" - RoboJournal");
                }

                this->setCursor(Qt::ArrowCursor);
            }
        }

        if(Buffer::backend=="Postgres"){
            PSQLCore po;
            bool success=po.Connect();

            if(!success){
                QMessageBox m;
                m.critical(this,"Error","Connection attempt with <b>" +
                           Buffer::database_name + "</b> on <b>" +
                           Buffer::host + "</b> failed.");
            }
            else{
                ui->WriteButton->setEnabled(true);


                ui->TodayButton->setEnabled(true);
                ui->SearchButton->setEnabled(true);
                ui->EditEntryButton->setEnabled(true);
                ui->DisconnectButton->setEnabled(true);
                ui->NextEntry->setEnabled(true);
                ui->LastEntry->setEnabled(true);

                ui->ConnectButton->setDisabled(true);
                ui->StatusMessage->setText("Connected to <b>" + Buffer::database_name + "</b> on <b>" +
                                           Buffer::host);
            }
        }
    }
    if(Buffer::backend=="SQLite"){

        SQLiteCore sl;
        bool success=sl.Connect();

        if(!success){
            QMessageBox m;
            m.critical(this,"Error", Buffer::backend + " connection attempt with <b>" +
                       Buffer::database_name + "</b> on <b>" +
                       Buffer::host + "</b> failed.");
        }
        else{
            ui->WriteButton->setEnabled(true);


            ui->TodayButton->setEnabled(true);
            ui->SearchButton->setEnabled(true);
            ui->EditEntryButton->setEnabled(true);
            ui->DisconnectButton->setEnabled(true);
            ui->NextEntry->setEnabled(true);
            ui->LastEntry->setEnabled(true);

            ui->ConnectButton->setDisabled(true);
            ui->StatusMessage->setText("Connected to <b>" + Buffer::database_name + "</b> on <b>" +
                                       Buffer::host);
        }




        // create the entry tree
        CreateTree();

        this->setCursor(Qt::ArrowCursor);

        // flip the connection active switch
        ConnectionActive=true;
    }

    // connect to latest entry automatically

    if((is_connected && num_records != 0) || (ConnectionActive)){

        ui->NextEntry->setEnabled(true);
        ui->LastEntry->setEnabled(true);
        ui->TodayButton->setEnabled(true);


        // only load most recent entry if config allows it

        if(Buffer::autoload){

            MostRecent();

            Record=CurrentID;
        }
    }
    // if there are no entries at login, disable a few buttons to prevent crashes
    else{

        if(num_records==0){
            ui->TodayButton->setEnabled(false);
            ui->NextEntry->setEnabled(false);
            ui->LastEntry->setEnabled(false);
            ui->DeleteEntry->setEnabled(false);
            ui->EditEntryButton->setEnabled(false);
            ui->actionLatest_Entry->setDisabled(true);
            ui->actionNext_Entry->setDisabled(true);
            ui->actionPrevious_Entry->setDisabled(true);
            ui->actionDelete_Current_Entry->setDisabled(true);
            ui->actionEdit_Selected_Entry->setDisabled(true);
        }
    }

    //unlock search tab if a connection has been made.
    if(is_connected){
        ui->tabWidget->setTabEnabled(1,true);

        // new for 0.4.1: change tooltip once tab has been unlocked.
        ui->tabWidget->setTabToolTip(1,"Search for entries containing specific patterns or tags.");
    }

    // show tag reminder-- new for 0.4.1
    if(Buffer::show_untagged_reminder){

        TagReminder t(this);

        // Bugfix (cheap hack) 2/28/13: Call the Lookup() function on line # 1735 and get int count to see if
        // the TagReminder dialog should be displayed at all. We don't do  anything with this data other than
        // get a count of the items in the list. It runs again (for real) after the form gets exec()ed.
        // Kind of kludgy and inefficient but at least it works now without breaking QDialog::showEvent().

        int untagged_count=t.Lookup();

        if(untagged_count > 0){

            t.exec();

            // 3/13/13: Update RoboJournal's configuration using data stored in buffer if the "Display this
            // reminder window next time" box was unchecked when the TagReminder instance was destructed. This
            // was originally done fro mthe TagReminder class but that produced a bug that sometimes required
            // the user to click the close button twice. This method is a bit less elegant but it fixes that problem.

            SettingsManager sm;
            sm.SaveNagPreferences();

            // reload the config b/c we made changes. This prevents the preferences window from
            // relying on old data and getting out of sync with the real app configuration.
            sm.LoadConfig(false);


            // Bugfix 3/5/13: Refresh most recent entry (if autoload=true) just in case user added new tags to it
            // with Tag Reminder.
            if(Buffer::autoload){
                MostRecent();
            }
        }
    }

    // new for 0.5: show notification if the date override feature is enabled (8/24/13) and enact safety protocols if the feature is
    // being misused and could result in database corruption (9/12/13).
    if((Buffer::use_date_override) && (is_connected)){
        QString msg="Date override is enabled; all new entries created during this session will artificially use " +
                Buffer::override_date.toString("MM-dd-yyyy") + " as their date of origin.";

        if(Buffer::no_safety)
            msg=msg + "<br><br><b>Warning:</b> Normal safety protocols are disabled. To prevent database corruption, <span style=\"text-decoration: "
                    "underline\">never</span> use the date override to insert a new entry at a future date or prior to the most recent item in the timeline.";


        // Bugfix 9/12/13: Disable New Entries as safety protocol if adding them would corrupt the database.
        if((Buffer::override_date < lastdate) && (!Buffer::no_safety)){
            msg="RoboJournal's safety protocols prevent you from adding new entries "
                    "during this session because the date override value you specified (" + Buffer::override_date.toString("MM-dd-yyyy") +
                    ") predates the most recent entry in the Timeline.<br><br>This restriction is necessary because adding new entries in the "
                    "wrong chronological order causes database corruption.";

            ui->WriteButton->setDisabled(true);
            ui->actionWrite->setDisabled(true);
        }

        // Bugfix 9/12/13: Prevent date override misuse (future dates) from corrupting the database.
        if((Buffer::override_date > QDate::currentDate()) && (!Buffer::no_safety)){
            msg="RoboJournal's safety protocols prevent you from adding new entries "
                    "during this session because the date override value you specified (" + Buffer::override_date.toString("MM-dd-yyyy") +
                    ") is in the future.<br><br>This restriction is necessary because adding new entries in the "
                    "wrong chronological order causes database corruption.";

            ui->WriteButton->setDisabled(true);
            ui->actionWrite->setDisabled(true);
        }

        QMessageBox m;
        m.information(this,"RoboJournal",msg);
    }
}

//################################################################################################
/*
  Launch editor in revision mode. QString entry is the selected index.

  */
void MainWindow::Modify(){
    using namespace std;

    if(CurrentID != "-1"){

        this->setCursor(Qt::WaitCursor);
        ui->statusBar->showMessage("Retrieving requested entry from server, please wait....",0);

        Buffer::editentry=CurrentID;
        Buffer::editmode=true;

        Editor e(this);
        e.exec();
        launched_editor=true;


        // Update Tree
        CreateTree();


        // refresh entry in Output Pane
        GetEntry(CurrentID,false);



        ui->StatusMessage->setText("Connected to " + Buffer::backend  + " database <b>" +
                                   Buffer::database_name + "</b> on <b>" +  Buffer::host);

        // keep current entry selected
        HighlightCurrentSelection(CurrentID);



        this->setCursor(Qt::ArrowCursor);
        ui->statusBar->clearMessage();


    }
    else{ // show error if no  valid entries are selected
        QMessageBox a;
        a.critical(this,"RoboJournal","Please select an entry to modify before clicking the <b>Modify Entry</b> button.");

    }
}


//################################################################################################
// Delete an entry
void MainWindow::DeleteSelectedEntry(){

    MySQLCore my;

    // make sure we're not working with a node, since nodes always have -1 as Current ID
    if(CurrentID!="-1"){

        if(Buffer::showwarnings){
            QMessageBox b;

            int choice=b.question(this,"RoboJournal","Do you really want to delete the selected journal entry?"
                                  " This action cannot be undone.",QMessageBox::Cancel,QMessageBox::Ok);


            switch(choice){
            case QMessageBox::Ok:
                if(Buffer::backend=="MySQL"){
                    my.DeleteEntry(CurrentID);
                    CreateTree();

                    ui->StatusMessage->setText("Connected to " + Buffer::backend  + " database <b>" +
                                               Buffer::database_name + "</b> on <b>" +  Buffer::host);
                    ui->Output->setPlainText(NULL);

                    // Bugfix (8/15/12) prevent the deleter from being used again until a different node is clicked
                    CurrentID = -1;
                    ui->EntryList->clearSelection();
                }
                break;

            case QMessageBox::Cancel:
                // do nothing
                break;


            }
        }
        else{
            if(Buffer::backend=="MySQL"){
                my.DeleteEntry(CurrentID);
                CreateTree();
                ui->StatusMessage->setText("Connected to " + Buffer::backend  + " database <b>" +
                                           Buffer::database_name + "</b> on <b>" +  Buffer::host);
                ui->Output->setPlainText(NULL);

                // Bugfix (8/15/12) prevent the deleter from being used again until a different node is clicked
                CurrentID = -1;
                ui->EntryList->clearSelection();
            }
        }

        //Disable the Tagger and Exporter momentarily because RJ will crash if people click on these w/o
        // an entry selected.
        // bugfix (8/27/12) Prevent crash by disabling the tagger and entryexporter after posting an entry.
        ui->Tag->setDisabled(true);
        ui->ExportEntry->setDisabled(true);
        ui->actionManage_Tags_2->setDisabled(true);
        ui->actionExport_Entry->setDisabled(true);
    }

}


/*
  This function doesn't really terminate a connection since there is no persistent connection...
  all SQL queries are called directly as needed. it just resets the MainWindow class to its
  intial state so a new connection could be made later.
  */
//################################################################################################
void MainWindow::Disconnect(){

    // Bugfix for 0.4.1 (3/14/13): clear all current search data to prevent a segfault that happens
    // if the user disconnects while there are active search results in the Search Tab.
    ClearSearchResults();

    ui->WriteButton->setDisabled(true);
    ui->NextEntry->setDisabled(true);
    ui->LastEntry->setDisabled(true);
    ui->Tag->setDisabled(true);
    ui->actionManage_Tags_2->setDisabled(true);
    ui->TodayButton->setDisabled(true);
    ui->SearchButton->setDisabled(true);
    ui->DeleteEntry->setDisabled(true);
    ui->EditEntryButton->setDisabled(true);
    ui->DisconnectButton->setDisabled(true);
    ui->ConnectButton->setEnabled(true);
    ui->ExportEntry->setDisabled(true);
    ui->Output->setText(NULL);
    ui->EntryList->clear();

    ui->actionWrite->setDisabled(true);
    ui->actionDisconnect->setDisabled(true);
    ui->actionLatest_Entry->setDisabled(true);
    ui->actionNext_Entry->setDisabled(true);
    ui->actionPrevious_Entry->setDisabled(true);
    ui->actionDelete_Current_Entry->setDisabled(true);
    ui->actionEdit_Selected_Entry->setDisabled(true);
    ui->actionConnect->setEnabled(true);
    ui->EntryList->setHeaderHidden(true);
    ui->actionWrite->setDisabled(true);
    ui->actionPrint->setDisabled(true);
    ui->actionExport_Entry->setDisabled(true);

    // prepare Search pane
    ui->SearchTerm->setDisabled(true);
    ui->ClearButton->setDisabled(true);
    ui->SearchButton->setDisabled(true);
    ui->TagList->setDisabled(true);
    ui->TagList->clear();
    ui->SearchList->clear();
    ui->SearchTerm->clear();
    ui->SearchFilter->setDisabled(true);
    ui->SearchFilter->setCurrentIndex(3);
    //ui->HighlightTerm->setDisabled(true);
    ui->SearchCount->clear();


    // Clear the ID list so we don't have data from previous connections on it. That sure would cause problems....
    IDList.clear();

    // flip the connection active switch
    ConnectionActive=false;


    ui->StatusMessage->setText("Terminated connection with <b>" + Buffer::host + "</b>");
    ui->TotalCount->clear();
    ui->TotalCount->setToolTip(NULL);

    ui->Status_User->clear();

    // Clean buffered entry text
    Buffer::SearchText.clear();
    Buffer::EntryText.clear();

    // Re-lock search tab
    ui->tabWidget->setTabEnabled(1,false);

    // new for 0.4.1: change tooltip text on disconnect
    ui->tabWidget->setTabToolTip(1,"Search for entries containing specific patterns or tags. "
                                 "<span style=\"color: red\">(requires active connection)</span>");

    /* Bugfix 2/10/13: revert title bar after disconnect. Revert if the name_in_titlebar flag is still active OR
      if the user disabled the flag during the current connection (otherwise it won't get reverted).
    */
    if((Buffer::name_in_titlebar) || (windowTitle()== Buffer::database_name + " - RoboJournal")){
        this->setWindowTitle("RoboJournal");
    }

    //clean out list of untagged entries in buffer. This prevents the list from having items from
    // multiple sessions.
    Buffer::records.clear();

    if(Buffer::backend=="MySQL"){
        MySQLCore m;
        m.Disconnect();
    }

    // Reset the date override trigger thingy that is used to get the date of the most recent entry
    // for comparison to the date override value (9/12/13).
    if(Buffer::datebox_override)
        date_override_trigger_tripped=false;

}

//################################################################################################
// show preferences window
void MainWindow::Preferences(){
    Newconfig c(this);
    //c.setWindowTitle("RoboJournal Preferences");
    c.exec();
    launched_config=true;

    if(c.MadeChanges){
        SettingsManager s;
        s.LoadConfig(false);

        if((!ConnectionActive) && (ui->DisconnectButton->isEnabled())){
            CreateTree();
            ui->StatusMessage->setText("Connected to " + Buffer::backend  + " database <b>" +
                                       Buffer::database_name + "</b> on <b>" +  Buffer::host);
        }

        // Setup toolbar position again in case user changed it
        switch(Buffer::toolbar_pos){

        case 0:
            this->addToolBar(Qt::LeftToolBarArea,ui->mainToolBar);
            break;

        case 1:
            this->addToolBar(Qt::TopToolBarArea,ui->mainToolBar);
            break;

        case 2:
            this->addToolBar(Qt::RightToolBarArea,ui->mainToolBar);
            break;
        }
    }

    //Re-decorate the GUI
    Decorate_GUI();

    //Disable the Tagger and Exporter momentarily because RJ will crash if people click on these w/o
    // an entry selected.
    // bugfix (8/15/12) Prevent crash by disabling the tagger and entryexporter after posting an entry.
    ui->Tag->setDisabled(true);
    ui->ExportEntry->setDisabled(true);
    ui->actionManage_Tags_2->setDisabled(true);
    ui->actionExport_Entry->setDisabled(true);
}

//################################################################################################
// show credits window
void MainWindow::ShowCredits(){
    AboutRJ a(this);

    int width=a.width();
    int height=a.height();
    a.setMaximumSize(width, height);
    a.setMinimumSize(width, height);
    a.setWindowTitle("About RoboJournal");
    a.exec();
}

//################################################################################################
// Call editor dialog class
void MainWindow::Write(){
    using namespace std;

    this->setCursor(Qt::WaitCursor);
    ui->statusBar->showMessage("Preparing a new entry, please wait...",0);

    // Start Editor in new entry mode
    Buffer::editmode=false;


    // Declare and show a journal editor object
    Editor e(this);


    e.setWindowTitle("Untitled Entry - RoboJournal");
    e.exec();
    launched_editor=true;

    // Refresh the entry list
    cout << "OUTPUT: Re-creating entry list to include newest entry." << endl;
    CreateTree();

    // rebuild ID list
    MySQLCore my;

    IDList.clear();
    IDList=my.Create_ID_List(Buffer::entryrange);

    // Reset Status bar message
    ui->StatusMessage->setText("Connected to " + Buffer::backend  + " database <b>" +
                               Buffer::database_name + "</b> on <b>" +  Buffer::host);

    // bugfix (8/15/12) Prevent crash by disabling the tagger and entryexporter after posting an entry.
    // the app will segfault if the user clicks on those buttons if there's no entry selected.
    ui->Tag->setDisabled(true);
    ui->ExportEntry->setDisabled(true);
    ui->actionManage_Tags_2->setDisabled(true);
    ui->actionExport_Entry->setDisabled(true);


    this->setCursor(Qt::ArrowCursor);
    ui->statusBar->clearMessage();
}

//################################################################################################
// Function that highlights the tree item corresponding to current entry being viewed
void MainWindow::HighlightCurrentSelection(QString CurrentID){
    using namespace std;
    ui->EntryList->clearSelection();
    QTreeWidgetItemIterator it(ui->EntryList,QTreeWidgetItemIterator::NoChildren);

    while (*it) {
        if ((*it)->text(1) == CurrentID)

            (*it)->setSelected(true);

        ++it;

        // Make sure CurrentID always reflects current highlighted index
        // This is necessary in case the user wants to edit an entry

        this->CurrentID=CurrentID;
    }

    // make sure that the currentitem and selecteditem are always the same.
    // if not, the Entryexporter and Tagger usually get out of sync with the entry being viewed.
    QList<QTreeWidgetItem *> current=ui->EntryList->selectedItems();
    ui->EntryList->setCurrentItem(current[0]);

    QTreeWidgetItem *selected=ui->EntryList->currentItem();

    // forward the current selected entry to entry exporter
    if(selected != NULL){
        EntryExporter e;

        // get title
        QString title=selected->text(0);
        title=title.trimmed();
        title=title.simplified();

        // process timestamp

        if(current_entry_time.isEmpty()){
            if(Buffer::backend=="MySQL"){
                MySQLCore d;
                current_entry_time=d.GetTimestamp(selected->text(1));

            }
        }
        e.UpdateValues(title,current_entry_date,current_entry_body,current_entry_time);
    }

    ui->Tag->setEnabled(true);
    ui->ExportEntry->setEnabled(true);
    ui->actionManage_Tags_2->setEnabled(true);
    ui->actionExport_Entry->setEnabled(true);


}

//################################################################################################
// Return the long date from the short date. (int) This was originally done more than once
// in CreateTree() so it made sense to split it into a separate function to prevent redundant code

QString MainWindow::GetLongMonth(int month){
    QString longmonth;
    switch(month){
    case 1:
        longmonth="January";
        break;

    case 2:
        longmonth="February";
        break;

    case 3:
        longmonth="March";
        break;

    case 4:
        longmonth="April";
        break;

    case 5:
        longmonth="May";
        break;

    case 6:
        longmonth="June";
        break;

    case 7:
        longmonth="July";
        break;

    case 8:
        longmonth="August";
        break;

    case 9:
        longmonth="September";
        break;

    case 10:
        longmonth="October";
        break;

    case 11:
        longmonth="November";
        break;

    case 12:
        longmonth="December";
        break;

    }

    return longmonth;
}


//################################################################################################
// All-important function that sets up the Entry tree list.
void MainWindow::CreateTree(){
    using namespace std;
    ui->StatusMessage->setText("Populating entry list, please wait...");
    QIcon rooticon(":/icons/database.png");
    //QIcon monthicon(":/icons/bullet_black.png");
    //QIcon yearicon(":/icons/bullet_blue.png");


    QString iconpath;

    switch(Buffer::entry_node_icon){
    case 0:
        iconpath=":/icons/pencil.png";
        break;

    case 1:
        iconpath=":/icons/pencil2.png";
        break;

    case 2:
        iconpath=":/icons/pencil-small.png";
        break;

    case 3:
        iconpath=":/icons/balloon-quotation.png";
        break;

    case 4:
        iconpath=":/icons/node.png";
        break;

    case 5:
        iconpath=":/icons/bullet_black.png";
        break;

    case 6:
        iconpath=":/icons/bullet_blue.png";
        break;
    }

    QIcon entryicon(iconpath);

    // get rid of indicator (expand/collapse widget) on root node. Fixed in version 0.3.
    ui->EntryList->setRootIsDecorated(false);

    QFont heavy("sans",10);
    heavy.setWeight(QFont::DemiBold);

    int totalcount=0;

    // clear message and tree panes
    ui->EntryList->clear();

    if(!Buffer::On_Search){
        ui->Output->clear();
    }


    // MySQL code
    if(Buffer::backend=="MySQL"){

        // sort entries by day
        if(Buffer::sortbyday){
            cout << "OUTPUT: Creating entry list from contents of MySQL database " << Buffer::database_name.toStdString() << " ...";
            MySQLCore my;

            bool NewJournal=true;

            QList<QString> YearList;
            QList<QString> MonthList;
            QList<QString> DayList;
            QList<QString> EntryList;

            YearList=my.getYear();


            QListIterator<QString> IteratorYear(YearList);


            //ui->EntryList->setHeaderLabel("Database contents (chronological)");
            //ui->EntryList->setToolTip(NULL);
            //ui->EntryList->setHeaderHidden(false);

            QString db=Buffer::database_name;


            QTreeWidgetItem *root = new QTreeWidgetItem(ui->EntryList);
            root->setText(0, db);
            root->setIcon(0,rooticon);
            root->setToolTip(0, "<b>" + Buffer::database_name + "</b>@<b>" + Buffer::host + "</b>");

            for(int y=0; y<YearList.count(); y++){
                QTreeWidgetItem *year = new QTreeWidgetItem(root);

                QString nextyear=IteratorYear.next();
                year->setText(0, nextyear);


                MonthList=my.getMonth(nextyear);
                QListIterator<QString> IteratorMonth(MonthList);


                for(int m=0; m < MonthList.count(); m++){
                    QTreeWidgetItem *month = new QTreeWidgetItem(year);
                    //month->setFont(0,heavy);

                    //month->setIcon(0,monthicon);

                    QString itemmonth=IteratorMonth.next();

                    //cout << "Item month:" << itemmonth.toStdString() <<endl;
                    int switchmonth=itemmonth.toInt();

                    QString longmonth=GetLongMonth(switchmonth);


                    DayList=my.getDay(itemmonth,nextyear);
                    //cout << "Daylist: " << DayList.length();
                    QListIterator<QString> IteratorDay(DayList);


                    int EntryCount=0; // keep track of the number of entries for each month

                    for(int d=0; d<DayList.length(); d++){
                        QString itemday=IteratorDay.next();
                        QTreeWidgetItem *day = new QTreeWidgetItem(month);

                        itemday=itemday.trimmed();

                        // BUGFIX for version 0.5 (6/11/13):
                        // Ensure all strings are trimmed to get rid of any unwanted spaces or linebreaks.
                        // This debris sometimes gets introduced when the database is restored from a dump file
                        // and it throws the entire tree off.

                        // Organize Date depending on Buffer data
                        switch(Buffer::date_format){

                        case 0: // international
                            day->setText(0, itemday.trimmed() + " " + longmonth.trimmed()  );
                            break;

                        case 1:  // usa
                            day->setText(0,longmonth.trimmed() + " " + itemday.trimmed());
                            break;

                        case 2: // japan
                            day->setText(0,longmonth.trimmed() + " " + itemday.trimmed());
                            break;
                        }

                        EntryList=my.getEntries(itemday,itemmonth);

                        QListIterator<QString> IteratorEntry(EntryList);

                        for(int e=0; e<EntryList.length(); e++){
                            QString entry=IteratorEntry.next();
                            QTreeWidgetItem *EntryItem = new QTreeWidgetItem(day);
                            QStringList item=entry.split("|");
                            QString tooltip=longmonth + " " + itemday + ", " +
                                    nextyear + " : " + item[0];


                            EntryItem->setText(0, item[0]);
                            EntryItem->setToolTip(0, tooltip);
                            EntryItem->setText(1, item[1]);
                            EntryItem->setIcon(0,entryicon);

                            // New for 0.5: Check to see if this is the most recent entry.
                            if((e==0) && (Buffer::datebox_override) && (!date_override_trigger_tripped)){
                                int month=0, day=0, year=0;

                                year=EntryItem->parent()->parent()->text(0).toInt();
                                day=itemday.toInt();

                                if(longmonth=="January")
                                    month=1;

                                if(longmonth=="February")
                                    month=2;

                                if(longmonth=="March")
                                    month=3;

                                if(longmonth=="April")
                                    month=4;

                                if(longmonth=="May")
                                    month=5;

                                if(longmonth=="June")
                                    month=6;

                                if(longmonth=="July")
                                    month=7;

                                if(longmonth=="August")
                                    month=8;

                                if(longmonth=="September")
                                    month=9;

                                if(longmonth=="October")
                                    month=10;

                                if(longmonth=="November")
                                    month=11;

                                if(longmonth=="December")
                                    month=12;

                                QDate mostrecent;
                                mostrecent.setDate(year,month,day);
                                cout << mostrecent.toString("Recent date: mm/dd/yyyy").toStdString() << endl;
                                date_override_trigger_tripped=true;
                            }

                            // since we know there are entries at this point, NewJournal should be set to false.
                            NewJournal=false;


                            EntryCount++; // auto-increment entry count

                            totalcount++; // update totalcount
                        }

                        //if EntryCount==0, we have a new journal. Show a message if this happens.
                        if(EntryCount==0){
                            QMessageBox a;
                            a.information(this,"RoboJournal","Welcome to your new journal! Click <b>Write in Journal</b> (or press F4) to get started.");
                        }

                        EntryList.clear();

                    }
                    DayList.clear();

                    month->setText(0, longmonth);
                    QString monthcount=QString::number(EntryCount);

                    if(EntryCount==1){
                        month->setToolTip(0, longmonth + " " + nextyear +
                                          " (" + monthcount + " entry)");
                    }
                    else{
                        month->setToolTip(0, longmonth + " " + nextyear +
                                          " (" + monthcount + " entries)");
                    }


                    EntryCount=0;
                }
                MonthList.clear();
            }

            //if NewJournal is still true, that means we have a new journal. Show a message if this happens.
            if(NewJournal){
                root->setHidden(true);
                QMessageBox a;
                a.information(this,"RoboJournal","Welcome to your new journal! Click <b>Write in Journal</b> (or press F4) to get started.");
            }

            YearList.clear();

            TotalEntryCount(totalcount);


        }

        //#################################################
        //          Sort entries by month only
        //#################################################

        else{
            cout << "OUTPUT: Creating entry list from contents of MySQL database " << Buffer::database_name.toStdString() << " ...";
            MySQLCore my;

            bool NewJournal=true;

            QList<QString> YearList;
            QList<QString> MonthList;
            QList<QString> EntryList;

            YearList=my.getYear();


            QListIterator<QString> IteratorYear(YearList);


            //ui->EntryList->setHeaderLabel("Database contents (chronological)");
            //ui->EntryList->setToolTip(NULL);
            //ui->EntryList->setHeaderHidden(false);

            QString db=Buffer::database_name;


            QTreeWidgetItem *root = new QTreeWidgetItem(ui->EntryList);


            root->setText(0, db);
            root->setIcon(0,rooticon);
            root->setToolTip(0, "<b>" + Buffer::database_name + "</b>@<b>" + Buffer::host + "</b>");
            //root->setFont(0,heavy);



            for(int y=0; y<YearList.count(); y++){
                QTreeWidgetItem *year = new QTreeWidgetItem(root);
                //year->setIcon(0,yearicon);

                QString nextyear=IteratorYear.next();
                year->setText(0, nextyear);
                //year->setFont(0,heavy);

                //cout << "Current Year: " << nextyear.toStdString() << endl;

                MonthList=my.getMonth(nextyear);
                QListIterator<QString> IteratorMonth(MonthList);


                for(int m=0; m < MonthList.count(); m++){
                    QTreeWidgetItem *month = new QTreeWidgetItem(year);
                    //month->setFont(0,heavy);

                    //month->setIcon(0,monthicon);

                    QString itemmonth=IteratorMonth.next();

                    //cout << "Item month:" << itemmonth.toStdString() <<endl;
                    int switchmonth=itemmonth.toInt();

                    QString longmonth=GetLongMonth(switchmonth);

                    EntryList=my.getEntriesMonth(itemmonth,nextyear);
                    //cout << "Daylist: " << DayList.length();
                    QListIterator<QString> IteratorEntry(EntryList);

                    int EntryCount=0; // keep track of the number of entries for each month

                    for(int e=0; e<EntryList.length(); e++){
                        QString raw_entry=IteratorEntry.next();
                        //cout << entry.toStdString() << endl;
                        QTreeWidgetItem *EntryItem = new QTreeWidgetItem(month);

                        QStringList item=raw_entry.split(",");

                        // Organize Date depending on Buffer data


                        QString entry;

                        switch(Buffer::date_format){

                        // BUGFIX for version 0.5 (6/11/13):
                        // Ensure all strings are trimmed to get rid of any unwanted spaces or linebreaks.
                        // This debris sometimes gets introduced when the database is restored from a dump file
                        // and it throws the entire tree off.

                        case 0: // international
                            entry=item[2].trimmed() + " " + longmonth.trimmed() + ": " + item[1].trimmed();
                            break;

                        case 1:  // usa
                            entry=longmonth.trimmed() + " " + item[2].trimmed() + ": " + item[1].trimmed();
                            break;

                        case 2: // japan
                            entry=longmonth.trimmed() + " " + item[2].trimmed() + ": " + item[1].trimmed();
                            break;
                        }

                        EntryItem->setText(0,entry);
                        EntryItem->setToolTip(0,entry);

                        EntryItem->setText(1,item[0]);
                        EntryItem->setIcon(0, entryicon);

                        // New for 0.5: Check to see if this is the most recent entry (9/12/13).
                        // If so, set the lastdate value with the value if date override is enabled.
                        if((e==0) && (Buffer::datebox_override) && (!date_override_trigger_tripped)){
                            int month=0, day=0, year=0;

                            year=EntryItem->parent()->parent()->text(0).toInt();
                            day=item[2].toInt();

                            if(longmonth=="January")
                                month=1;

                            if(longmonth=="February")
                                month=2;

                            if(longmonth=="March")
                                month=3;

                            if(longmonth=="April")
                                month=4;

                            if(longmonth=="May")
                                month=5;

                            if(longmonth=="June")
                                month=6;

                            if(longmonth=="July")
                                month=7;

                            if(longmonth=="August")
                                month=8;

                            if(longmonth=="September")
                                month=9;

                            if(longmonth=="October")
                                month=10;

                            if(longmonth=="November")
                                month=11;

                            if(longmonth=="December")
                                month=12;

                            lastdate.setDate(year,month,day);
                            //cout << " Recent date: " << lastdate.toString("M/dd/yyyy").toStdString() << endl;
                            date_override_trigger_tripped=true;
                        }

                        // since we know there are entries at this point, NewJournal should be set to false.
                        NewJournal=false;

                        EntryCount++; // auto-increment entry count

                        totalcount++; // update totalcount
                    }

                    EntryList.clear();

                    QString monthcount=QString::number(EntryCount);

                    month->setText(0, longmonth);

                    // do month post count
                    if(EntryCount==1){
                        month->setToolTip(0, longmonth + " " + nextyear +
                                          " (" + monthcount + " entry)");
                    }
                    else{
                        month->setToolTip(0, longmonth + " " + nextyear +
                                          " (" + monthcount + " entries)");
                    }

                    EntryCount=0;
                }
                MonthList.clear();
            }
            YearList.clear();

            TotalEntryCount(totalcount);

            //if NewJournal is still true, that means we have a new journal. Show a message if this happens.
            if(NewJournal){
                root->setHidden(true);
                QMessageBox a;
                a.information(this,"RoboJournal","Welcome to your new journal! Click <b>Write in Journal</b> (or press F4) to get started.");
            }

        }
    }
    ui->EntryList->expandAll();
    cout << "Done!" << endl;

    // re-enable a few buttons just in case they were disabled.
    // (e.g. we log into a new journal and then switch to an existing one)

    if(num_records > 0){
        ui->TodayButton->setEnabled(true);
        ui->NextEntry->setEnabled(true);
        ui->LastEntry->setEnabled(true);
        ui->DeleteEntry->setEnabled(true);
        ui->EditEntryButton->setEnabled(true);
        ui->actionLatest_Entry->setDisabled(false);
        ui->actionNext_Entry->setDisabled(false);
        ui->actionPrevious_Entry->setDisabled(false);
        ui->actionDelete_Current_Entry->setDisabled(false);
        ui->actionEdit_Selected_Entry->setDisabled(false);
    }
    else{
        ui->TodayButton->setEnabled(false);
        ui->NextEntry->setEnabled(false);
        ui->LastEntry->setEnabled(false);
        ui->DeleteEntry->setEnabled(false);
        ui->EditEntryButton->setEnabled(false);
        ui->actionLatest_Entry->setDisabled(true);
        ui->actionNext_Entry->setDisabled(true);
        ui->actionPrevious_Entry->setDisabled(true);
        ui->actionDelete_Current_Entry->setDisabled(true);
        ui->actionEdit_Selected_Entry->setDisabled(true);

    }
}

//################################################################################################
// Update status bar label with total entry count
void MainWindow::TotalEntryCount(int totalcount){

    QString count=QString::number(totalcount);

    // update num_count. If num_count==0, the new journal notification will show and we will not
    //jump to first entry at login
    num_records=totalcount;

    ui->TotalCount->setTextFormat(Qt::RichText);

    // Note: Use space characters here instead of HTML nonbreaking space. For some reason, "&nbsp" shows up.
    if(totalcount==1){
        ui->TotalCount->setText("<img src=\":/icons/page_white_database.png\">&nbsp;&nbsp;" + count  + " entry&nbsp;");
    }
    else{
        ui->TotalCount->setText("<img src=\":/icons/page_white_database.png\">&nbsp;&nbsp;" + count  + " entries&nbsp;");
    }

    QString indicator;

    // create status bar messages
    if(Buffer::allentries){
        indicator="<nobr>Total number of entries to date in <b>" + Buffer::database_name + "</b></nobr>";
        ui->TotalCount->setToolTip(indicator);
        //ui->statusBar->showMessage("Displaying all entries in the database...",1500);
    }
    else{
        if(Buffer::entryrange==1){

            //QFrame *div = new QFrame(this);
            //div->setFrameStyle();

            indicator="<nobr>Total number of entries for current range (" +
                    QString::number(Buffer::entryrange) + " year) in <b>" + Buffer::database_name + "</b></nobr>";
            ui->TotalCount->setToolTip(indicator);
            //ui->statusBar->showMessage("Displaying all entries from this year...",1500);


            // Add year indicator if user decides to use it


            if(Buffer::use_indicator){

                ui->TotalCount->setText(ui->TotalCount->text() +  "<b>:</b>" +
                                        "&nbsp;<img src=\":/icons/arrow-repeat.png\">&nbsp;1 year");
            }
        }
        else{
            indicator="<nobr>Total number of entries for current range (" +
                    QString::number(Buffer::entryrange) + " years) in <b>" + Buffer::database_name + "</b></nobr>";
            ui->TotalCount->setToolTip(indicator);
            //ui->statusBar->showMessage("Displaying all entries from the last " + Buffer::entryrange
            //+ " years...",1500);


            // Add year indicator if user decides to use it
            if(Buffer::use_indicator){

                QString range=QString::number(Buffer::entryrange);
                ui->TotalCount->setText(ui->TotalCount->text() +  "<b>:</b>" + "&nbsp;<img src=\":/icons/arrow-repeat.png\">&nbsp;"
                                        + range + " years&nbsp;");
            }
        }
    }
}

//################################################################################################
// Get the most recent entry from the db
void MainWindow::MostRecent(){
    if(Buffer::backend=="MySQL"){
        MySQLCore c;
        QString body=c.Recent();

        // convert plain text linebreaks to html
        body.replace("\n","<br>");

        // find and use system colors
        QPalette pal;
        QBrush bg=pal.highlight();
        QBrush fg=pal.highlightedText();

        QColor bgcolor=bg.color();
        QColor fgcolor=fg.color();

        QString color1=bgcolor.name();
        QString color2=fgcolor.name();

        //QString timestamp=c.TimeStamp(id);

        // get tags
        QString tags=c.GetTags(c.recordnum);

        //format tags
        QString output_tags=FormatTags(tags, color1,color2);

        QString username=Buffer::username;

        if(Buffer::use_full_name){
            username=Buffer::full_name;
        }

        // display title above entry
        if(Buffer::show_title){

            QString id=c.recordnum;
            QString title=c.GetTitle(id);
            title="<h2>" + title + "</h2>";

            if((Buffer::use_custom_theme) && (!Buffer::datebox_override)){
                QString invertcolor=InvertColor(Buffer::text_hexcolor);
                ui->Output->setText(title + "<div style=\"background-color: " + Buffer::text_hexcolor +
                                    "; color: " + invertcolor + "; width=100%; \"><small>&nbsp;&nbsp; " +
                                    username + " wrote:</small></div><div><br>" + body   + "<br>" +
                                    output_tags + "</div>");
            }
            else{
                ui->Output->setText(title + "<div style=\"background-color: " + color1 +
                                    "; color: " + color2 + "; width=100%; \"><small>&nbsp;&nbsp; " +
                                    username + " wrote:</small></div><div><br>" + body  + "<br>" +
                                    output_tags  + "</div>");
            }

        }
        // don't display title
        else{
            if((Buffer::use_custom_theme) && (!Buffer::datebox_override)){
                QString invertcolor=InvertColor(Buffer::text_hexcolor);
                ui->Output->setText("<div style=\"background-color: " + Buffer::text_hexcolor +
                                    "; color: " + invertcolor + "; width=100%; \"><small>&nbsp;&nbsp; " +
                                    username + " wrote:</small></div><div><br>" + body  + "<br>" +
                                    output_tags +"</div> ");
            }
            else{
                ui->Output->setText("<div style=\"background-color: " + color1 +
                                    "; color: " + color2 + "; width=100%; \"><small>&nbsp;&nbsp; " +
                                    username + " wrote:</small></div><div><br>" + body  +
                                    "<br>" + output_tags + "</div>");
            }

        }

        QString text=ui->Output->toHtml();
        Buffer::EntryText=text;


        // Set Current ID
        ui->NextEntry->setDisabled(true);
        CurrentID=QString::number(c.ID);


        // Highlight Current Entry
        HighlightCurrentSelection(CurrentID);

        QTreeWidgetItem *selected=ui->EntryList->currentItem();
        //EntryExporter e(this);
        EntryExporter::title=selected->text(0);
    }
}
//################################################################################################
void MainWindow::GetPrevious(){
    if(Buffer::backend=="MySQL"){
        MySQLCore c;
        QString body=c.GetPrevious();
        ui->Output->setText(body);
    }
}
//################################################################################################
MainWindow::~MainWindow()
{

    // save window sizes
    SettingsManager s;

    if((Buffer::editor_sizechanged) && (launched_editor)){
        s.SaveEditorSize(Buffer::editor_temporarysize);
    }

    if((Buffer::config_sizechanged) && (launched_config)){
        s.SaveConfigSize(Buffer::config_temporarysize);
    }

    // only save state if window is not maximized.
    if(!this->isMaximized()){
        //Save current window size before destroying window.
        QByteArray geo=this->saveGeometry();

        s.SaveMainWindowSize(geo);
    }

    // 6/21/13: remember the splitter position if it has changed.
    QByteArray current=ui->splitter->saveState();
    if(current != Buffer::mw_splitter_size){
        s.SaveSplitterPos(current);
    }

    delete ui;
}
//################################################################################################
void MainWindow::on_WriteButton_clicked()
{
    Write();
}

//################################################################################################

void MainWindow::on_SearchButton_clicked()
{
    SearchDatabase();
}
//################################################################################################


void MainWindow::on_ConnectButton_clicked()
{
    Connect();
}
//################################################################################################
void MainWindow::on_DisconnectButton_clicked()
{
    Disconnect();

}

//################################################################################################

void MainWindow::on_ConfigButton_clicked()
{
    Preferences();
}
//################################################################################################

void MainWindow::on_actionPreferences_2_triggered()
{
    Preferences();
}
//################################################################################################
void MainWindow::on_actionConnect_triggered()
{
    Connect();
}
//################################################################################################
void MainWindow::on_actionToolbar_triggered(bool checked)
{
    if(!checked){
        ui->mainToolBar->hide();
    }
    else{
        ui->mainToolBar->show();
    }

}

//################################################################################################

void MainWindow::on_actionCopy_triggered()
{
    ui->Output->copy();
}
//################################################################################################
void MainWindow::on_actionSelect_all_triggered()
{
    ui->Output->selectAll();
}
//################################################################################################
void MainWindow::on_actionMain_toolbar_triggered(bool checked)
{
    if(!checked){
        ui->mainToolBar->hide();
    }
    else{
        ui->mainToolBar->show();
    }
}
//################################################################################################
void MainWindow::on_actionLeft_default_triggered()
{
    this->addToolBar(Qt::LeftToolBarArea,ui->mainToolBar);

    if(Buffer::show_icon_labels){
        ui->ConnectButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->DisconnectButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->LastEntry->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->WriteButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->EditEntryButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->DeleteEntry->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->NextEntry->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->TodayButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->ConfigButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->Tag->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->ExportEntry->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
}
//################################################################################################
void MainWindow::on_actionTop_triggered()
{
    this->addToolBar(Qt::TopToolBarArea,ui->mainToolBar);

    if(Buffer::show_icon_labels){
        ui->ConnectButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->DisconnectButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->LastEntry->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->WriteButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->EditEntryButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->DeleteEntry->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->NextEntry->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->TodayButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->ConfigButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->Tag->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        ui->ExportEntry->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
}
//################################################################################################
void MainWindow::on_actionRight_triggered()
{
    this->addToolBar(Qt::RightToolBarArea,ui->mainToolBar);

    if(Buffer::show_icon_labels){
        ui->ConnectButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->DisconnectButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->LastEntry->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->WriteButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->EditEntryButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->DeleteEntry->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->NextEntry->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->TodayButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->ConfigButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->Tag->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->ExportEntry->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
}
//################################################################################################
void MainWindow::on_actionQuit_triggered()
{

    this->close();
}
//################################################################################################
void MainWindow::on_actionAbout_RoboJournal_triggered()
{
    ShowCredits();
}
//################################################################################################
void MainWindow::on_TodayButton_clicked()
{
    MostRecent();
}
//################################################################################################
void MainWindow::on_LastEntry_clicked()
{
    //GetPrevious();
    int direction=0;
    GetAdjacent(direction);
}

//################################################################################################

void MainWindow::on_actionLatest_Entry_triggered()
{
    MostRecent();
}
//################################################################################################
void MainWindow::on_actionDisconnect_triggered()
{
    Disconnect();
}

//################################################################################################
// Function that retrieves an entry when its entry is clicked on
void MainWindow::on_EntryList_itemClicked(QTreeWidgetItem *item)
{

    using namespace std;

    // make sure selected item is an entry and not just another subnode
    if(item->child(0)==NULL){
        QString id=item->text(1);

        GetEntry(id,false);

        // update selected entry
        UpdateSelectedEntry(id);

        // re-enable tagger and Exporter if we click on a valid entry

        if((!ui->Tag->isEnabled()) || (!ui->ExportEntry->isEnabled())){

            ui->Tag->setEnabled(true);
            ui->ExportEntry->setEnabled(true);
            ui->actionManage_Tags_2->setEnabled(true);
            ui->actionExport_Entry->setEnabled(true);
        }

    }
    else{
        //cout << "This is **NOT** an entry!" << endl;
        CurrentID="-1";
    }
}



//################################################################################################
// Function that moves to the next adjacent entry based on row ID.
// direction 1=forward; direction 0 =backward;
void MainWindow::GetAdjacent(int direction){

    using namespace std;
    if(Record!=NULL){
        QListIterator<QString> browser(IDList);
        using namespace std;

        //go backward one entry
        if(direction==0){

            if(!ui->NextEntry->isEnabled()){
                ui->NextEntry->setEnabled(true);
                ui->actionNext_Entry->setEnabled(true);
            }

            int position=IDList.indexOf(Record);
            position--;
            //cout << position << endl;

            QString lastindex=IDList.value(position);
            QString id=lastindex;
            //cout << lastindex.toStdString() << endl;

            if(position<0){
                ui->LastEntry->setDisabled(true);
                ui->actionPrevious_Entry->setDisabled(true);
                if(!ui->NextEntry->isEnabled()){
                    ui->NextEntry->setEnabled(true);
                    ui->actionNext_Entry->setEnabled(true);
                }
            }
            else{

                if(CurrentID != "-1"){
                    GetEntry(id,false);
                    Record=lastindex;
                }
            }


        }
        // go forward one entry
        if(direction==1){

            if(browser.next()==NULL){
                ui->NextEntry->setDisabled(true);
                ui->actionNext_Entry->setDisabled(true);
            }
            else{
                if(!ui->LastEntry->isEnabled()){
                    ui->LastEntry->setEnabled(true);
                    ui->actionPrevious_Entry->setEnabled(true);
                }

                int position=IDList.indexOf(Record);
                position++;

                QString nextindex=IDList.value(position);
                QString id=nextindex;
                //cout << nextindex.toStdString() << endl;

                if(position==IDList.size()){
                    ui->NextEntry->setDisabled(true);
                    ui->actionNext_Entry->setDisabled(true);
                    if(!ui->LastEntry->isEnabled()){
                        ui->LastEntry->setEnabled(true);
                        ui->actionPrevious_Entry->setEnabled(true);
                    }
                }
                else{

                    if(CurrentID != "-1"){
                        GetEntry(id,false);
                        Record=nextindex;
                    }
                }
            }
        }
    }

    if(CurrentID != "-1"){
        HighlightCurrentSelection(Record);
    }


}

//################################################################################################

void MainWindow::on_NextEntry_clicked()
{
    int direction=1;
    GetAdjacent(direction);
}
//################################################################################################
void MainWindow::on_actionPrevious_Entry_triggered()
{
    int direction=0;
    GetAdjacent(direction);
}
//################################################################################################
void MainWindow::on_actionNext_Entry_triggered()
{
    int direction=1;
    GetAdjacent(direction);
}

//################################################################################################

void MainWindow::on_actionEdit_Selected_Entry_triggered()
{
    Modify();
}

//################################################################################################

void MainWindow::on_actionSetup_Wizard_2_triggered()
{

    FirstRun f;


    if(Buffer::showwarnings){
        QMessageBox m;
        int reconfigure=m.question(this,"Reconfigure RoboJournal?",
                                   "Running the RoboJournal Setup Wizard again will purge all existing configuration settings. "
                                   "(however, any journal databases you have already created will be preserved) "
                                   " Are you sure you want to do this?",QMessageBox::Cancel,QMessageBox::Ok);

        switch(reconfigure){
        case QMessageBox::Ok:
            f.exec();
            break;

        case QMessageBox::Cancel:
            // do nothing
            break;
        }
    }
    else{
        f.exec();
    }
}

//################################################################################################
void MainWindow::on_actionPrint_triggered()
{
    if(ui->Output->toPlainText()!=NULL){


        Print();
        //ui->Output->print();
    }
}

//################################################################################################

void MainWindow::on_DeleteEntry_clicked()
{
    DeleteSelectedEntry();

}

//################################################################################################
void MainWindow::on_actionDelete_Current_Entry_triggered()
{
    DeleteSelectedEntry();

}

//################################################################################################
void MainWindow::on_actionWrite_triggered()
{
    Write();
}
//################################################################################################
void MainWindow::on_actionHelp_file_triggered()
{
    ShowHelp();
}
//################################################################################################
void MainWindow::on_EditEntryButton_clicked()
{
    Modify();
}
//################################################################################################
void MainWindow::on_Tag_clicked()
{

    Tag();
}
//################################################################################################
void MainWindow::on_actionManage_Tags_2_triggered()
{
    Tag();
}

void MainWindow::on_actionSelect_Default_Journal_triggered()
{
    if((ui->DisconnectButton->isEnabled()) || (Buffer::On_Search)){
        //Disconnect first

        // Switch back to Entry mode before disconnecting so we don't mess up which buttons should be enabled
        if(Buffer::On_Search){
            ui->tabWidget->setCurrentIndex(0);
            Disconnect();
        }
        else{
            Disconnect();
        }
    }

    // launch JournalSelector
    JournalSelector j(this);
    j.exec();
}

void MainWindow::on_ExportEntry_clicked()
{
    ExportEntry();
}

void MainWindow::on_actionExport_Entry_triggered()
{
    ExportEntry();
}

void MainWindow::on_actionEntry_List_toggled(bool arg1)
{
    QList<int> minimized;
    minimized.append(0);
    minimized.append(100);


    if(arg1){
        if(!Buffer::mw_splitter_size.isEmpty()){
            ui->splitter->restoreState(Buffer::mw_splitter_size);
        }
        else{
            QList<int> size;
            size.append(1);
            size.append(splittersize); // was originally 225 but fixed values dont work very well
            ui->splitter->setSizes(size);
        }
    }
    else{
        ui->splitter->setSizes(minimized);
    }
}

void MainWindow::on_SearchFilter_currentIndexChanged(const QString &arg1)
{
    ui->SearchFilter->setToolTip(arg1);

    if(arg1=="Search by Tag"){
        ui->SearchTerm->setDisabled(true);
        ui->TagList->setDisabled(false);
        ui->WholeWords->setDisabled(true);
        ui->TagList->clear();

        ui->TagList->setToolTip("<p>Click <b>Search</b> to display all entries marked with the currently-selected tag.</p>");
        ui->SearchTerm->setToolTip("<p>The search pattern input field is not available in tag search mode.</p>");
        GetTagList();
        Buffer::LastSearchTerm.clear();
    }
    else{
        ui->SearchTerm->setDisabled(false);
        ui->TagList->setDisabled(true);
        ui->TagList->setToolTip("<p>Tag search is not available in pattern-match mode.</p>");
        ui->SearchTerm->setToolTip("<p>Enter the pattern (one or more words) you wish to search for in this field and click the <b>Search</b> button to continue.<br><br>FYI: The pattern search function is case-sensitive!</p>");
        ui->WholeWords->setDisabled(false);
        ui->SearchTerm->setFocus();
        ui->TagList->clear();
        Buffer::LastSearchTerm.clear();
    }
}

void MainWindow::on_ClearButton_clicked()
{
    ClearSearchResults();
}

void MainWindow::on_actionJournal_Creator_triggered()
{
    //Disconnect first
    if((ui->DisconnectButton->isEnabled()) || (Buffer::On_Search)){

        // Switch back to Entry mode before disconnecting so we don't mess up which buttons should be enabled
        if(Buffer::On_Search){
            ui->tabWidget->setCurrentIndex(0);
            Disconnect();
        }
        else{
            Disconnect();
        }
    }

    //JournalCreator jc(this);
    NewJournalCreator jc(this);
    jc.exec();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    QScrollBar *vbar = ui->Output->verticalScrollBar();

    switch(index){

    case 0:
        Buffer::search_scroll_pos=vbar->value();
        break;

    case 1:
        Buffer::entry_scroll_pos=vbar->value();
        break;
    }

    SwitchTab(index);
}

void MainWindow::on_WholeWords_clicked()
{
    // Every time this box gets checked, clean the last word searched so we can search again. This is important so
    // we can do whole word or grep search even if the search term itself has not changed.
    Buffer::LastSearchTerm.clear();

    // bugfix (2/10/13): Auto-focus the search term again in order to save the user a click.
    // This allows the user to just press Enter to search again. Automation is a good thing.
    ui->SearchTerm->setFocus();
}

void MainWindow::on_SearchList_currentItemChanged(QTreeWidgetItem *current)
{
    if(has_search_results){
        QString id=current->text(3);

        GetEntry(id,true);
    }
}

void MainWindow::on_actionJournal_Selector_triggered()
{
    if((ui->DisconnectButton->isEnabled()) || (Buffer::On_Search)){
        //Disconnect first

        // Switch back to Entry mode before disconnecting so we don't mess up which buttons should be enabled
        if(Buffer::On_Search){
            ui->tabWidget->setCurrentIndex(0);
            Disconnect();
        }
        else{
            Disconnect();
        }
    }

    // launch JournalSelector
    JournalSelector j(this);
    j.exec();
}

// New for 0.5: Restore splitter to default position. --Will Kraft, 6/21/13
void MainWindow::on_actionRestore_Splitter_Position_triggered()
{
    QList<int> size;
    size.append(1);
    size.append(splittersize); // was originally 225 but fixed values dont work very well

    if (Buffer::showwarnings){
        QMessageBox m;

        int choice=m.question(this,"RoboJournal","Do you really want to move the splitter bar back to its default position?",
                              QMessageBox::No | QMessageBox::Yes, QMessageBox::No);

        switch(choice){
        case QMessageBox::Yes:
            ui->splitter->setSizes(size);
            Buffer::mw_splitter_size.clear();

            break;

        case QMessageBox::No:
            // do nothing
            break;
        }
    }
    else{
        ui->splitter->setSizes(size);
        Buffer::mw_splitter_size.clear();
    }
}

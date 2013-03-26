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


#include "ui/editor.h"
#include "ui_editor.h"
#include <iostream>
#include <core/buffer.h>
#include <stdlib.h>
#include <QDate>
#include "sql/mysqlcore.h"
#include <QDateEdit>
#include <QMessageBox>
#include <QMenu>
#include <QFont>
#include <QStatusBar>
#include <QRegExp>
#include <QVBoxLayout>
#include <QToolBar>
#include <QColor>
#include <QPalette>
#include <QDesktopWidget>
#include "ui/SpellTextEdit.h"
#include "ui/highlighter.h"
#include <QWidgetAction>

QString Editor::body;
QString Editor::title;
int Editor::day;
int Editor::month;
int Editor::year;


void Editor::reject(){
    ConfirmExit();
}

//#############################################################################################################
// Set up form (create toolbars and statusbar) and bind everything to a vbox layout. New for 0.4.
void Editor::PrimaryConfig(){
    using namespace std;

    if(Buffer::show_icon_labels){
        ui->PostEntry->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->Cancel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->CutButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->CopyButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->PasteButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->UndoButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->RedoButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->ShowErrors->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }

    QToolBar *bar=new QToolBar(this);
    bar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    bar->setLayoutDirection(Qt::LeftToRight);
    bar->setContextMenuPolicy(Qt::NoContextMenu);

    // Bind the Toolbar Buttons to QWidgetActions before adding them to the toolbar. This allows the toolbar overflow
    // menu to work where it would not if we had added the buttons to the toolbar directly (using QToolBar::addWidget).
    QWidgetAction* postAction = new QWidgetAction(this);
    postAction->setDefaultWidget(ui->PostEntry);

    QWidgetAction* cancelAction = new QWidgetAction(this);
    cancelAction->setDefaultWidget(ui->Cancel);

    QWidgetAction* cutAction = new QWidgetAction(this);
    cutAction->setDefaultWidget(ui->CutButton);

    QWidgetAction* copyAction = new QWidgetAction(this);
    copyAction->setDefaultWidget(ui->CopyButton);

    QWidgetAction* pasteAction = new QWidgetAction(this);
    pasteAction->setDefaultWidget(ui->PasteButton);

    QWidgetAction* undoAction = new QWidgetAction(this);
    undoAction->setDefaultWidget(ui->UndoButton);

    QWidgetAction* redoAction = new QWidgetAction(this);
    redoAction->setDefaultWidget(ui->RedoButton);

    QWidgetAction* spellAction = new QWidgetAction(this);
    spellAction->setDefaultWidget(ui->ShowErrors);


    // populate bar.
    bar->addAction(postAction);
    bar->addAction(cancelAction);

    bar->addSeparator();

    bar->addAction(cutAction);
    bar->addAction(copyAction);
    bar->addAction(pasteAction);

    bar->addSeparator();

    bar->addAction(undoAction);
    bar->addAction(redoAction);

    bar->addSeparator();

    bar->addAction(spellAction);



    // Set up spacers for the Title/Date bar (masterbar). These are used to keep the title/date fields from being squished together.

    QWidget* spacer1 = new QWidget();
    spacer1->setMinimumWidth(10);
    spacer1->setMinimumHeight(30); // add a bit of vertical padding to the upper toolbar.
    spacer1->setMaximumWidth(10);
    spacer1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    QWidget* spacer2 = new QWidget();
    spacer2->setMinimumWidth(15);
    spacer2->setMaximumWidth(15);
    spacer2->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    QWidget* spacer3 = new QWidget();
    spacer3->setMinimumWidth(10);
    spacer3->setMaximumWidth(10);
    spacer3->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    QWidget* spacer4 = new QWidget();
    spacer4->setMinimumWidth(4);
    spacer4->setMaximumWidth(4);
    spacer4->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    QWidget* spacer5 = new QWidget();
    spacer5->setMinimumWidth(4);
    spacer5->setMaximumWidth(4);
    spacer5->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);

    // Hide buttons on Date Indicator
    ui->EntryDate->setButtonSymbols(QAbstractSpinBox::NoButtons);

    ui->EntryDate->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    ui->EntryTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);


    QToolBar *masterbar= new QToolBar(this);
    masterbar->setLayoutDirection(Qt::LeftToRight);
    masterbar->setContextMenuPolicy(Qt::PreventContextMenu);


    masterbar->addWidget(spacer1);
    masterbar->addWidget(ui->label);
    masterbar->addWidget(spacer4);
    masterbar->addWidget(ui->EntryTitle);
    masterbar->addWidget(spacer2);
    masterbar->addWidget(ui->label_2);
    masterbar->addWidget(spacer5);
    masterbar->addWidget(ui->EntryDate);
    masterbar->addWidget(spacer3);

    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->setContentsMargins(2,6,2,0);
    layout->setSpacing(1);


    // create status bar and populate it
    QStatusBar *sbar=new QStatusBar(this);

    sbar->addWidget(ui->WordCount, 0);
    sbar->addWidget(ui->CharacterCount,0);
    sbar->addWidget(ui->RemainingCharacters,0);

    // empty slot to keep everything spaced properly
    QLabel *spacer=new QLabel(" ");
    sbar->addWidget(spacer,1);
    sbar->addWidget(ui->DocumentMode,0);
    sbar->setSizeGripEnabled(true);

    layout->addWidget(masterbar);
    layout->addWidget(ui->line);


#ifdef _WIN32
    // Delete the toolbar dividing line on Windows. On most Linux window managers it helps to have a divider, but
    // on Windows it just looks bad.
    delete ui->line;
#endif


    layout->addWidget(bar,1);


    // Decide which TextEdit to use depending on whether user enabled spellcheck in preferences:

    // Option 1: Use Jan Sundermeyer's spellcheck-enabled editor thingy.
    if(Buffer::use_spellcheck){

        spell=new SpellTextEdit(this,Buffer::current_dictionary);
        high=new Highlighter(spell->document(),Buffer::current_dictionary,true);

        spell->setDict(Buffer::current_dictionary);
        high->setDict(Buffer::current_dictionary);

        // allow the app to detect when the SpellTextEdit adds a word or has its text changed in any way.
        // The second function is essential for the word count feature to work.
        connect(spell,SIGNAL(addWord(QString)),high,SLOT(slot_addWord(QString)));
        connect(spell, SIGNAL(textChanged()), this, SLOT(on_spell_textChanged()));

        layout->addWidget(spell,true);

    }

    // Option 2: If we're not using spellcheck, just use a regular QTextEdit. This is unchanged from =< 0.3.
    else{
        layout->addWidget(ui->EntryPost,true);

    }


    layout->addWidget(sbar);
    this->setLayout(layout);


    // restore window size from previous session
    bool usetemp=Buffer::editor_temporarysize.isEmpty();
    //cout << " Temporary size is valid : " << usetemp << endl;


    if(usetemp && startup){
        this->resize(Buffer::editor_geometry);
    }
    else{
        this->resize(Buffer::editor_temporarysize);
    }

    // decide what to do with toolbar spellcheck button
    if(Buffer::use_spellcheck){
        ui->ShowErrors->setDisabled(false);

        if(Buffer::show_spell_errors_by_default){
            ui->ShowErrors->setChecked(true);
        }
        else{
            ui->ShowErrors->setChecked(false);
            high->enableSpellChecking(false);
        }

    }
    else{
        ui->ShowErrors->setDisabled(true);
    }

    if(Buffer::editmode){
        ui->Cancel->setText("Discard Changes");
        ui->Cancel->setToolTip("Discard Changes");
    }
    else{
        ui->Cancel->setText("Discard Entry");
        ui->Cancel->setToolTip("Discard Entry");
    }

    // clear startup mode.
    startup=false;
}

//#############################################################################################################
// This function applies Post-processing features to the body text; new for 0.4.
QString Editor::Do_Post_Processing(QString rawtext, int wordcount){
    using namespace std;

    body=rawtext;


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

            if((next.contains("st")) || (next.contains("ST"))){
                fixed=next.section("",0,next.length()-2);
                fixed=fixed + "<sup>st</sup>";
            }

            if((next.contains("nd")) || (next.contains("ND"))){
                fixed=next.section("",0,next.length()-2);
                fixed=fixed + "<sup>nd</sup>";
            }

            if((next.contains("rd")) || (next.contains("RD"))){
                fixed=next.section("",0,next.length()-2);
                fixed=fixed + "<sup>rd</sup>";
            }

            if((next.contains("th")) || (next.contains("TH"))){
                fixed=next.section("",0,next.length()-2);
                fixed=fixed + "<sup>th</sup>";
            }

            body=body.replace(next,fixed);
        }
    }

    return body;
}

//#############################################################################################################
// Set fonts in editor
void Editor::Set_Editor_Fonts(){
    using namespace std;

    QFont font;

    if(Buffer::use_custom_theme_editor){
        font.setFamily(Buffer::font_face);

        int size=Buffer::font_size.toInt();
        font.setFixedPitch(true);
        font.setPointSize(size);

        /* Bugfix: Prevent segfault by separating the spell and ui.entryPost objects with a conditional.
         * If spellcheck is disabled, trying to apply font settings to the spell object causes the whole app to crash.
         * Spell should not get called at all if Buffer::use_spellcheck==false.
         */

        if(Buffer::use_spellcheck){
            spell->setFont(font);
        }
        else{
            ui->EntryPost->setFont(font);
        }



    }
    else{

        font.setFamily("Verdana");
        font.setFixedPitch(true);
        font.setPointSize(14);

        /* Bugfix: Prevent segfault by separating the spell and ui.entryPost objects with a conditional.
         * If spellcheck is disabled, trying to apply font settings to the spell object causes the whole app to crash.
         * Spell should not get called at all if Buffer::use_spellcheck==false.
         */

        if(Buffer::use_spellcheck){
            spell->setFont(font);
        }
        else{
            ui->EntryPost->setFont(font);
        }
    }
}

//#############################################################################################################
// Ask the user to confirm the desire to exit the editor window. Triggered if ESC is pressed, Abort is clicked,
// or window is closed.
void Editor::ConfirmExit(){

    if(Buffer::showwarnings){
        QMessageBox m(this);
        m.setWindowTitle("RoboJournal");
        m.setIcon(QMessageBox::Question);

        if(Buffer::editmode){
            m.setText("Any changes you made to this entry have not been saved yet. Are you sure you want to close the editor now?");
        }
        else{
            m.setText("This entry has not been saved. Are you sure you want to close the editor now?");
        }

        m.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        m.setDefaultButton(QMessageBox::No);
        int choice=m.exec();

        switch(choice){
        case QMessageBox::Yes:
            this->done(0);
            break;

        case QMessageBox::No:
            // do nothing
            break;
        }
    }
    else{
        this->done(0);
    }

}

//#############################################################################################################
Editor::Editor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editor)
{
    ui->setupUi(this);

    startup=true;

    // Set up form
    PrimaryConfig();


#ifdef unix
    QIcon unixicon(":/icons/robojournal-icon-big.png");
    this->setWindowIcon(unixicon);

#endif

    // hide question mark button in title bar when running on Windows
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    if(Buffer::editmode){
        InitiateEditMode();
        LoadEntry(Buffer::editentry);
    }
    else{
        // set todays date if we're writing a new entry
        SetDate();
    }



    // Only zero everything out if we are creating a new entry. Otherwise, DocumentStats()
    // gets called when the editor starts up in edit mode and the proper values are pre-loaded.

    if(!Buffer::editmode){
        ui->WordCount->setText(" Word count: 0 ");
        ui->CharacterCount->setText(" Character count: 0 ");
        ui->RemainingCharacters->setText(" Remaining characters: 65535 ");
        ui->DocumentMode->setText(" Write Mode ");
    }

    Set_Editor_Fonts();


    //Set cursor focus by default.
    if(Buffer::use_spellcheck){
        spell->setFocus();
    }
    else{
        ui->EntryPost->setFocus();
    }
}


//#############################################################################################################
// Setup the form in EditMode
void Editor::InitiateEditMode(){

    ui->PostEntry->setText("Save Changes");

    ui->DocumentMode->setText(" Edit Mode ");
    ui->DocumentMode->setAlignment(Qt::AlignRight);

    ui->PostEntry->setToolTip("Save Changes");

}

//#############################################################################################################
// Load an entry and populate the form with it if EditMode is true
void Editor::LoadEntry(QString entry){

    if(Buffer::backend=="MySQL"){

        MySQLCore m;
        QList<QString> list=m.RetrieveEntryFull(entry);

        QString body=list.at(5);

        // Replace post-processing features with plain text to make the entry easy to edit. This will be a temporary
        // work-around until Rich Text is fully supported. It would not do to have people see &mdash; and wonder what it is.
        // The special characters will be restored anyway once the user saves the entry.
        body=body.replace("&ldquo;","\"");
        body=body.replace("&rdquo;","\"");
        body=body.replace("&mdash;","--");
        body=body.replace("&ndash;","-");
        body=body.replace("&hellip;","...");
        body=body.replace("&rsquo;","\'");
        body=body.replace(QRegExp("</?sup>"),"");

        ui->EntryTitle->setText(list.at(0));

        if(Buffer::use_spellcheck){
            spell->setPlainText(body);

        }
        else{

            ui->EntryPost->setPlainText(body);
        }


        QDate postdate;
        postdate.setDate(list.at(3).toInt(),list.at(1).toInt(),list.at(2).toInt());

        ui->EntryDate->setDate(postdate);


        // Do document statistics
        DocumentStats();
    }

}

//#############################################################################################################
//Update entry in the database
bool Editor::UpdateEntry(){

    using namespace std;

    //Bugfix for 0.2
    // clean commas out of the title so it doesn't mess up the array in MainWindow::CreateTree()
    // Update 0.4: reimplement this as a Regex to make it a bit more robust.
    QString title=ui->EntryTitle->text();

    QRegExp badchars("[,]+|[\"]+|[\']+");
    title=title.remove(badchars);

    QString body;

    //get word count value.
    QString count=ui->WordCount->text().section(":",1,-1);
    //cout << "word count: " + count.toStdString() << endl;
    int wordcount=count.toInt();

    if(Buffer::use_spellcheck){
        body=Do_Post_Processing(spell->toPlainText(), wordcount);
    }
    else{
        body=Do_Post_Processing(ui->EntryPost->toPlainText(), wordcount);
    }

    QDate post_date=ui->EntryDate->date();

    int day=post_date.day();
    int month=post_date.month();
    int year=post_date.year();

    MySQLCore m;
    bool success=m.Update(title,month,day,year,body,Buffer::editentry);
    return success;

}

//#############################################################################################################
void Editor::resizeEvent(QResizeEvent *){

    if(!startup){
        Buffer::editor_temporarysize=this->size();
        Buffer::editor_sizechanged=true;
    }
}

//#############################################################################################################
// Prepare a new Entry and post it to database
bool Editor::NewEntry(){
    using namespace std;

    //get word count value
    QString count=ui->WordCount->text().section(":",1,-1);
    //cout << "word count: " + count.toStdString() << endl;
    int wordcount=count.toInt();

    if(Buffer::use_spellcheck){
        Editor::body=Do_Post_Processing(spell->toPlainText(),wordcount);
    }
    else{
        Editor::body=Do_Post_Processing(ui->EntryPost->toPlainText(),wordcount);
    }


    //Bugfix for 0.2
    // clean commas out of the title so it doesn't mess up the array in MainWindow::CreateTree()
    // Update 0.4: reimplement this as a Regex to make it a bit more robust.
    QString title=ui->EntryTitle->text();

    QRegExp badchars("[,]+|[\"]+|[\']+");
    title=title.remove(badchars);


    Editor::title=title;
    QDate post_date=ui->EntryDate->date();


    Editor::day=post_date.day();
    Editor::month=post_date.month();
    Editor::year=post_date.year();


    bool success=false;
    cout << "OUTPUT: Adding new entry \"" + Editor::title.toStdString() + "\" to database " << Buffer::database_name.toStdString() << "...";

    if(Buffer::backend=="MySQL"){
        MySQLCore my;
        success=my.AddEntry();
    }

    if(success){
        cout << "SUCCESS!" << endl;


    }
    else{
        cout << "FAILED" << endl;
    }

    return success;
}

//#############################################################################################################
Editor::~Editor()
{


    delete ui;
}

//#############################################################################################################
//get current date
void Editor::SetDate(){
    ui->EntryDate->setDate(QDate::currentDate());

}

// Automatically update window title with post title
void Editor::setTitle(QString title){
    this->setWindowTitle(title + " -- RoboJournal");

    if(title==NULL){
        this->setWindowTitle("Untitled Entry -- RoboJournal");
    }
}




//#############################################################################################################
void Editor::on_EntryTitle_textChanged(const QString &title)
{
    // pass title string to Editor::SetTitle() function... don't confuse this with QDialog::SetWindowTitle!!!
    setTitle(title);
}

//#############################################################################################################
// keep track of document statistics
void Editor::DocumentStats(){
    using namespace std;

    QString data;

    if(Buffer::use_spellcheck){
        data=spell->toPlainText();
    }
    else{
        data=ui->EntryPost->toPlainText();
    }


    int maxlength=65535;
    int num=data.size();
    int remaining=maxlength-num;

    QString str_num;
    QString str_rem;

    str_num.append(QString("%1").arg(num));
    str_rem.append(QString("%1").arg(remaining));

    QString wordcount=WordCount(data);


    ui->WordCount->setText(" Word count: " + wordcount + " ");
    ui->CharacterCount->setText(" Character count: " + str_num + " ");
    ui->RemainingCharacters->setText(" Remaining characters: " + str_rem + " ");

    // prevent mysql data overflows, though it must have been one hell of a day if you can't say
    // what's on your mind in less than 65k chars

    if(remaining<0){

        ui->RemainingCharacters->setText("<font color=\"red\"> Remaining characters: " +
                                         str_rem +"</font> ");


        ui->PostEntry->setDisabled(true);
    }
    else{
        ui->PostEntry->setEnabled(true);
    }

}

//#############################################################################################################
// Get word count from body. This needs to return a QString instead
// of Int because it needs to be displayed in a QLabel. Sure, we could
// return an int but that would require an extra conversion step.

QString Editor::WordCount(QString data){

    // get rid of extra whitespace that would throw off the correct count
    data=data.trimmed();
    data=data.simplified();

    // split the string into an array of words and count the size of array. That's our word count.
    QStringList words=data.split(" ");
    int count=words.count();

    QString wordcount="0";

    // solve a bug that always returns wordcount as 1 even if the body is blank
    if(data.size()==0){
        wordcount="0";
    }
    else{
        wordcount=QString::number(count);
    }

    return wordcount;
}


//#############################################################################################################
void Editor::on_EntryPost_textChanged()
{
    DocumentStats();
}

//#############################################################################################################

void Editor::on_spell_textChanged(){
    DocumentStats();
}

//#############################################################################################################
void Editor::on_UndoButton_clicked()
{

    if(Buffer::use_spellcheck){
        spell->undo();
    }
    else{
        ui->EntryPost->undo();
    }

}

//#############################################################################################################
void Editor::on_RedoButton_clicked()
{
    if(Buffer::use_spellcheck){
        spell->redo();
    }
    else{
        ui->EntryPost->redo();
    }


}


//#############################################################################################################
void Editor::on_Cancel_clicked()
{
    if(Buffer::showwarnings){
        ConfirmExit();
    }
    else{
        this->done(0);
    }


}

//#############################################################################################################
void Editor::on_CutButton_clicked()
{
    if(Buffer::use_spellcheck){
        spell->cut();
    }
    else{
        ui->EntryPost->cut();
    }

}

//#############################################################################################################
void Editor::on_CopyButton_clicked()
{
    if(Buffer::use_spellcheck){
        spell->copy();
    }
    else{
        ui->EntryPost->copy();
    }

}

//#############################################################################################################
void Editor::on_PasteButton_clicked()
{
    if(Buffer::use_spellcheck){
        spell->paste();
    }
    else{
        ui->EntryPost->paste();
    }

}

//#############################################################################################################
void Editor::on_PostEntry_clicked()
{
    QMessageBox m;

    if(Buffer::use_spellcheck){

        if(Buffer::editmode){ // edit mode

            if((spell->toPlainText().isEmpty()) || (ui->EntryTitle->text().isEmpty())){
                m.critical(this,"RoboJournal","You must fill in the title and body of this entry before "
                           "RoboJournal can save it.");
            }
            else{
                bool success=UpdateEntry();

                if(success){
                    this->hide();
                    m.information(this,"RoboJoural", "\"" + ui->EntryTitle->text() +
                                  "\" was successfully updated.");
                    close();
                }
            }
        }
        else{ // new entry mode

            if((spell->toPlainText().isEmpty()) || (ui->EntryTitle->text().isEmpty())){
                m.critical(this,"RoboJournal","You must fill in the title and body of this entry before "
                           "RoboJournal can save it.");
            }
            else{
                bool success=NewEntry();

                if(success){
                    this->hide();
                    m.information(this,"RoboJoural", "\"" + ui->EntryTitle->text() +
                                  "\" was successfully added to your journal.");
                    close();
                }
            }
        }

    }

    // Non-Spellcheck Mode
    else{

        if(Buffer::editmode){ // edit mode

            if((ui->EntryPost->toPlainText().isEmpty()) || (ui->EntryTitle->text().isEmpty())){
                m.critical(this,"RoboJournal","You must fill in the title and body of this entry before "
                           "RoboJournal can save it.");
            }
            else{
                bool success=UpdateEntry();

                if(success){
                    this->hide();
                    m.information(this,"RoboJoural", "\"" + ui->EntryTitle->text() +
                                  "\" was successfully updated.");

                    close();
                }
            }
        }
        else{ // new entry mode

            if((ui->EntryPost->toPlainText().isEmpty()) || (ui->EntryTitle->text().isEmpty())){
                m.critical(this,"RoboJournal","You must fill in the title and body of this entry before "
                           "RoboJournal can save it.");
            }
            else{
                bool success=NewEntry();

                if(success){
                    this->hide();
                    m.information(this,"RoboJoural", "\"" + ui->EntryTitle->text() +
                                  "\" was successfully added to your journal.");

                    close();
                }
            }
        }
    }
}

void Editor::on_ShowErrors_toggled(bool checked)
{
    if(checked){
        high->enableSpellChecking(true);

    }
    else{
        high->enableSpellChecking(false);

    }
}

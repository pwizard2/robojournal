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
#include <QSplitter>
#include "ui/editortagmanager.h"
#include "ui_editortagmanager.h"
#include "core/htmlcore.h"
#include <QSplitterHandle>

QString Editor::body;
QString Editor::title;
int Editor::day;
int Editor::month;
int Editor::year;
QString Editor::tags;

void Editor::reject(){
    ConfirmExit();
}

// This function shows/hides the HTML code in the editor TextEdit. New for 0.5. 7/3/13.
//############################################################################################################
void Editor::ToggleHTML(bool checked){

    QString html=ui->EntryPost->toHtml();
    ui->EntryPost->clear();

    if(checked){


    ui->EntryPost->setPlainText(html);
    }
    else{

    ui->EntryPost->setHtml(doc->toPlainText());
    }
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
    bar->setContextMenuPolicy(Qt::DefaultContextMenu);


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

    // New actions for 0.5 (3/9/13)
    QWidgetAction* boldAction = new QWidgetAction(this);
    boldAction->setDefaultWidget(ui->bold);

    QWidgetAction* emAction = new QWidgetAction(this);
    emAction->setDefaultWidget(ui->Italic);

    QWidgetAction* uAction = new QWidgetAction(this);
    uAction->setDefaultWidget(ui->Underline);

    QWidgetAction* lpAction = new QWidgetAction(this);
    lpAction->setDefaultWidget(ui->ParaLeft);

    QWidgetAction* cpAction = new QWidgetAction(this);
    cpAction->setDefaultWidget(ui->ParaCenter);

    QWidgetAction* rpAction = new QWidgetAction(this);
    rpAction->setDefaultWidget(ui->ParaRight);

    QWidgetAction* tagAction = new QWidgetAction(this);
    tagAction->setDefaultWidget(ui->TagButton);

    QWidgetAction* bqAction = new QWidgetAction(this);
    bqAction->setDefaultWidget(ui->Blockquote);

    QWidgetAction* tcAction = new QWidgetAction(this);
    tcAction->setDefaultWidget(ui->ShowCode);


    /* ###############################################################################
    Update 7/4/13: Postpone these rich text formatting actions to 0.6 because the rich text
    display/editing system has been delayed. Hide these actions for now.

    bar->addAction(boldAction);
    bar->addAction(emAction);
    bar->addAction(uAction);

    bar->addSeparator();

    bar->addAction(lpAction);
    bar->addAction(cpAction);
    bar->addAction(rpAction);
    bar->addAction(bqAction);

    bar->addSeparator();
    */

    ui->bold->setVisible(false);
    ui->Italic->setVisible(false);
    ui->Underline->setVisible(false);
    ui->ParaLeft->setVisible(false);
    ui->ParaCenter->setVisible(false);
    ui->ParaRight->setVisible(false);
    ui->ShowCode->setVisible(false);
    //###############################################################################

    // populate primary toolbar.
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

    //bar->addAction(tcAction);
    bar->addAction(spellAction);
    bar->addAction(tagAction);


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
    layout->setContentsMargins(0,0,0,0);
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

    // (added for 0.5 -- 6/10/13) Set up vertical splitter for text area and tag area.
    divide=new QSplitter(this);
    divide->setOrientation(Qt::Vertical);

    // Add tagging interface as QWidget Object (6/10/13)
    et=new EditorTagManager(this);
    et->setMinimumWidth(this->width());
    et->setMaximumHeight(125);
    EditorTagManager::standalone_tagger=false; // false b/c this EditorTagManager is not contained in the standalone Tagger interface

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
        connect(spell,SIGNAL(addWord(QString)), high,SLOT(slot_addWord(QString)));
        connect(spell, SIGNAL(textChanged()), this, SLOT(on_spell_textChanged()));



        layout->addWidget(spell,true);

    }

    // Option 2: If we're not using spellcheck, just use a regular QTextEdit. This is unchanged from =< 0.3.
    else{

        divide->insertWidget(0,ui->EntryPost);
        divide->insertWidget(1,et);
        //layout->addWidget(ui->EntryPost,true);
    }

    layout->addWidget(divide,1);

    layout->addWidget(sbar);
    this->setLayout(layout);

    //Set up splitter sizes
    toggle_off.append(100);
    toggle_off.append(0);

    toggle_on.append(100);
    toggle_on.append(this->height()/4);

    divide->setSizes(toggle_off);


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

        // 7/12/13: Get current list of tags
        et->LoadTags(Buffer::editentry);
    }
    else{
        ui->Cancel->setText("Discard Entry");
        ui->Cancel->setToolTip("Discard Entry");
    }

    // clear startup mode.
    startup=false;

    // -- Will Kraft, 6/12/13: New for version 0.5.
    // This signal/slot pair connects the divider to the Manage Tags toolbar button. If the user drags the
    // slider up or down, emit a signal to toggle the toolbar button on or off.
    connect(divide, SIGNAL(splitterMoved(int,int)), this, SLOT(splitterMoved()));

    doc=new QTextDocument();


#ifdef _WIN32
    // 6/16/13: Make the splitter easier to see. Windows 7 renders these things as flat by default so the idea
    // is to emulate the old-school raised splitter bar appearance so the user will know something is there.
    QSplitterHandle *handle=divide->handle(1);
    QVBoxLayout *h_layout = new QVBoxLayout(handle);
    h_layout->setSpacing(0);
    h_layout->setMargin(0);
    h_layout->setContentsMargins(0,0,0,0); // make sure the splitter goes all the way to the edge of the frame

    QFrame *line = new QFrame(handle);
    line->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
    line->setLineWidth(3);
    h_layout->addWidget(line);
#endif

    //tighten up toolbar spacing for 0.5 (7/15/13)
    QSize barSize(16,16);
    masterbar->setIconSize(barSize);
    bar->setIconSize(barSize);
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

    HTMLCore h;

    if(Buffer::use_spellcheck){
        body=h.Do_Post_Processing(spell->toPlainText(), wordcount);
    }
    else{
        body=h.Do_Post_Processing(ui->EntryPost->toPlainText(), wordcount);
    }

    QDate post_date=ui->EntryDate->date();

    int day=post_date.day();
    int month=post_date.month();
    int year=post_date.year();

    // Get the tags from the EditorTagManager. New for 0.5, 6/29/13.
    QString taglist=et->HarvestTags();

    MySQLCore m;
    bool success=m.Update(title,month,day,year,body,Buffer::editentry,taglist);
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

    HTMLCore h;

    if(Buffer::use_spellcheck){
        Editor::body=h.Do_Post_Processing(spell->toPlainText(),wordcount);
    }
    else{

        QString body_text=ui->EntryPost->toPlainText();

        //body_text=h.ProcessEntryFromEditor(body_text);

        Editor::body=h.Do_Post_Processing(body_text,wordcount);

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

    // Get the tags from the EditorTagManager. New for 0.5, 6/29/13.

    QString tags=et->HarvestTags();

    if(!tags.isEmpty()){
        Editor::tags=tags;
    }
    else{
        Editor::tags="Null";
    }


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
    this->setWindowTitle(title + " - RoboJournal");

    if(title==NULL){
        this->setWindowTitle("Untitled Entry - RoboJournal");
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

    doc->setHtml(ui->EntryPost->toHtml());
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

void Editor::on_TagButton_toggled(bool checked)
{
    if(checked){
        divide->setSizes(toggle_on);
    }
    else{
        divide->setSizes(toggle_off);
    }
}

void Editor::splitterMoved(){
    if(divide->sizes().at(1)==0) {
        ui->TagButton->setChecked(false);
    }
    else{
        ui->TagButton->setChecked(true);
    }
}

void Editor::on_ShowCode_toggled(bool checked)
{
    ToggleHTML(checked);

}

void Editor::on_Italic_toggled(bool checked)
{
    if(Buffer::use_spellcheck){

    }
    else{
        ui->EntryPost->setFontItalic(checked);
    }
}

void Editor::on_Underline_toggled(bool checked)
{
    if(Buffer::use_spellcheck){

    }
    else{
        ui->EntryPost->setFontUnderline(checked);
    }
}

void Editor::on_bold_toggled(bool checked)
{
    if(Buffer::use_spellcheck){

    }
    else{
        if(checked){
            ui->EntryPost->setFontWeight(87); // bold text
        }
        else{
            ui->EntryPost->setFontWeight(50); // normal text
        }
    }
}

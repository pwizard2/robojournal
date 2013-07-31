#include "exportsingle.h"
#include "ui_exportsingle.h"
#include <QFileDialog>
#include <QDir>
#include <QPushButton>
#include <QIcon>
#include "core/buffer.h"
#include "ui/entryexporter.h"
#include <QMessageBox>


#ifdef _WIN32
#include <windows.h>
#endif

QString ExportSingle::path;
QString ExportSingle::filename;
bool ExportSingle::use_html;

//################################################################################################

ExportSingle::ExportSingle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExportSingle)
{
    ui->setupUi(this);
    PrimaryConfig();
}

//################################################################################################

ExportSingle::~ExportSingle()
{
    delete ui;
}

//################################################################################################
// Get values from formdata and buffer them.
void ExportSingle::HarvestValues(){

    ExportSingle::filename=ui->FileName->text();
    ExportSingle::path=ui->ExportLocation->text();
    ExportSingle::use_html=ui->HTML->isChecked();
}

//################################################################################################
// setup form when PrimaryConfig::exec() is called
void ExportSingle::PrimaryConfig(){
    using namespace std;

    // set defaults
    ui->HTML->click();
    ui->IncludeDate->click();
    ui->IncludeJournalName->click();

    // mass export tab
    ui->HTML->click();



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

//################################################################################################
// Show folder selection dialog when user chooses output directory
void ExportSingle::Browse(){
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
// process filename. This function gets called on load or whenever the filename flags get changed.
void ExportSingle::SetName(){

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
void ExportSingle::on_BrowseButton_clicked()
{
    Browse();
}

//################################################################################################
void ExportSingle::on_HTML_clicked()
{
    SetName();
}

//################################################################################################
void ExportSingle::on_PlainText_clicked()
{
    SetName();
}

//################################################################################################
void ExportSingle::on_IncludeDate_clicked()
{
    SetName();
}

void ExportSingle::on_IncludeJournalName_clicked()
{
    SetName();
}

//################################################################################################
void ExportSingle::on_AllowFreeEdits_toggled(bool checked)
{
    if(checked){
        ui->FileName->setReadOnly(false);
        ui->FileName->setFocus();
    }
    else{
        ui->FileName->setReadOnly(true);
        ui->FileName->clearFocus();
        SetName();
    }
}

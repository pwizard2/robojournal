#include "exportmulti.h"
#include "ui_exportmulti.h"
#include <QDir>
#include <QFileDialog>
#include <QPushButton>
#include <QIcon>
#include <QMessageBox>
#include <QDateTime>
#include "core/buffer.h"


#ifdef _WIN32
#include <windows.h>
#endif


ExportMulti::ExportMulti(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExportMulti)
{
    ui->setupUi(this);
    PrimaryConfig();
}

ExportMulti::~ExportMulti()
{
    delete ui;
}

//################################################################################################
// setup form when PrimaryConfigis called
void ExportMulti::PrimaryConfig(){
    using namespace std;

    // mass export tab
    ui->HTML->click();
    ui->SortDescending->click();
    ui->IncludeExportDate->click();


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
    SetMassName();
}

//################################################################################################
// Show folder selection dialog when user chooses output directory
void ExportMulti::Browse(){
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
// Set filename for Mass export
void ExportMulti::SetMassName(){

    QString extension;

    if(ui->HTML->isChecked()){
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

    ui->FileName->setText(filename);

}

void ExportMulti::on_BrowseButton_clicked()
{
    Browse();
}

void ExportMulti::on_checkBox_toggled(bool checked)
{
    if(checked){
        ui->FileName->setReadOnly(false);
        ui->FileName->setFocus();
    }
    else{
        ui->FileName->setReadOnly(true);
        ui->FileName->clearFocus();
        SetMassName();
    }
}

void ExportMulti::on_HTML_clicked()
{
    SetMassName();
}

void ExportMulti::on_PlainText_clicked()
{
      SetMassName();
}

void ExportMulti::on_IncludeExportDate_clicked()
{
      SetMassName();
}

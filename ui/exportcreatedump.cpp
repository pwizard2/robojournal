#include "exportcreatedump.h"
#include "ui_exportcreatedump.h"

ExportCreateDump::ExportCreateDump(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExportCreateDump)
{
    ui->setupUi(this);
}

ExportCreateDump::~ExportCreateDump()
{
    delete ui;
}

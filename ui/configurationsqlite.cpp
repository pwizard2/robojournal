#include "configurationsqlite.h"
#include "ui_configurationsqlite.h"

ConfigurationSQLite::ConfigurationSQLite(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationSQLite)
{
    ui->setupUi(this);
}

ConfigurationSQLite::~ConfigurationSQLite()
{
    delete ui;
}

#ifndef CONFIGURATIONSQLITE_H
#define CONFIGURATIONSQLITE_H

#include <QWidget>

namespace Ui {
    class ConfigurationSQLite;
}

class ConfigurationSQLite : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigurationSQLite(QWidget *parent = 0);
    ~ConfigurationSQLite();
    void GetChanges();

private:
    Ui::ConfigurationSQLite *ui;
    void PopulateForm();
};

#endif // CONFIGURATIONSQLITE_H

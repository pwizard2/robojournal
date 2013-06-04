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

private:
    Ui::ConfigurationSQLite *ui;
};

#endif // CONFIGURATIONSQLITE_H

#ifndef CONFIGURATIONSQLITE_H
#define CONFIGURATIONSQLITE_H

#include <QWidget>
#include <QListWidgetItem>

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
    void Show_Known_Journals();
    void ApplyDefaultProperties(QListWidgetItem *item);
    void demoteDatabase(QListWidgetItem *item);
    void Harvest_Favorite_Databases();

    QListWidgetItem *default_db;

private slots:
    void on_Favorites_itemDoubleClicked(QListWidgetItem *item);

};

#endif // CONFIGURATIONSQLITE_H

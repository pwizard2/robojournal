#ifndef NEWJOURNALCREATOR_H
#define NEWJOURNALCREATOR_H

#include <QDialog>
#include <QStackedWidget>
#include "ui/journalcreatorcoverpage.h"
#include "ui_journalcreatorcoverpage.h"
#include "ui/mysqljournalpage.h"
#include "ui_mysqljournalpage.h"
#include "ui/sqlitejournalpage.h"
#include "ui_sqlitejournalpage.h"


namespace Ui {
class NewJournalCreator;
}

class NewJournalCreator : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewJournalCreator(QWidget *parent = 0);
    ~NewJournalCreator();

    void ManageOKButton(bool unlock);
    
private slots:
    void on_DatabaseType_currentRowChanged(int currentRow);

private:
    Ui::NewJournalCreator *ui;

    void PrimaryConfig();

    QStackedWidget *stack;
    MySQLJournalPage *m;
    SQLiteJournalPage *s;
    JournalCreatorCoverPage *c;



};

#endif // NEWJOURNALCREATOR_H

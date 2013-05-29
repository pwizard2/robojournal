#ifndef MYSQLJOURNALPAGE_H
#define MYSQLJOURNALPAGE_H

#include <QWidget>


namespace Ui {
class MySQLJournalPage;
}

class MySQLJournalPage : public QWidget
{
    Q_OBJECT
    
public:
    explicit MySQLJournalPage(QWidget *parent = 0);
    ~MySQLJournalPage();

signals:
    void unlockOK();
    void unlockNotOK();

public slots:
    void ClearForm();

    
private slots:
    void on_Password1_textChanged();

    void on_Password2_textEdited();

    void on_JournalName_editingFinished();

    void on_Username_editingFinished();

private:
    Ui::MySQLJournalPage *ui;
    void PrimaryConfig();
    void PasswordsMatch();
    int PasswordStrength(QString passwd);

};

#endif // MYSQLJOURNALPAGE_H

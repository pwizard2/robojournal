#ifndef EXPORTSINGLE_H
#define EXPORTSINGLE_H

#include <QWidget>

namespace Ui {
class ExportSingle;
}

class ExportSingle : public QWidget
{
    Q_OBJECT
    
public:
    explicit ExportSingle(QWidget *parent = 0);
    ~ExportSingle();
    void HarvestValues();

    static QString filename;
    static QString path;
    static bool use_html;


    
private slots:
    void on_BrowseButton_clicked();

    void on_HTML_clicked();

    void on_PlainText_clicked();

    void on_IncludeDate_clicked();

    void on_IncludeJournalName_clicked();

    void on_AllowFreeEdits_toggled(bool checked);

private:
    Ui::ExportSingle *ui;
    void PrimaryConfig();
    void Browse();
    void SetName();
};

#endif // EXPORTSINGLE_H

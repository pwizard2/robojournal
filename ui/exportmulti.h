#ifndef EXPORTMULTI_H
#define EXPORTMULTI_H

#include <QWidget>

namespace Ui {
class ExportMulti;
}

class ExportMulti : public QWidget
{
    Q_OBJECT
    
public:
    explicit ExportMulti(QWidget *parent = 0);
    ~ExportMulti();

private slots:
    void on_BrowseButton_clicked();

    void on_checkBox_toggled(bool checked);

    void on_HTML_clicked();

    void on_PlainText_clicked();

    void on_IncludeExportDate_clicked();

private:
    Ui::ExportMulti *ui;
    void PrimaryConfig();
    void Browse();
    void SetMassName();
};

#endif // EXPORTMULTI_H

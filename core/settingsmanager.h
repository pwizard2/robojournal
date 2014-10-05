#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>
#include <QByteArray>

class SettingsManager
{

public:
    SettingsManager();

    void LoadConfig(bool startup);
    void UpdateConfig();
    void NewConfig(QString host,QString db_name, QString port, QString newuser);
    QString FullName();
    bool GenderCheck();
    void InstallDictionaries();
    void SaveEditorSize(QSize geo);
    void SaveConfigSize(QSize geo);
    void SavePreviewSize(QSize geo);
    void SaveMainWindowSize(QByteArray geo);
    void SaveNagPreferences();
    void SaveSplitterPos(QByteArray value);
    void Save_Mysqldump_Path(QString path);
    void Save_HelpDoc(QString path);


private:
    //extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
    std::string ok_param;
    std::string fail_param;
};

#endif // SETTINGSMANAGER_H

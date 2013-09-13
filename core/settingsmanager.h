#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>


class SettingsManager
{


public:
    SettingsManager();

    void LoadConfig();
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

private:
    //extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
};

#endif // SETTINGSMANAGER_H

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "ui_SessionConfig.h"
#include <QDialog>
#include <QSettings>

//////////////////////////////////////////////////////////////////////////
class CConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CConfigDialog(QWidget* parent = Q_NULLPTR);
    ~CConfigDialog();

    void SetAppName( const std::string& name );
    void SetRunBeforeName( const std::string& name );

    std::string GetAppName() const;
    std::string GetRunBeforeName() const;
    QList<std::pair<QString, QString>> GetEnvironmentValues() const;
    QTableWidget* GetEnvTableWidget();

private Q_SLOTS:
    void addEnironVariable();
    void delEnironVariable();
    void editEnironVariable();
    void addTestAppPath();
    void addRunBeforePath();
    void addNewParam();
    void delParam();

private:
    Ui::Dialog uiConf;
};

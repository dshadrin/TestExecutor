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
    void SetProxyName( const std::string& name );

    std::string GetAppName() const;
    std::string GetProxyName() const;

private Q_SLOTS:
    void addEnironVariable();
    void delEnironVariable();
    void editEnironVariable();
    void addTestAppPath();
    void addProxyPath();

private:
    Ui::Dialog uiConf;
};

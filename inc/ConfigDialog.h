#pragma once

//////////////////////////////////////////////////////////////////////////

#include "ui_SessionConfig.h"
#include <QDialog>
#include <QSettings>

//////////////////////////////////////////////////////////////////////////
class CConfig;

//////////////////////////////////////////////////////////////////////////
class CConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CConfigDialog(CConfig* config, QWidget* parent = Q_NULLPTR);
    ~CConfigDialog();

private Q_SLOTS:
    void addEnironVariable();
    void addTestAppPath();
    void addProxyPath();

private:
    Ui::Dialog uiConf;
    CConfig* m_config;
};

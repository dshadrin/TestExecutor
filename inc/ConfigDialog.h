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
    CConfigDialog(QWidget* parent = Q_NULLPTR);
    ~CConfigDialog();

private Q_SLOTS:
    void addEnironVariable();

private:
    Ui::Dialog uiConf;
};

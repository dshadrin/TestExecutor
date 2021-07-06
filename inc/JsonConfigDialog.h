#pragma once

//////////////////////////////////////////////////////////////////////////

#include "ui_JsonConfig.h"
#include "JsonConfig.h"
#include <QDialog>
#include <QSettings>

//////////////////////////////////////////////////////////////////////////
class CJsonConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CJsonConfigDialog(QWidget* parent = Q_NULLPTR);
    ~CJsonConfigDialog();

    void InitDialog(CJsonConfig* pConfig);

private Q_SLOTS:
    void addNewProperty();
    void delProperty();
    void editProperty();

private:
    Ui::JsonConfigDialog uiConf;
    CJsonConfig* m_pConfig;
};

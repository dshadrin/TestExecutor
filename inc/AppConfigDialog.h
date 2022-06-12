#pragma once

//////////////////////////////////////////////////////////////////////////

#include "ui_AppConfig.h"
#include "AppConfig.h"
#include <QDialog>
#include <QStringList>
#include <QMenu>

const int TREE_VALUE_COLUMN = 0;
const int TREE_FLAGS_COLUMN = 1;
const int TABLE_NAME_COLUMN = 0;
const int TABLE_VALUE_COLUMN = 1;
const int TABLE_TYPE_COLUMN = 2;
//////////////////////////////////////////////////////////////////////////
class CAppConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CAppConfigDialog(CAppConfig* pConfig, QWidget* parent = Q_NULLPTR);
    ~CAppConfigDialog();

private:
    Ui::AppConfigDialog uiConf;
    CAppConfig* m_pConfig;
    QMenu* m_treeContextMnu;
    bool m_bDirty;
};

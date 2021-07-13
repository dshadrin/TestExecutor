#pragma once

//////////////////////////////////////////////////////////////////////////

#include "ui_JsonConfig.h"
#include "JsonConfig.h"
#include <QDialog>
#include <QStringList>
#include <QMenu>

const int TREE_VALUE_COLUMN = 0;
const int TREE_FLAGS_COLUMN = 1;
const int TABLE_NAME_COLUMN = 0;
const int TABLE_VALUE_COLUMN = 1;
const int TABLE_TYPE_COLUMN = 2;
//////////////////////////////////////////////////////////////////////////
class CJsonConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CJsonConfigDialog(QWidget* parent = Q_NULLPTR);
    ~CJsonConfigDialog();

    void InitDialog(CJsonConfig* pConfig);

    void SetDirty( bool isDirty );
    void FillTableProperties();
    int FindTableProprtyRow( const QString& name ); //return -1 if not found

private:
    QString GetCurrentTreePath();
    uint32_t GetFlagsTreeNode( QTreeWidgetItem* node );
    void SavePropertyValue( int row, const QString& name, const QString& value, const QString& type );
    void FillTreeNode( const Json::Value& jValue, QTreeWidgetItem* parent );
    void clickOkButton();
    void clickCancelButton();
    void clickDiscardButton();
    void clickApplyButton();
    void Reset();

private Q_SLOTS:
    void addNewProperty();
    void delProperty();
    void editProperty();

    void addNewPropertiesSet();
    void delPropertiesSet();
    void editPropertiesSet();

    void clickButton( QAbstractButton* button );

    void itemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous );
    void treeContextMenuRequested( const QPoint& pos );

private:
    Ui::JsonConfigDialog uiConf;
    CJsonConfig* m_pConfig;
    QTreeWidgetItem* m_currentTreeItem;
    QMenu* m_treeContextMnu;
    bool m_bDirty;
};

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
    explicit CJsonConfigDialog(CJsonConfig* pConfig, QWidget* parent = Q_NULLPTR);
    ~CJsonConfigDialog();

    void InitDialog();

private:
    QString GetTreePath( const QTreeWidgetItem* node, bool withRootNode = false );
    uint32_t GetFlagsTreeNode( QTreeWidgetItem* node );
    void FillTreeNode( const Json::Value& jValue, QTreeWidgetItem* parent );
    QTreeWidgetItem* GetCurrentTreeNode();

    int AddNewPropertyValue( const SValueView& vv );
    void SetPropertyValue( int row, const SValueView& vv );
    SValueView GetPropertyValue( int row );
    void FillTableProperties();
    void FillTableProperties( const VectorValues& props );
    int FindTablePropertyRow( const QString& name ); //return -1 if not found
    void ClearTableProperties();

    void clickOkButton();
    void clickCancelButton();
    void clickDiscardButton();
    void clickApplyButton();
    void Reset();

    void RestoreTreeCursorPosition( const QString& parentText, const QString& currText, const QString& flags );

    void SetDirty( bool isDirty );

private Q_SLOTS:
    void addNewProperty();
    void delProperty();
    void editProperty();
    void itemTableChanged( QTableWidgetItem* item );
    void SaveTablePropertiesSet( QTreeWidgetItem* previous );

    void addNewPropertiesSet();
    void delPropertiesSet();
    void editPropertiesSet();
    void itemTreeChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous );
    void treeContextMenuRequested( const QPoint& pos );

    void clickButton( QAbstractButton* button );

protected:
    void closeEvent( QCloseEvent* event ) override;

private:
    Ui::JsonConfigDialog uiConf;
    CJsonConfig* m_pConfig;
    QMenu* m_treeContextMnu;
    bool m_bDirty;
};

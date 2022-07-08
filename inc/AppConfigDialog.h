#pragma once

//////////////////////////////////////////////////////////////////////////

#include "AppConfig.h"
#include "ui_AppConfig.h"
#include <QDialog>

class CAppConfig;
class QCloseEvent;
class QTableWidgetItem;
class QTreeWidgetItem;
class QAbstractButton;
class QMenu;

//////////////////////////////////////////////////////////////////////////
class CAppConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CAppConfigDialog(CAppConfig& config, QWidget* parent = Q_NULLPTR);
    ~CAppConfigDialog();

private:
    void InitDialog();
    void FillTreeNode(const QString& parentName = "", QTreeWidgetItem* parent = Q_NULLPTR);
    void SetDirty( bool isDirty ) { m_bDirty = isDirty; }

    // Get node propertis methods does NOT check input parameter
    // It is calling side responsibility
    QString GetNodePath( const QTreeWidgetItem* node ) const;
    quint32 GetNodeFlags( QTreeWidgetItem* node ) const;
    qint32 GetNodeChildsMaxCount( QTreeWidgetItem* node ) const;
    ETypeValue GetNodeType( QTreeWidgetItem* node ) const;

private Q_SLOTS:
    void addNewProperty();
    void delProperty();
    void editProperty();
    void itemTableChanged( QTableWidgetItem* item );
//    void SaveTablePropertiesSet( QTreeWidgetItem* previous );

    void addNewPropertiesSet();
    void delPropertiesSet();
    void editPropertiesSet();
    void itemTreeChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous );
//    void treeContextMenuRequested( const QPoint& pos );

    void clickButton( QAbstractButton* button );

protected:
    void closeEvent( QCloseEvent* event ) override;

private:
    Ui::AppConfigDialog uiConf;
    CAppConfig& m_config;
    QMenu* m_treeContextMnu;
    bool m_bDirty;
};

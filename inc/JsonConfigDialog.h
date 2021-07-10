#pragma once

//////////////////////////////////////////////////////////////////////////

#include "ui_JsonConfig.h"
#include "JsonConfig.h"
#include <QDialog>
#include <QStringList>

//////////////////////////////////////////////////////////////////////////
class CJsonConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CJsonConfigDialog(QWidget* parent = Q_NULLPTR);
    ~CJsonConfigDialog();

    void InitDialog(CJsonConfig* pConfig);
    void FillTableProperties();
    int FindTableProprtyRow( const QString& name ); //return -1 if not found

private:
    void SavePropertyValue( int row, const QString& name, const QString& value, const QString& type );
    QString GetCurrentTreePath();
    void FillTreeNode( const Json::Value& jValue, QTreeWidgetItem* parent );

private Q_SLOTS:
    void addNewProperty();
    void delProperty();
    void editProperty();


    void itemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous );


private:
    Ui::JsonConfigDialog uiConf;
    CJsonConfig* m_pConfig;
    QTreeWidgetItem* m_currentTreeItem;
    bool m_boolTableDirty;
};

#include "JsonConfigDialog.h"
#include "Common.h"
#include "VarEditDialog.h"
#include <QTableWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <cassert>

//////////////////////////////////////////////////////////////////////////
CJsonConfigDialog::CJsonConfigDialog(QWidget* parent) :
    QDialog( parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint ),
    m_pConfig(nullptr)
{
    uiConf.setupUi( this );
    setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint );
    setAttribute( Qt::WA_CustomWhatsThis );

    QFont headerFont( "Times", 10, QFont::Bold );

    // environment variables table
    {
//         QTableWidgetItem* nameHeaderItem = new QTableWidgetItem( tr( "Name" ) );
//         //nameHeaderItem->setIcon( QIcon( QPixmap( ":/Images/cubed.png" ) ) );
//         nameHeaderItem->setTextAlignment( Qt::AlignLeft );
//         nameHeaderItem->setFont( headerFont );
//         uiConf.envTableWidget->setHorizontalHeaderItem( 0, nameHeaderItem );

//         QTableWidgetItem* valHeaderItem = new QTableWidgetItem( tr( "Value" ) );
//         //valHeaderItem->setIcon( QIcon( QPixmap( ":/Images/cubed.png" ) ) );
//         valHeaderItem->setTextAlignment( Qt::AlignLeft );
//         valHeaderItem->setFont( headerFont );
//         uiConf.envTableWidget->setHorizontalHeaderItem( 1, valHeaderItem );
    }

    // object properties table
    {
        QTableWidgetItem* nameHeaderItem = new QTableWidgetItem( tr( "Name" ) );
        //nameHeaderItem->setIcon( QIcon( QPixmap( ":/Images/cubed.png" ) ) );
        nameHeaderItem->setTextAlignment( Qt::AlignLeft );
        nameHeaderItem->setFont( headerFont );
        uiConf.tablePropertiesWidget->setHorizontalHeaderItem( 0, nameHeaderItem );

        QTableWidgetItem* valHeaderItem = new QTableWidgetItem( tr( "Value" ) );
        //valHeaderItem->setIcon( QIcon( QPixmap( ":/Images/cubed.png" ) ) );
        valHeaderItem->setTextAlignment( Qt::AlignLeft );
        valHeaderItem->setFont( headerFont );
        uiConf.tablePropertiesWidget->setHorizontalHeaderItem( 1, valHeaderItem );

        QTableWidgetItem* typeHeaderItem = new QTableWidgetItem( tr( "Type" ) );
        //valHeaderItem->setIcon( QIcon( QPixmap( ":/Images/cubed.png" ) ) );
        typeHeaderItem->setTextAlignment( Qt::AlignLeft );
        typeHeaderItem->setFont( headerFont );
        uiConf.tablePropertiesWidget->setHorizontalHeaderItem( 2, typeHeaderItem );

        uiConf.tablePropertiesWidget->setLineWidth( uiConf.tablePropertiesWidget->width() );
    }

    QObject::connect( uiConf.addParamButton, SIGNAL( clicked() ), this, SLOT( addNewProperty() ) );
    QObject::connect( uiConf.delParamButton, SIGNAL( clicked() ), this, SLOT( delProperty() ) );
    QObject::connect( uiConf.editParamButton, SIGNAL( clicked() ), this, SLOT( editProperty() ) );
}

CJsonConfigDialog::~CJsonConfigDialog()
{

}

void CJsonConfigDialog::InitDialog(CJsonConfig* pConfig)
{
    if (pConfig != nullptr && m_pConfig == nullptr)
    {
        m_pConfig = pConfig;

        // fill object tree
        auto jRoot = m_pConfig->GetSettings();
        uiConf.treeObjectsWidget->setColumnCount( 1 );
        QList<QTreeWidgetItem*> items;
        auto count = jRoot.size();
        for (auto it = jRoot.begin(); it != jRoot.end(); ++it)
        {
            if (it->value().is_object())
            {
                const char* key = it->key_c_str();
                items.append( new QTreeWidgetItem( (QTreeWidget*)nullptr, QStringList( key ) ) );
            }
        }
        uiConf.treeObjectsWidget->insertTopLevelItems( 0, items );

//         boost::optional<boost::property_tree::ptree&> pt = m_pt.get_child_optional( KEY_ENV_PATH );
//         if (pt.has_value() && !pt.get().empty())
//         {
//             LOG_INFO << "Get environment:";
//             QTableWidget* table = dlg->GetEnvTableWidget();
//             for (const auto& it : pt.get())
//             {
//                 const int row = table->rowCount();
//                 table->setRowCount( row + 1 );
//                 const std::string eName = it.first;
//                 const std::string eValue = it.second.get_value<std::string>( "" );
//                 QTableWidgetItem* nameItem = new QTableWidgetItem( QString::fromStdString( eName ) );
//                 QTableWidgetItem* valueItem = new QTableWidgetItem( QString::fromStdString( eValue ) );
//                 table->setItem( row, 0, nameItem );
//                 table->setItem( row, 1, valueItem );
//                 LOG_INFO << "    " << eName << "=" << eValue;
//             }
//             table->sortItems( 0 );
//         }
//         else
//         {
//             LOG_INFO << "Get environment empty list";
//         }
    }
}

void CJsonConfigDialog::addNewProperty()
{
    QScopedPointer<CVarEditor> dlg( new CVarEditor( tr( "Create new property" ), Q_NULLPTR ) );
    if (dlg)
    {
        if ( dlg->exec() == QDialog::Accepted )
        {
            QString name = dlg->uiVarEdit.lineEditName->text();
            QString value = dlg->uiVarEdit.lineEditValue->text();
            int row = uiConf.tablePropertiesWidget->rowCount();
            uiConf.tablePropertiesWidget->setRowCount( row + 1 );
            QTableWidgetItem* newItem = new QTableWidgetItem( name );
            uiConf.tablePropertiesWidget->setItem( row, 0, newItem );
            newItem = new QTableWidgetItem( value );
            uiConf.tablePropertiesWidget->setItem( row, 1, newItem );
            uiConf.tablePropertiesWidget->sortItems( 0 );
        }
    }
}


void CJsonConfigDialog::delProperty()
{
    int row = uiConf.tablePropertiesWidget->currentRow();
    if (row >= 0)
    {
        uiConf.tablePropertiesWidget->removeRow( row );
        uiConf.tablePropertiesWidget->sortItems( 0 );
    }
}

void CJsonConfigDialog::editProperty()
{
    int row = uiConf.tablePropertiesWidget->currentRow();
    if (row != -1)
    {
        QTableWidgetItem* nameItem = uiConf.tablePropertiesWidget->item( row, 0 );
        QString name = nameItem->text();
        QTableWidgetItem* valueItem = uiConf.tablePropertiesWidget->item( row, 1 );
        QString value = valueItem->text();

        QScopedPointer<CVarEditor> dlg( new CVarEditor( tr( "Edit property" ), Q_NULLPTR ) );
        if (dlg)
        {
            dlg->uiVarEdit.lineEditName->setText( nameItem->text() );
            dlg->uiVarEdit.lineEditValue->setText( valueItem->text() );
            if (dlg->exec() == QDialog::Accepted)
            {
                nameItem->setText( dlg->uiVarEdit.lineEditName->text() );
                valueItem->setText( dlg->uiVarEdit.lineEditValue->text() );
                uiConf.tablePropertiesWidget->sortItems( 0 );
            }
        }
    }
}


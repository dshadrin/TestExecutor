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
    m_pConfig(nullptr),
    m_currentTreeItem(nullptr),
    m_boolTableDirty(false)
{
    uiConf.setupUi( this );
    setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint );
    setAttribute( Qt::WA_CustomWhatsThis );

    QFont headerFont( "Tahoma", 9, QFont::Bold );

    // tree of props sets
    {
        uiConf.treeObjectsWidget->setColumnCount( 2 );

        QTreeWidgetItem* nameHeaderItem = new QTreeWidgetItem( (QTreeWidget*)nullptr, QStringList { tr("Name"), tr("Count")});
        nameHeaderItem->setIcon( 0, QIcon( ":/common/icons8-name-tag-24.png" ) );
        nameHeaderItem->setIcon( 1, QIcon( ":/common/icons8-counter-50.png" ) );
        nameHeaderItem->setTextAlignment( 0, Qt::AlignLeft );
        nameHeaderItem->setTextAlignment( 1, Qt::AlignLeft );
        nameHeaderItem->setFont( 0, headerFont );
        nameHeaderItem->setFont( 1, headerFont );
        uiConf.treeObjectsWidget->setHeaderItem( nameHeaderItem );
        uiConf.treeObjectsWidget->header()->resizeSection( 1, 40 );
        uiConf.treeObjectsWidget->header()->setStretchLastSection( false );
        uiConf.treeObjectsWidget->header()->setSectionResizeMode( 0, QHeaderView::Stretch );
        uiConf.treeObjectsWidget->header()->setStyleSheet( QString( "QHeaderView::section {background-color: rgb(222, 222, 222);}" ) );
    }

    // object properties table
    {
        uiConf.tablePropertiesWidget->setColumnCount( 3 );

        QTableWidgetItem* nameHeaderItem = new QTableWidgetItem( tr( "Name" ) );
        nameHeaderItem->setIcon( QIcon( ":/common/icons8-name-tag-24.png" ) );
        nameHeaderItem->setTextAlignment( Qt::AlignLeft );
        nameHeaderItem->setFont( headerFont );
        uiConf.tablePropertiesWidget->setHorizontalHeaderItem( 0, nameHeaderItem );

        QTableWidgetItem* valHeaderItem = new QTableWidgetItem( tr( "Value" ) );
        valHeaderItem->setIcon( QIcon( ":/common/icons8-important-property-24.png"));
        valHeaderItem->setTextAlignment( Qt::AlignLeft );
        valHeaderItem->setFont( headerFont );
        uiConf.tablePropertiesWidget->setHorizontalHeaderItem( 1, valHeaderItem );

        QTableWidgetItem* typeHeaderItem = new QTableWidgetItem( tr( "Type" ) );
        typeHeaderItem->setIcon( QIcon( ":/common/icons8-paste-as-text-24.png"));
        typeHeaderItem->setTextAlignment( Qt::AlignLeft );
        typeHeaderItem->setFont( headerFont );
        uiConf.tablePropertiesWidget->setHorizontalHeaderItem( 2, typeHeaderItem );

        uiConf.tablePropertiesWidget->setLineWidth( uiConf.tablePropertiesWidget->width() );
        uiConf.tablePropertiesWidget->horizontalHeader()->setStretchLastSection( false );
        uiConf.tablePropertiesWidget->horizontalHeader()->resizeSection( 2, 60 );
        uiConf.tablePropertiesWidget->horizontalHeader()->resizeSection( 0, 250 );
        uiConf.tablePropertiesWidget->horizontalHeader()->setSectionResizeMode( 1, QHeaderView::Stretch );
        uiConf.tablePropertiesWidget->horizontalHeader()->setStyleSheet( QString( "QHeaderView::section {background-color: rgb(222, 222, 222);}" ) );
    }

    // table edit buttons
    QObject::connect( uiConf.addParamButton, SIGNAL( clicked() ), this, SLOT( addNewProperty() ) );
    QObject::connect( uiConf.delParamButton, SIGNAL( clicked() ), this, SLOT( delProperty() ) );
    QObject::connect( uiConf.editParamButton, SIGNAL( clicked() ), this, SLOT( editProperty() ) );

    // tree edit buttons

    // tree navigation signals
    QObject::connect( uiConf.treeObjectsWidget, SIGNAL( currentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT( itemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
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

        QTreeWidgetItem* currentItem = new QTreeWidgetItem( (QTreeWidget*)nullptr, QStringList( "Configuration"));
        currentItem->setData( 0, Qt::UserRole, 0 );
        FillTreeNode( jRoot, currentItem );
        QList<QTreeWidgetItem*> items { currentItem };

        uiConf.treeObjectsWidget->insertTopLevelItems( 0, items );
        uiConf.treeObjectsWidget->setCurrentItem( currentItem );
        uiConf.treeObjectsWidget->expandAll();

        FillTableProperties();
    }
}

void CJsonConfigDialog::FillTreeNode( const Json::Value& jValue, QTreeWidgetItem* parent )
{
    QList<QTreeWidgetItem*> items;

    auto handleItem = [&]( const Json::Value& jv, const QString& itemName ) -> void
    {
        if (jv.isObject() || jv.isArray())
        {
            const QString sType = CJsonConfig::JsonTypeToStringType( jv.type() );
            QTreeWidgetItem* item = new QTreeWidgetItem( parent, QStringList { itemName, QString::number(jv.size())}, (int)CJsonConfig::StringTypeToProjectType(sType));
            item->setData( 0, Qt::UserRole, &jv );
            FillTreeNode( jv, item );
            items.append( item );
        }
    };

    if (jValue.isObject())
    {
        Json::Value::Members listNames = jValue.getMemberNames();
        for (size_t i = 0; i < listNames.size(); ++i)
        {
            const Json::Value& jv = jValue[listNames[i]];
            handleItem( jv, listNames[i].c_str() );
        }
    }
    else if (jValue.isArray())
    {
        int count = jValue.size();
        for (int i = 0; i < count; ++i)
        {
            const Json::Value& jv = jValue[i];
            handleItem( jv, std::to_string( i ).c_str() );
        }
    }
}


void CJsonConfigDialog::FillTableProperties()
{
    QString path = GetCurrentTreePath();
    if (!path.isEmpty())
    {
        std::vector<SValueView> props = m_pConfig->GetProperties( path );
        if (!props.empty())
        {
            for (auto& it : props)
            {
                const int row = uiConf.tablePropertiesWidget->rowCount();
                uiConf.tablePropertiesWidget->setRowCount( row + 1 );
                uiConf.tablePropertiesWidget->setItem( row, 0, new QTableWidgetItem( it.name ) );
                uiConf.tablePropertiesWidget->setItem( row, 1, new QTableWidgetItem( it.value ) );
                uiConf.tablePropertiesWidget->setItem( row, 2, new QTableWidgetItem( it.type ) );
            }
            uiConf.tablePropertiesWidget->sortItems( 0 );
        }
        else
        {
            uiConf.editParamButton->setEnabled( false );
            uiConf.delParamButton->setEnabled( false );

        }
    }
}


int CJsonConfigDialog::FindTableProprtyRow( const QString& name )
{
    int row = -1;
    int count = uiConf.tablePropertiesWidget->rowCount();
    for (int i = 0; i < count; ++i)
    {
        if (name.compare( uiConf.tablePropertiesWidget->item( i, 0 )->text(), Qt::CaseSensitivity::CaseInsensitive ) == 0)
        {
            row = i;
            break;
        }
    }
    return row;
}


void CJsonConfigDialog::SavePropertyValue( int row, const QString& name, const QString& value, const QString& type )
{
    QTableWidgetItem* nameItem = uiConf.tablePropertiesWidget->item( row, 0 );
    QTableWidgetItem* valueItem = uiConf.tablePropertiesWidget->item( row, 1 );
    QTableWidgetItem* typeItem = uiConf.tablePropertiesWidget->item( row, 2 );
    nameItem->setText( name );
    valueItem->setText( value );
    typeItem->setText( type );
    uiConf.tablePropertiesWidget->sortItems( 0 );
}


QString CJsonConfigDialog::GetCurrentTreePath()
{
    QString result;
    QTreeWidgetItem* currentItem = uiConf.treeObjectsWidget->currentItem();
    if (currentItem != nullptr)
    {
        result = currentItem->text( 0 );
        while (currentItem->parent() != nullptr)
        {
            currentItem = currentItem->parent();
            result = currentItem->text( 0 ) + "." + result;
        }
    }
    return result;
}

void CJsonConfigDialog::addNewProperty()
{
    QScopedPointer<CVarEditor> dlg( new CVarEditor( tr( "Create new property" ), Q_NULLPTR ) );
    if (dlg)
    {
        bool isRun = true;
        while ( isRun && dlg->exec() == QDialog::Accepted )
        {
            QString name = dlg->uiVarEdit.lineEditName->text();
            QString value = dlg->uiVarEdit.lineEditValue->text();
            QString type = dlg->uiVarEdit.comboBoxTypeJson->currentText();
            int row = FindTableProprtyRow( name );
            if (row < 0)
            {
                row = uiConf.tablePropertiesWidget->rowCount();
                uiConf.tablePropertiesWidget->setRowCount( row + 1 );
                QTableWidgetItem* newItem = new QTableWidgetItem( name );
                uiConf.tablePropertiesWidget->setItem( row, 0, newItem );
                newItem = new QTableWidgetItem( value );
                uiConf.tablePropertiesWidget->setItem( row, 1, newItem );
                newItem = new QTableWidgetItem( type );
                uiConf.tablePropertiesWidget->setItem( row, 2, newItem );
                uiConf.tablePropertiesWidget->sortItems( 0 );
                isRun = false;
            }
            else
            {
                QMessageBox msgBox;
                msgBox.setText( "Property with name " + name + " exists already");
                msgBox.setInformativeText( "Do you want to save your changes?" );
                msgBox.setStandardButtons( QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );
                msgBox.setDefaultButton( QMessageBox::Save );
                int ret = msgBox.exec();
                switch (ret)
                {
                case QMessageBox::Save:
                    SavePropertyValue( row, name, value, type );
                    isRun = false;
                    break;
                case QMessageBox::Discard:
                    // Don't Save, return to edit
                    break;
                case QMessageBox::Cancel:
                default:
                    // Cancel was clicked
                    isRun = false;
                    break;
                }
            }
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
        QTableWidgetItem* valueItem = uiConf.tablePropertiesWidget->item( row, 1 );
        QTableWidgetItem* typeItem = uiConf.tablePropertiesWidget->item( row, 2 );

        QScopedPointer<CVarEditor> dlg( new CVarEditor( tr( "Edit property" ), Q_NULLPTR ) );
        if (dlg)
        {
            // set dialog data
            dlg->uiVarEdit.lineEditName->setText( nameItem->text() );
            dlg->uiVarEdit.lineEditValue->setText( valueItem->text() );
            dlg->SetCurrentType( typeItem->text() );
            if (dlg->exec() == QDialog::Accepted)
            {
                // save dialog data
                QString name = dlg->uiVarEdit.lineEditName->text();
                int newRow = FindTableProprtyRow( name );
                if (newRow != -1 && newRow != row)
                {
                    // we are edit other property, because save other
                    row = newRow;
                }
                SavePropertyValue( row, name, dlg->uiVarEdit.lineEditValue->text(), dlg->uiVarEdit.comboBoxTypeJson->currentText() );
            }
        }
    }
}

void CJsonConfigDialog::itemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{
    assert( m_currentTreeItem == previous );
    if (current != nullptr)
    {
        m_currentTreeItem = current;
        uiConf.tablePropertiesWidget->clear();
    }
}


#include "JsonConfigDialog.h"
#include "Common.h"
#include "VarEditDialog.h"
#include <QTableWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QAction>
#include <QInputDialog>
#include <cassert>

//////////////////////////////////////////////////////////////////////////
const int HEX_BASE = 16;

//////////////////////////////////////////////////////////////////////////
CJsonConfigDialog::CJsonConfigDialog(QWidget* parent) :
    QDialog( parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint ),
    m_pConfig(nullptr),
    m_currentTreeItem(nullptr),
    m_treeContextMnu(nullptr),
    m_bDirty(false)
{
    uiConf.setupUi( this );
    setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint );
    setAttribute( Qt::WA_CustomWhatsThis );

    QFont headerFont( "Tahoma", 9, QFont::Bold );

    // tree of props sets
    {
        uiConf.treeObjectsWidget->setColumnCount( 2 );

        QTreeWidgetItem* nameHeaderItem = new QTreeWidgetItem( (QTreeWidget*)nullptr, QStringList { tr("Name"), tr("Flags")});
        nameHeaderItem->setIcon( TREE_VALUE_COLUMN, QIcon( ":/common/icons8-name-tag-24.png" ) );
        nameHeaderItem->setIcon( TREE_FLAGS_COLUMN, QIcon( ":/common/icons8-counter-50.png" ) );
        nameHeaderItem->setTextAlignment( TREE_VALUE_COLUMN, Qt::AlignLeft );
        nameHeaderItem->setTextAlignment( TREE_FLAGS_COLUMN, Qt::AlignLeft );
        nameHeaderItem->setFont( TREE_VALUE_COLUMN, headerFont );
        nameHeaderItem->setFont( TREE_FLAGS_COLUMN, headerFont );
        uiConf.treeObjectsWidget->setHeaderItem( nameHeaderItem );
        uiConf.treeObjectsWidget->header()->resizeSection( TREE_FLAGS_COLUMN, 40 );
        uiConf.treeObjectsWidget->header()->setStretchLastSection( false );
        uiConf.treeObjectsWidget->header()->setSectionResizeMode( TREE_VALUE_COLUMN, QHeaderView::Stretch );
        uiConf.treeObjectsWidget->header()->setStyleSheet( QString( "QHeaderView::section {background-color: rgb(222, 222, 222);}" ) );
    }

    // object properties table
    {
        uiConf.tablePropertiesWidget->setColumnCount( 3 );

        QTableWidgetItem* nameHeaderItem = new QTableWidgetItem( tr( "Name" ) );
        nameHeaderItem->setIcon( QIcon( ":/common/icons8-name-tag-24.png" ) );
        nameHeaderItem->setTextAlignment( Qt::AlignLeft );
        nameHeaderItem->setFont( headerFont );
        uiConf.tablePropertiesWidget->setHorizontalHeaderItem( TABLE_NAME_COLUMN, nameHeaderItem );

        QTableWidgetItem* valHeaderItem = new QTableWidgetItem( tr( "Value" ) );
        valHeaderItem->setIcon( QIcon( ":/common/icons8-important-property-24.png"));
        valHeaderItem->setTextAlignment( Qt::AlignLeft );
        valHeaderItem->setFont( headerFont );
        uiConf.tablePropertiesWidget->setHorizontalHeaderItem( TABLE_VALUE_COLUMN, valHeaderItem );

        QTableWidgetItem* typeHeaderItem = new QTableWidgetItem( tr( "Type" ) );
        typeHeaderItem->setIcon( QIcon( ":/common/icons8-paste-as-text-24.png"));
        typeHeaderItem->setTextAlignment( Qt::AlignLeft );
        typeHeaderItem->setFont( headerFont );
        uiConf.tablePropertiesWidget->setHorizontalHeaderItem( 2, typeHeaderItem );

        uiConf.tablePropertiesWidget->setLineWidth( uiConf.tablePropertiesWidget->width() );
        uiConf.tablePropertiesWidget->horizontalHeader()->setStretchLastSection( false );
        uiConf.tablePropertiesWidget->horizontalHeader()->resizeSection( TABLE_TYPE_COLUMN, 60 );
        uiConf.tablePropertiesWidget->horizontalHeader()->resizeSection( TABLE_NAME_COLUMN, 250 );
        uiConf.tablePropertiesWidget->horizontalHeader()->setSectionResizeMode( 1, QHeaderView::Stretch );
        uiConf.tablePropertiesWidget->horizontalHeader()->setStyleSheet( QString( "QHeaderView::section {background-color: rgb(222, 222, 222);}" ) );
    }

    // actions
    QObject::connect( uiConf.actionAddNewPropertiesSet, SIGNAL( triggered() ), this, SLOT( addNewPropertiesSet() ) );
    QObject::connect( uiConf.actionDelPropertiesSet, SIGNAL( triggered() ), this, SLOT( delPropertiesSet() ) );
    QObject::connect( uiConf.actionEditPropertiesSet, SIGNAL( triggered() ), this, SLOT( editPropertiesSet() ) );

    // tree context menu
    m_treeContextMnu = new QMenu( this );

    // run context menu
    connect( uiConf.treeObjectsWidget, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( treeContextMenuRequested( const QPoint& ) ) );

    // table edit buttons
    QObject::connect( uiConf.addParamButton, SIGNAL( clicked() ), this, SLOT( addNewProperty() ) );
    QObject::connect( uiConf.delParamButton, SIGNAL( clicked() ), this, SLOT( delProperty() ) );
    QObject::connect( uiConf.editParamButton, SIGNAL( clicked() ), this, SLOT( editProperty() ) );

    // tree navigation signals
    QObject::connect( uiConf.treeObjectsWidget, SIGNAL( currentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT( itemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ) );

    // dialog buttons signals
    QObject::connect( uiConf.buttonBox, &QDialogButtonBox::clicked, this, &CJsonConfigDialog::clickButton );
}

CJsonConfigDialog::~CJsonConfigDialog()
{

}

void CJsonConfigDialog::InitDialog(CJsonConfig* pConfig)
{
    if (pConfig != nullptr && m_pConfig == nullptr)
    {
        m_pConfig = pConfig;
        m_pConfig->CreateTmpConfig();

        // fill object tree
        auto jRoot = m_pConfig->GetSettings();

        QTreeWidgetItem* currentItem = new QTreeWidgetItem( (QTreeWidget*)nullptr,
                                                            QStringList { CJsonConfig::GetRootNodeName(),
                                                                          QString().asprintf( "%04X", CJsonConfig::GetNodeFlagsByName( CJsonConfig::GetRootNodeName() ) ) } );
        currentItem->setData( TREE_VALUE_COLUMN, Qt::UserRole, 0 );
        FillTreeNode( jRoot, currentItem );
        QList<QTreeWidgetItem*> items { currentItem };

        uiConf.treeObjectsWidget->insertTopLevelItems( TREE_VALUE_COLUMN, items );
        uiConf.treeObjectsWidget->setCurrentItem( currentItem );
        uiConf.treeObjectsWidget->expandAll();
        uiConf.treeObjectsWidget->sortByColumn( 1, Qt::SortOrder::AscendingOrder );

        FillTableProperties();
        SetDirty( false );
    }
}

void CJsonConfigDialog::SetDirty(bool isDirty)
{
    m_bDirty = isDirty;
    uiConf.buttonBox->button( QDialogButtonBox::StandardButton::Apply )->setEnabled( m_bDirty );
    uiConf.buttonBox->button( QDialogButtonBox::StandardButton::Reset )->setEnabled( m_bDirty );
}

void CJsonConfigDialog::FillTreeNode( const Json::Value& jValue, QTreeWidgetItem* parent )
{
    QList<QTreeWidgetItem*> items;

    auto handleItem = [&]( const Json::Value& jv, const QString& itemName ) -> void
    {
        if (jv.isObject() || jv.isArray())
        {
            const QString sType = CJsonConfig::JsonTypeToStringType( jv.type() );
            const uint32_t uiFlags = CJsonConfig::GetNodeFlagsByName( itemName );
            const QString flags = QString().asprintf( "%04X", uiFlags);
            QTreeWidgetItem* item = new QTreeWidgetItem( parent, QStringList { itemName, flags }, (int)CJsonConfig::StringTypeToProjectType(sType));
            item->setData( TREE_VALUE_COLUMN, Qt::UserRole, &jv );
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
                uiConf.tablePropertiesWidget->setItem( row, TABLE_NAME_COLUMN, new QTableWidgetItem( it.name ) );
                uiConf.tablePropertiesWidget->setItem( row, TABLE_VALUE_COLUMN, new QTableWidgetItem( it.value ) );
                uiConf.tablePropertiesWidget->setItem( row, TABLE_TYPE_COLUMN, new QTableWidgetItem( it.type ) );
            }
            uiConf.tablePropertiesWidget->sortItems( TABLE_NAME_COLUMN );
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
        if (name.compare( uiConf.tablePropertiesWidget->item( i, TABLE_NAME_COLUMN )->text(), Qt::CaseSensitivity::CaseInsensitive ) == 0)
        {
            row = i;
            break;
        }
    }
    return row;
}


void CJsonConfigDialog::SavePropertyValue( int row, const QString& name, const QString& value, const QString& type )
{
    QTableWidgetItem* nameItem = uiConf.tablePropertiesWidget->item( row, TABLE_NAME_COLUMN );
    QTableWidgetItem* valueItem = uiConf.tablePropertiesWidget->item( row, TABLE_VALUE_COLUMN );
    QTableWidgetItem* typeItem = uiConf.tablePropertiesWidget->item( row, TABLE_TYPE_COLUMN );
    nameItem->setText( name );
    valueItem->setText( value );
    typeItem->setText( type );
    uiConf.tablePropertiesWidget->sortItems( TABLE_NAME_COLUMN );
}


QString CJsonConfigDialog::GetCurrentTreePath()
{
    QString result;
    QTreeWidgetItem* currentItem = uiConf.treeObjectsWidget->currentItem();
    if (currentItem != nullptr)
    {
        result = currentItem->text( TREE_VALUE_COLUMN );
        while (currentItem->parent() != nullptr)
        {
            currentItem = currentItem->parent();
            result = currentItem->text( TREE_VALUE_COLUMN ) + "." + result;
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
                uiConf.tablePropertiesWidget->setItem( row, TABLE_NAME_COLUMN, newItem );
                newItem = new QTableWidgetItem( value );
                uiConf.tablePropertiesWidget->setItem( row, TABLE_VALUE_COLUMN, newItem );
                newItem = new QTableWidgetItem( type );
                uiConf.tablePropertiesWidget->setItem( row, TABLE_TYPE_COLUMN, newItem );
                uiConf.tablePropertiesWidget->sortItems( TABLE_NAME_COLUMN );
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
        uiConf.tablePropertiesWidget->sortItems( TABLE_NAME_COLUMN );
    }
}

void CJsonConfigDialog::editProperty()
{
    int row = uiConf.tablePropertiesWidget->currentRow();
    if (row != -1)
    {
        QTableWidgetItem* nameItem = uiConf.tablePropertiesWidget->item( row, TABLE_NAME_COLUMN );
        QTableWidgetItem* valueItem = uiConf.tablePropertiesWidget->item( row, TABLE_VALUE_COLUMN );
        QTableWidgetItem* typeItem = uiConf.tablePropertiesWidget->item( row, TABLE_TYPE_COLUMN );

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

void CJsonConfigDialog::treeContextMenuRequested( const QPoint& pos )
{
    QList<QAction*> actions;
    QTreeWidgetItem* item = uiConf.treeObjectsWidget->currentItem();
    uint32_t flags = GetFlagsTreeNode( item );

    if (flags & JO_APPEND)
    {
        actions.push_back( uiConf.actionAddNewPropertiesSet );
    }
    else if (flags == JO_UNSPECIFIED)
    {
        actions.push_back( uiConf.actionDelPropertiesSet );
        actions.push_back( uiConf.actionEditPropertiesSet );
    }

    m_treeContextMnu->exec( actions, uiConf.treeObjectsWidget->mapToGlobal( pos ) );
}

void CJsonConfigDialog::itemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{
    assert( m_currentTreeItem == previous );
    if (current != nullptr)
    {
        m_currentTreeItem = current;
        uiConf.tablePropertiesWidget->clear();

        ulong flags = GetFlagsTreeNode( m_currentTreeItem );
        uiConf.addSetButton->setEnabled( (flags & JO_APPEND) ? true : false );
        uiConf.delSetButton->setEnabled( (flags == JO_UNSPECIFIED) ? true : false );
        uiConf.editSetButton->setEnabled( (flags == JO_UNSPECIFIED) ? true : false );
        uiConf.addParamButton->setEnabled( (flags == JO_UNSPECIFIED) ? true : false );
        uiConf.delParamButton->setEnabled( false );
        uiConf.editParamButton->setEnabled( false );
    }
}

void CJsonConfigDialog::addNewPropertiesSet()
{
    bool isMultiple = false;
    uint32_t flags = GetFlagsTreeNode( m_currentTreeItem );
    uint32_t linkFlag = flags & JO_LINKS;
    QString title = (linkFlag) ? tr("Select ") : tr("Define new ");
    QString unit = CJsonConfig::GetUnitNameByFlag( flags & JO_MAIN_MASK, &isMultiple );
    title.append( unit );

    bool ok = false;
    QString text = "";

    if (!linkFlag)
    {
        text = QInputDialog::getText( this, title,
                                      unit + tr( " name:" ), QLineEdit::Normal,
                                      "", &ok );
    }

    if (ok && !text.isEmpty())
    {
        // insert new object
        QString path = GetCurrentTreePath();
        path.append( u'.' );
        path.append( text );
        if (!m_pConfig->IsNodeExists( path ))
        {
            QTreeWidgetItem* item = new QTreeWidgetItem( m_currentTreeItem, QStringList { text, "" }, linkFlag ? (int)ETypeValue::link : (int)ETypeValue::object_value );
            m_currentTreeItem->addChild( item );
            uiConf.treeObjectsWidget->setCurrentItem( item );
            SetDirty( true );
        }
    }
}


void CJsonConfigDialog::delPropertiesSet()
{

}


void CJsonConfigDialog::editPropertiesSet()
{

}

void CJsonConfigDialog::clickButton( QAbstractButton* button )
{
    const QDialogButtonBox::ButtonRole role = uiConf.buttonBox->buttonRole( button );
    switch (role)
    {
    case QDialogButtonBox::AcceptRole:
        clickOkButton();
        break;
    case QDialogButtonBox::RejectRole:
        clickCancelButton();
        break;
    case QDialogButtonBox::ResetRole:
        clickDiscardButton();
        break;
    case QDialogButtonBox::ApplyRole:
        clickApplyButton();
        break;
    default:
        break;
    }
}

uint32_t CJsonConfigDialog::GetFlagsTreeNode( QTreeWidgetItem* node )
{
    bool ok = false;
    uint32_t flags = node->text( 1 ).toULong( &ok, HEX_BASE );
    return ok ? flags : JO_UNSPECIFIED;
}

void CJsonConfigDialog::clickOkButton()
{
    m_pConfig->RemoveTmpConfig();
    QDialog::accept();
}

void CJsonConfigDialog::clickCancelButton()
{
    m_pConfig->RestoreConfig();
    m_pConfig->RemoveTmpConfig();
    QDialog::reject();
}

void CJsonConfigDialog::clickDiscardButton()
{
    m_pConfig->RestoreConfig();
    Reset();
}

void CJsonConfigDialog::clickApplyButton()
{
    m_pConfig->CreateTmpConfig();
    SetDirty( false );
}

void CJsonConfigDialog::Reset()
{
    uiConf.treeObjectsWidget->clear();
    uiConf.tablePropertiesWidget->clear();
    SetDirty( false );
    m_currentTreeItem = nullptr;
    CJsonConfig* pConfig = m_pConfig;
    m_pConfig = nullptr;
    InitDialog( pConfig );
}



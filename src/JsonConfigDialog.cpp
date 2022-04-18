#include "JsonConfigDialog.h"
#include "Common.h"
#include "VarEditDialog.h"
#include <QTableWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QAction>
#include <QInputDialog>
#include <QStringList>
#include <cassert>

//////////////////////////////////////////////////////////////////////////
const int HEX_BASE = 16;

//////////////////////////////////////////////////////////////////////////
CJsonConfigDialog::CJsonConfigDialog( CJsonConfig* pConfig, QWidget* parent) :
    QDialog( parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint ),
    m_pConfig( pConfig ),
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
        uiConf.tablePropertiesWidget->setHorizontalHeaderItem( TABLE_TYPE_COLUMN, typeHeaderItem );

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
    QObject::connect( uiConf.tablePropertiesWidget, SIGNAL( itemChanged( QTableWidgetItem* ) ), this, SLOT( itemTableChanged( QTableWidgetItem* ) ) );

    // tree navigation signals
    QObject::connect( uiConf.treeObjectsWidget, SIGNAL( currentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT( itemTreeChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ) );

    // dialog buttons signals
    QObject::connect( uiConf.buttonBox, &QDialogButtonBox::clicked, this, &CJsonConfigDialog::clickButton );

    InitDialog();
}

CJsonConfigDialog::~CJsonConfigDialog()
{

}

void CJsonConfigDialog::InitDialog()
{
    if (m_pConfig != nullptr)
    {
        m_pConfig->CreateBackup();

        // fill object tree
        const Json::Value& jRoot = const_cast<const CJsonConfig*>(m_pConfig)->GetSettings();

        QTreeWidgetItem* currentItem = new QTreeWidgetItem( (QTreeWidget*)nullptr,
                                                            QStringList { CJsonConfig::GetRootNodeName(),
                                                                          QString().asprintf( "%04X", CJsonConfig::GetNodeFlagsByName( CJsonConfig::GetRootNodeName() ) ) } );
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
            uint32_t uiFlags = CJsonConfig::GetNodeFlagsByName( itemName );
            if (uiFlags == 0 && parent)
            {
                const uint32_t parentFlags = GetFlagsTreeNode( parent );
                if (parentFlags & JO_ACCESS_MASK)
                {
                    uiFlags = parentFlags & JO_MAIN_MASK;
                }
            }
            if (uiFlags)
            {
                const QString flags = QString().asprintf( "%04X", uiFlags );
                QTreeWidgetItem* item = new QTreeWidgetItem( parent, QStringList { itemName, flags }, (int)CJsonConfig::StringTypeToProjectType( sType ) );

                if (uiFlags & JO_LINKS)
                {
                    VectorValues vv = m_pConfig->GetProperties( GetTreePath( parent ) + "." + itemName );
                    if (!vv.empty())
                    {
                        for (const auto& v : vv)
                        {
                            QTreeWidgetItem* newItem = new QTreeWidgetItem( item, QStringList { v.name, QString().asprintf( "%04X", uiFlags & ~JO_ACCESS_MASK ) }, (int)ETypeValue::link );
                            newItem->setData( TREE_VALUE_COLUMN, Qt::UserRole, v.value );
                            item->addChild( newItem );
                        }
                    }
                }
                else
                {
                    FillTreeNode( jv, item );
                }
                items.append( item );
            }
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

QTreeWidgetItem* CJsonConfigDialog::GetCurrentTreeNode()
{
    return uiConf.treeObjectsWidget->currentItem();
}

void CJsonConfigDialog::FillTableProperties()
{
    uint32_t uiFlags = GetFlagsTreeNode( GetCurrentTreeNode() );
    bool linkPath = ((uiFlags & JO_LINKS) && (uiFlags & JO_ACCESS_MASK) == 0);
    QString path = linkPath ? GetCurrentTreeNode()->data( TREE_VALUE_COLUMN, Qt::UserRole ).toString() : GetTreePath( GetCurrentTreeNode() );
    if (!path.isEmpty())
    {
        VectorValues props = m_pConfig->GetProperties( path );
        FillTableProperties( props );
        if (linkPath)
        {
            uiConf.addParamButton->setEnabled( false );
            uiConf.editParamButton->setEnabled( false );
            uiConf.delParamButton->setEnabled( false );
        }
    }
}

void CJsonConfigDialog::FillTableProperties( const VectorValues& props )
{
    if (!props.empty())
    {
        for (auto& it : props)
        {
            AddNewPropertyValue( it );
        }
        uiConf.tablePropertiesWidget->sortItems( TABLE_NAME_COLUMN );
        uiConf.tablePropertiesWidget->setCurrentCell( 0, TABLE_NAME_COLUMN );
    }
    else
    {
        uiConf.editParamButton->setEnabled( false );
        uiConf.delParamButton->setEnabled( false );
    }
}

int CJsonConfigDialog::FindTablePropertyRow( const QString& name )
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


void CJsonConfigDialog::SetPropertyValue( int row, const SValueView& vv )
{
    if (row >= 0 && row < uiConf.tablePropertiesWidget->rowCount())
    {
        uiConf.tablePropertiesWidget->item( row, TABLE_NAME_COLUMN )->setText( vv.name );
        uiConf.tablePropertiesWidget->item( row, TABLE_VALUE_COLUMN )->setText( vv.value );
        uiConf.tablePropertiesWidget->item( row, TABLE_TYPE_COLUMN )->setText( vv.type );
    }
}

SValueView CJsonConfigDialog::GetPropertyValue( int row )
{
    SValueView vv;
    if (row >= 0 && row < uiConf.tablePropertiesWidget->rowCount())
    {
        vv.name = uiConf.tablePropertiesWidget->item( row, TABLE_NAME_COLUMN )->text();
        vv.value = uiConf.tablePropertiesWidget->item( row, TABLE_VALUE_COLUMN )->text();
        vv.type = uiConf.tablePropertiesWidget->item( row, TABLE_TYPE_COLUMN )->text();
    }
    return vv;
}

int CJsonConfigDialog::AddNewPropertyValue( const SValueView& vv )
{
    int row = uiConf.tablePropertiesWidget->rowCount();
    uiConf.tablePropertiesWidget->setRowCount( row + 1 );
    uiConf.tablePropertiesWidget->setItem( row, TABLE_NAME_COLUMN, new QTableWidgetItem( vv.name ) );
    uiConf.tablePropertiesWidget->setItem( row, TABLE_VALUE_COLUMN, new QTableWidgetItem( vv.value ) );
    uiConf.tablePropertiesWidget->setItem( row, TABLE_TYPE_COLUMN, new QTableWidgetItem( vv.type ) );
    return row;
}

QString CJsonConfigDialog::GetTreePath( const QTreeWidgetItem* node, bool withRootNode )
{
    QString result;
    const QTreeWidgetItem* item = node;
    if (item != nullptr)
    {
        const QString rootName = CJsonConfig::GetRootNodeName();
        QString nodeName;
        do 
        {
            nodeName = item->text( TREE_VALUE_COLUMN );
            if (withRootNode || nodeName != rootName)
            {
                result = result.isEmpty() ? nodeName : nodeName + "." + result;
            }
            item = item->parent();
        } while (item != nullptr);
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
            SValueView vv;
            vv.name = dlg->uiVarEdit.lineEditName->text();
            vv.value = dlg->uiVarEdit.lineEditValue->text();
            vv.type = dlg->uiVarEdit.comboBoxTypeJson->currentText();
            int row = FindTablePropertyRow( vv.name );
            if (row < 0)
            {
                row = AddNewPropertyValue( vv );
                uiConf.tablePropertiesWidget->sortItems( TABLE_NAME_COLUMN );
                SetDirty( true );
                SaveTablePropertiesSet( GetCurrentTreeNode() );
                isRun = false;
            }
            else
            {
                QMessageBox msgBox;
                msgBox.setText( "Property with name " + vv.name + " exists already");
                msgBox.setInformativeText( "Do you want to save your changes?" );
                msgBox.setStandardButtons( QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );
                msgBox.setDefaultButton( QMessageBox::Save );
                int ret = msgBox.exec();
                switch (ret)
                {
                case QMessageBox::Save:
                    SetPropertyValue( row, vv );
                    uiConf.tablePropertiesWidget->sortItems( TABLE_NAME_COLUMN );
                    SetDirty( true );
                    SaveTablePropertiesSet( GetCurrentTreeNode() );
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
        SetDirty( true );
        SaveTablePropertiesSet( GetCurrentTreeNode() );
    }
}

void CJsonConfigDialog::editProperty()
{
    int row = uiConf.tablePropertiesWidget->currentRow();
    if (row != -1)
    {
        SValueView vv = GetPropertyValue( row );

        QScopedPointer<CVarEditor> dlg( new CVarEditor( tr( "Edit property" ), Q_NULLPTR ) );
        if (dlg)
        {
            // set dialog data
            dlg->uiVarEdit.lineEditName->setText( vv.name );
            dlg->uiVarEdit.lineEditValue->setText( vv.value );
            dlg->SetCurrentType( vv.type );
            if (dlg->exec() == QDialog::Accepted)
            {
                // save dialog data
                vv.name = dlg->uiVarEdit.lineEditName->text();
                vv.value = dlg->uiVarEdit.lineEditValue->text();
                vv.type = dlg->uiVarEdit.comboBoxTypeJson->currentText();

                int newRow = FindTablePropertyRow( vv.name );
                if (newRow != -1 && newRow != row)
                {
                    // we are edit other property, because save other
                    row = newRow;
                }
                SetPropertyValue( row, vv );
                SetDirty( true );
                SaveTablePropertiesSet( GetCurrentTreeNode() );
            }
        }
    }
}

void CJsonConfigDialog::itemTableChanged( QTableWidgetItem* item )
{
    if (item != nullptr)
    {
        uiConf.delParamButton->setEnabled( true );
        uiConf.editParamButton->setEnabled( true );
    }
}

void CJsonConfigDialog::treeContextMenuRequested( const QPoint& pos )
{
    QList<QAction*> actions;
    uint32_t flags = GetFlagsTreeNode( GetCurrentTreeNode() );

    if (flags & JO_APPEND)
    {
        uiConf.actionAddNewPropertiesSet->setText( (flags & JO_LINKS) ? tr( "Select link to properties set" ) : tr( "Create new set of properties" ) );
        actions.push_back( uiConf.actionAddNewPropertiesSet );
    }
    else if ((flags & JO_ACCESS_MASK) == 0)
    {
        uiConf.actionDelPropertiesSet->setText( (flags & JO_APPEND) ? tr( "Delete link to properties set" ) : tr( "Delete set of properties" ) );
        uiConf.actionEditPropertiesSet->setText( (flags & JO_APPEND) ? tr( "Change link to properties set" ) : tr( "Edit name for set of properties" ) );
        actions.push_back( uiConf.actionDelPropertiesSet );
        actions.push_back( uiConf.actionEditPropertiesSet );
    }

    m_treeContextMnu->exec( actions, uiConf.treeObjectsWidget->mapToGlobal( pos ) );
}

void CJsonConfigDialog::itemTreeChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{
    uint32_t flags = GetFlagsTreeNode( current );
    if (current != nullptr)
    {
        uiConf.addSetButton->setEnabled( (flags & JO_APPEND) ? true : false );
        uiConf.delSetButton->setEnabled( ((flags & JO_ACCESS_MASK) == 0) ? true : false );
        uiConf.editSetButton->setEnabled( ((flags & JO_ACCESS_MASK) == 0) ? true : false );
        uiConf.addParamButton->setEnabled( ((flags & JO_ACCESS_MASK) == 0) ? true : false );
        uiConf.delParamButton->setEnabled( false );
        uiConf.editParamButton->setEnabled( false );
    }

    ClearTableProperties();
    if ((flags & JO_ACCESS_MASK) == 0)
    {
        FillTableProperties();
    }
}

void CJsonConfigDialog::SaveTablePropertiesSet( QTreeWidgetItem* previous )
{
    uint32_t count = uiConf.tablePropertiesWidget->rowCount();
    VectorValues props;
    for (uint32_t i = 0; i < count; ++i)
    {
        SValueView vv = GetPropertyValue( i );
        props.push_back( vv );
    }
    m_pConfig->SaveProperties( GetTreePath( previous ), props );
}

void CJsonConfigDialog::addNewPropertiesSet()
{
    bool isMultiple = false;
    uint32_t flags = GetFlagsTreeNode( GetCurrentTreeNode() );
    uint32_t linkFlag = flags & JO_LINKS;
    QString unit = CJsonConfig::GetUnitNameByFlags( flags, &isMultiple );

    bool ok = false;

    if (!linkFlag)
    {
        // add properties set
        QString text = QInputDialog::getText( this, "Define new " + unit,
                                              unit + tr( " name:" ), QLineEdit::Normal,
                                              "", &ok );
        if (ok && !text.isEmpty())
        {
            // insert new object
            QString path = GetTreePath( GetCurrentTreeNode() );
            path.append( u'.' );
            path.append( text );
            if (!m_pConfig->IsNodeExists( path ))
            {
                QTreeWidgetItem* item = new QTreeWidgetItem( GetCurrentTreeNode(), QStringList { text, QString().asprintf( "%04X", flags & JO_MAIN_MASK ) }, (int)ETypeValue::object_value );
                GetCurrentTreeNode()->addChild( item );
                uiConf.treeObjectsWidget->setCurrentItem( item );
                SetDirty( true );

                const VectorValues& tpl = CJsonConfig::GetUnitTemplateByFlags( flags );
                FillTableProperties( tpl );
                m_pConfig->SaveProperties( path, tpl, true );
            }
        }
    }
    else
    {
        // select link to properties set
        if (isMultiple == false && GetCurrentTreeNode()->childCount() > 0)
        {
            QMessageBox::warning( this, tr( "Configuration editor" ),
                                  "More one link to properties set of " + unit + " is not available",
                                  QMessageBox::Ok );
        }
        else
        {
            const QString sFlags = QString().asprintf( "%04X", flags & JO_MAIN_MASK );
            QList<QTreeWidgetItem*> items = uiConf.treeObjectsWidget->findItems( sFlags, Qt::MatchExactly | Qt::MatchRecursive, TREE_FLAGS_COLUMN );
            if (items.isEmpty())
            {
                QMessageBox::warning( this, tr( "Configuration editor" ),
                                      "No one properties set of " + unit + " was found\nDefine new " + unit + " properties set",
                                      QMessageBox::Ok );
            }
            else
            {
                const uint32_t count = items.size();
                QStringList sList;
                for (const auto& item : items)
                {
                    sList.append( item->text( TREE_VALUE_COLUMN ) );
                }

                QString text = QInputDialog::getItem( this, "Select link to property set", unit + " links", sList, 0, false );
                QString data;
                for (const auto& item : items)
                {
                    if (text == item->text( TREE_VALUE_COLUMN ))
                    {
                        data = GetTreePath( item );
                        break;
                    }
                }

                int childCount = GetCurrentTreeNode()->childCount();
                bool isExists = false;
                for (int i = 0; i < childCount && !isExists; ++i)
                {
                    if (GetCurrentTreeNode()->child( i )->text( TREE_VALUE_COLUMN ) == text)
                    {
                        QMessageBox::warning( this, tr( "Configuration editor" ),
                                              "Item '" + text + "' does selected before",
                                              QMessageBox::Ok );
                        isExists = true;
                    }
                }

                if (!isExists)
                {
                    QTreeWidgetItem* newItem = new QTreeWidgetItem( GetCurrentTreeNode(), QStringList{ text, QString().asprintf( "%04X", flags & ~JO_ACCESS_MASK ) }, (int)ETypeValue::link );
                    newItem->setData( TREE_VALUE_COLUMN, Qt::UserRole, data );
                    GetCurrentTreeNode()->addChild( newItem );
                    SetDirty( true );
                    SValueView vv{ text, data, "LINK" };
                    QString path = GetTreePath( GetCurrentTreeNode() );
                    m_pConfig->SaveValue( path, vv, true );
                }
            }

        }
    }
}


void CJsonConfigDialog::delPropertiesSet()
{
    uint32_t flags = GetFlagsTreeNode( GetCurrentTreeNode() );
    QString parentText = GetCurrentTreeNode()->parent()->text( TREE_VALUE_COLUMN );
    QString path = GetTreePath( GetCurrentTreeNode()->parent() );
    m_pConfig->RemoveNode( path, GetCurrentTreeNode()->text( TREE_VALUE_COLUMN ) );
    QTreeWidgetItem* item = GetCurrentTreeNode();
    GetCurrentTreeNode()->parent()->removeChild( item );
    delete item;
    RestoreTreeCursorPosition( parentText, "", "" );
    SetDirty( true );
}


void CJsonConfigDialog::editPropertiesSet()
{
    QString parentPath = GetTreePath( GetCurrentTreeNode()->parent() );
    QString text = GetCurrentTreeNode()->text( TREE_VALUE_COLUMN );
    uint32_t flags = GetFlagsTreeNode( GetCurrentTreeNode() );

    uint32_t linkFlag = flags & JO_LINKS;
    QString title = (linkFlag) ? tr( "Edit " ) : tr( "Select other " );
    QString unit = CJsonConfig::GetUnitNameByFlags( flags );
    title.append( unit );

    bool ok = false;
    if (!linkFlag)
    {
        QString newText = QInputDialog::getText( this, title,
                                                 unit + tr( " name:" ), QLineEdit::Normal,
                                                 text, &ok );
        if (ok && !newText.isEmpty() && text != newText)
        {
            m_pConfig->RenameKeyName( parentPath, text, newText );
            GetCurrentTreeNode()->setText( TREE_VALUE_COLUMN, newText );
            SetDirty( true );
        }
    }

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

void CJsonConfigDialog::closeEvent( QCloseEvent* event )
{
    if (m_bDirty)
    {
        m_pConfig->RestoreFromBackup();
    }
    event->accept();
}

uint32_t CJsonConfigDialog::GetFlagsTreeNode( QTreeWidgetItem* node )
{
    bool ok = false;
    uint32_t flags = node ? node->text( TREE_FLAGS_COLUMN ).toUInt( &ok, HEX_BASE ) : 0;
    return ok ? flags : JO_UNSPECIFIED;
}

void CJsonConfigDialog::clickOkButton()
{
    m_pConfig->RemoveBackup();
    QDialog::accept();
}

void CJsonConfigDialog::clickCancelButton()
{
    if (m_bDirty)
    {
        m_pConfig->RestoreFromBackup();
    }
    QDialog::reject();
}

void CJsonConfigDialog::clickDiscardButton()
{
    if (m_bDirty)
    {
        m_pConfig->RestoreFromBackup();
        Reset();
    }
}

void CJsonConfigDialog::clickApplyButton()
{
    if (m_bDirty)
    {
        m_pConfig->CreateBackup();
        SetDirty( false );
    }
}

void CJsonConfigDialog::Reset()
{
    // save previous position data
    QString parentText = GetCurrentTreeNode()->parent()->text( TREE_VALUE_COLUMN );
    QString currText = GetCurrentTreeNode()->text( TREE_VALUE_COLUMN );
    QString flags = GetCurrentTreeNode()->text( TREE_FLAGS_COLUMN );

    // reset tree widget
    uiConf.treeObjectsWidget->clear();
    ClearTableProperties();
    SetDirty( false );
    InitDialog();

    // restore previous position in tree widget
    RestoreTreeCursorPosition( parentText, currText, flags );
}

void CJsonConfigDialog::RestoreTreeCursorPosition( const QString& parentText, const QString& currText, const QString& flags )
{
    bool isFound = false;
    QList<QTreeWidgetItem*> items;
    if (!currText.isEmpty())
    {
        items = uiConf.treeObjectsWidget->findItems( currText, Qt::MatchExactly | Qt::MatchFixedString | Qt::MatchRecursive );
        
        if (!items.isEmpty())
        {
            for (auto& item : items)
            {
                if (flags == item->text( TREE_FLAGS_COLUMN ))
                {
                    uiConf.treeObjectsWidget->setCurrentItem( item );
                    isFound = true;
                    break;
                }
            }
        }
    }
    if (!isFound && !parentText.isEmpty())
    {
        items = uiConf.treeObjectsWidget->findItems( parentText, Qt::MatchExactly | Qt::MatchFixedString | Qt::MatchRecursive );
        if (!items.isEmpty()) // assume only one item can be presented
        {
            uiConf.treeObjectsWidget->setCurrentItem( items.constFirst() );
        }
    }
}

void CJsonConfigDialog::ClearTableProperties()
{
    while (uiConf.tablePropertiesWidget->rowCount() > 0)
    {
        uiConf.tablePropertiesWidget->removeRow( 0 );
    }
}


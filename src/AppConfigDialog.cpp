#include "AppConfigDialog.h"
#include "utils/Exceptions.h"
#include <QCloseEvent>
#include <QMenu>
#include <QTableWidget>
#include <QMessageBox>
#include <QAction>
#include <QInputDialog>
#include <QStringList>

//////////////////////////////////////////////////////////////////////////
const int HEX_BASE = 16;
const int TREE_VALUE_COLUMN = 0;
const int TREE_FLAGS_COLUMN = 1;
const int TABLE_NAME_COLUMN = 0;
const int TABLE_VALUE_COLUMN = 1;
const int TABLE_TYPE_COLUMN = 2;

//////////////////////////////////////////////////////////////////////////
CAppConfigDialog::CAppConfigDialog( CAppConfig& config, QWidget* parent) :
    QDialog( parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint ),
    m_config( config ),
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
    QObject::connect( uiConf.buttonBox, SIGNAL( clicked() ), this, SLOT( clickButton() ) );

    InitDialog();
}

CAppConfigDialog::~CAppConfigDialog()
{

}

void CAppConfigDialog::InitDialog()
{
    FillTreeNode();
    uiConf.treeObjectsWidget->expandAll();
    uiConf.treeObjectsWidget->sortByColumn( TREE_FLAGS_COLUMN, Qt::SortOrder::AscendingOrder );
    SetDirty( false );
}

void CAppConfigDialog::FillTreeNode(const QString& parentName, QTreeWidgetItem* parent)
{
    QVector<QTreeWidgetItem*> items;

    ListNodesProperty nodes = m_config.GetListMainNodes( parentName );
    if (nodes.isEmpty())
    {
        auto mask = GetNodeFlags( parent );
        if (mask & JO_APPEND)
        {
            nodes = m_config.GetListPropertyNodes( GetNodePath( parent ), mask );
        }
    }
    
    rng::for_each( nodes, [&]( const auto& node ) -> void
    {
        QTreeWidgetItem* item = new QTreeWidgetItem( parent, QStringList { node.name, QString::asprintf( "%04X", node.mask ) } );
        if (!item)
        {
            tex::ThrowException<std::runtime_error>( fmt::format( FMT_STRING( "Error memory allocation for {:s} QTreeWidgetItem" ), node.name.toStdString() ) );
        }
        item->setData( TREE_VALUE_COLUMN, Qt::UserRole, QVariant::fromValue( node.maxCount ) );
        item->setData( TREE_FLAGS_COLUMN, Qt::UserRole, QVariant::fromValue( node.type ) );
        (parentName.isEmpty())
            ? items.append( item )
            : parent->addChild( item );
        FillTreeNode( node.name, item );
    } );

    if (parentName.isEmpty())
    {
        uiConf.treeObjectsWidget->insertTopLevelItems( TREE_VALUE_COLUMN, items );
        uiConf.treeObjectsWidget->setCurrentItem( items.at( 0 ) );
    }
}

QString CAppConfigDialog::GetNodePath( const QTreeWidgetItem* node ) const
{
    QString result;
    if (node != Q_NULLPTR)
    {
        result = node->text( TREE_VALUE_COLUMN );
        const QTreeWidgetItem* item = node->parent();
        while (item != nullptr)
        {
            result.insert( 0, '.' );
            result.insert( 0, item->text( TREE_VALUE_COLUMN ) );
            item = item->parent();
        }
    }
    return result;
}

quint32 CAppConfigDialog::GetNodeFlags( QTreeWidgetItem* node ) const
{
    bool ok = false;
    uint32_t flags = node ? node->text( TREE_FLAGS_COLUMN ).toUInt( &ok, HEX_BASE ) : 0;
    return ok ? flags : JO_UNSPECIFIED;
}

qint32 CAppConfigDialog::GetNodeChildsMaxCount( QTreeWidgetItem* node ) const
{
    bool ok = false;
    qint32 maxCount = node->data( TREE_VALUE_COLUMN, Qt::UserRole ).toInt( &ok );
    return ok ? maxCount : 0;
}

ETypeValue CAppConfigDialog::GetNodeType( QTreeWidgetItem* node ) const
{
    QVariant qv = node->data( TREE_FLAGS_COLUMN, Qt::UserRole );
    return qv.isValid() ? qvariant_cast<ETypeValue>(qv) : ETypeValue::NullValue;
}

void CAppConfigDialog::closeEvent( QCloseEvent* event )
{
    if (m_bDirty)
    {
        m_config.Undo();
        SetDirty(false);
    }
    event->accept();
}

void CAppConfigDialog::addNewPropertiesSet()
{

}

void CAppConfigDialog::delPropertiesSet()
{

}

void CAppConfigDialog::editPropertiesSet()
{

}

void CAppConfigDialog::addNewProperty()
{

}

void CAppConfigDialog::delProperty()
{

}

void CAppConfigDialog::editProperty()
{

}

void CAppConfigDialog::itemTableChanged( QTableWidgetItem* item )
{

}

void CAppConfigDialog::itemTreeChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{

}

void CAppConfigDialog::clickButton( QAbstractButton* button )
{

}

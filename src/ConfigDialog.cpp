#include "StdInc.h"
#include "ConfigDialog.h"
#include "Config.h"
#include "common.h"
#include "VarEditDialog.h"
#include <QTableWidget>
#include <cassert>

//////////////////////////////////////////////////////////////////////////
CConfigDialog::CConfigDialog(CConfig* config, QWidget* parent) :
    QDialog( parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint ),
    m_config(config)
{
    assert(config != Q_NULLPTR);
    uiConf.setupUi( this );
    setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint );
    setAttribute( Qt::WA_CustomWhatsThis );

    uiConf.appEdit->setText(QString::fromStdString(config->ReadTestAppPath()));
    uiConf.proxyEdit->setText(QString::fromStdString(config->ReadProxyPath()));

    QFont headerFont( "Times", 10, QFont::Bold );

    // environment variables table
    {
        QTableWidgetItem* nameHeaderItem = new QTableWidgetItem( tr( "Name" ) );
        //nameHeaderItem->setIcon( QIcon( QPixmap( ":/Images/cubed.png" ) ) );
        nameHeaderItem->setTextAlignment( Qt::AlignLeft );
        nameHeaderItem->setFont( headerFont );
        uiConf.envTableWidget->setHorizontalHeaderItem( 0, nameHeaderItem );

        QTableWidgetItem* valHeaderItem = new QTableWidgetItem( tr( "Value" ) );
        //valHeaderItem->setIcon( QIcon( QPixmap( ":/Images/cubed.png" ) ) );
        valHeaderItem->setTextAlignment( Qt::AlignLeft );
        valHeaderItem->setFont( headerFont );
        uiConf.envTableWidget->setHorizontalHeaderItem( 1, valHeaderItem );
    }

    // command line parameters table
    {
        QTableWidgetItem* nameHeaderItem = new QTableWidgetItem( tr( "Name" ) );
        //nameHeaderItem->setIcon( QIcon( QPixmap( ":/Images/cubed.png" ) ) );
        nameHeaderItem->setTextAlignment( Qt::AlignLeft );
        nameHeaderItem->setFont( headerFont );
        uiConf.paramTableWidget->setHorizontalHeaderItem( 0, nameHeaderItem );

        QTableWidgetItem* valHeaderItem = new QTableWidgetItem( tr( "Value" ) );
        //valHeaderItem->setIcon( QIcon( QPixmap( ":/Images/cubed.png" ) ) );
        valHeaderItem->setTextAlignment( Qt::AlignLeft );
        valHeaderItem->setFont( headerFont );
        uiConf.paramTableWidget->setHorizontalHeaderItem( 1, valHeaderItem );
    }


    QObject::connect(uiConf.addEnvButton, SIGNAL(clicked()), this, SLOT(addEnironVariable()));
    QObject::connect(uiConf.appSelect, SIGNAL(clicked()), this, SLOT(addTestAppPath()));
    QObject::connect(uiConf.proxySelect, SIGNAL(clicked()), this, SLOT(addProxyPath()));
}

CConfigDialog::~CConfigDialog()
{

}

void CConfigDialog::addTestAppPath()
{
    QString name = util::FindFile(m_config, tr("Find test application executable file"), tr("Executable (*.exe);;All files (*.*)"));
    uiConf.appEdit->setText(name);
    m_config->StoreTestAppPath(name.toStdString());
}

void CConfigDialog::addProxyPath()
{
    QString name = util::FindFile(m_config, tr("Find proxy executable file"), tr("Executable (*.exe);;All files (*.*)"));
    uiConf.proxyEdit->setText(name);
    m_config->StoreProxyPath(name.toStdString());
}

void CConfigDialog::addEnironVariable()
{
    QScopedPointer<CVarEditor> dlg( new CVarEditor( tr("Create new environment variable"), Q_NULLPTR ) );
    if ( dlg )
    {
        if ( dlg->exec() == QDialog::Accepted )
        {
            // save input
        }
    }
}

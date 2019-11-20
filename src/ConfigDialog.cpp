#include "StdInc.h"
#include "ConfigDialog.h"
#include "common.h"
#include "VarEditDialog.h"
#include <QTableWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <boost/filesystem.hpp>
#include <cassert>

//////////////////////////////////////////////////////////////////////////
CConfigDialog::CConfigDialog(QWidget* parent) :
    QDialog( parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint )
{
    uiConf.setupUi( this );
    setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint );
    setAttribute( Qt::WA_CustomWhatsThis );

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
    QString qPath;
    std::string appPath = GetAppName();
    if ( !appPath.empty() )
    {
        boost::filesystem::path path = boost::filesystem::path( appPath ).parent_path();
        qPath = QString::fromStdString( path.string() );
    }
    QString name = util::FindFile( qPath, tr("Find test application executable file"), tr("Executable (*.exe);;All files (*.*)"));
    uiConf.appEdit->setText(name);
}

void CConfigDialog::addProxyPath()
{
    QString qPath;
    std::string appPath = GetProxyName();
    if ( !appPath.empty() )
    {
        boost::filesystem::path path = boost::filesystem::path( appPath ).parent_path();
        qPath = QString::fromStdString( path.string() );
    }
    QString name = util::FindFile( qPath, tr("Find proxy executable file"), tr("Executable (*.exe);;All files (*.*)"));
    uiConf.proxyEdit->setText(name);
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

void CConfigDialog::SetAppName( const std::string& name )
{
    uiConf.appEdit->setText( QString::fromStdString( name ) );
}

void CConfigDialog::SetProxyName( const std::string& name )
{
    uiConf.proxyEdit->setText( QString::fromStdString( name ) );
}

std::string CConfigDialog::GetAppName() const
{
    return uiConf.appEdit->displayText().toStdString();
}

std::string CConfigDialog::GetProxyName() const
{
    return uiConf.proxyEdit->displayText().toStdString();
}


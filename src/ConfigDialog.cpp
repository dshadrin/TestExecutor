#include "StdInc.h"
#include "ConfigDialog.h"
#include "VarEditDialog.h"
#include <QTableWidget>

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


    QObject::connect( uiConf.addEnvButton, SIGNAL( clicked() ), this, SLOT( addEnironVariable() ) );
}

CConfigDialog::~CConfigDialog()
{

}

void CConfigDialog::addEnironVariable()
{
    QScopedPointer<CVarEditor> dlg( new CVarEditor( "Create new environment variable", Q_NULLPTR ) );
    if ( dlg )
    {
        if ( dlg->exec() == QDialog::Accepted )
        {
            // save input
        }
    }
}

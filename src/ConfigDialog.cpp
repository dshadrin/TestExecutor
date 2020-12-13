#include "StdInc.h"
#include "ConfigDialog.h"
#include "common.h"
#include "VarEditDialog.h"
#include <QTableWidget>
#include <QMessageBox>
#include <QCloseEvent>
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


    QObject::connect( uiConf.addEnvButton, SIGNAL( clicked() ), this, SLOT( addEnironVariable() ) );
    QObject::connect( uiConf.delEnvButton, SIGNAL( clicked() ), this, SLOT( delEnironVariable() ) );
    QObject::connect( uiConf.editEnvButton, SIGNAL( clicked() ), this, SLOT( editEnironVariable() ) );
    QObject::connect( uiConf.appSelect, SIGNAL( clicked() ), this, SLOT( addTestAppPath() ) );
    QObject::connect( uiConf.runBeforeSelect, SIGNAL( clicked() ), this, SLOT( addRunBeforePath() ) );
    QObject::connect( uiConf.addParamButton, SIGNAL( clicked() ), this, SLOT( addNewParam() ) );
    QObject::connect( uiConf.delParamButton, SIGNAL( clicked() ), this, SLOT( delParam() ) );
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
        fs::path path = fs::path( appPath ).parent_path();
        qPath = QString::fromStdString( path.string() );
    }
    QString name = util::FindFile( qPath, tr("Find test application executable file"), tr("Executable (*.exe);;All files (*.*)"));
    uiConf.appEdit->setText(name);
}

void CConfigDialog::addRunBeforePath()
{
    QString qPath;
    std::string appPath = GetRunBeforeName();
    if ( !appPath.empty() )
    {
        fs::path path = fs::path( appPath ).parent_path();
        qPath = QString::fromStdString( path.string() );
    }
    QString name = util::FindFile( qPath, tr("Find run before executable file"), tr("Executable (*.exe);;All files (*.*)"));
    uiConf.runBeforeEdit->setText(name);
}


void CConfigDialog::addNewParam()
{
    int row = uiConf.paramTableWidget->rowCount();
    uiConf.paramTableWidget->setRowCount( row + 1 );
}


void CConfigDialog::delParam()
{
    int row = uiConf.paramTableWidget->currentRow();
    if (row >= 0)
    {
        uiConf.paramTableWidget->removeRow( row );
        uiConf.paramTableWidget->sortItems( 0 );
    }
}

void CConfigDialog::addEnironVariable()
{
    QScopedPointer<CVarEditor> dlg( new CVarEditor( tr("Create new environment variable"), Q_NULLPTR ) );
    if ( dlg )
    {
        if ( dlg->exec() == QDialog::Accepted )
        {
            QString name = dlg->uiVarEdit.lineEditName->text();
            QString value = dlg->uiVarEdit.lineEditValue->text();
            int row = uiConf.envTableWidget->rowCount();
            uiConf.envTableWidget->setRowCount( row + 1 );
            QTableWidgetItem* newItem = new QTableWidgetItem( name );
            uiConf.envTableWidget->setItem( row, 0, newItem );
            newItem = new QTableWidgetItem( value );
            uiConf.envTableWidget->setItem( row, 1, newItem );
            uiConf.envTableWidget->sortItems( 0 );
        }
    }
}

void CConfigDialog::delEnironVariable()
{
    int row = uiConf.envTableWidget->currentRow();
    if (row >= 0)
    {
        uiConf.envTableWidget->removeRow( row );
        uiConf.envTableWidget->sortItems( 0 );
    }
}


void CConfigDialog::editEnironVariable()
{
    int row = uiConf.envTableWidget->currentRow();
    QTableWidgetItem* nameItem = uiConf.envTableWidget->item( row, 0 );
    QString name = nameItem->text();
    QTableWidgetItem * valueItem = uiConf.envTableWidget->item( row, 1 );
    QString value = valueItem->text();

    QScopedPointer<CVarEditor> dlg( new CVarEditor( tr( "Create new environment variable" ), Q_NULLPTR ) );
    if (dlg)
    {
        dlg->uiVarEdit.lineEditName->setText( nameItem->text() );
        dlg->uiVarEdit.lineEditValue->setText( valueItem->text() );
        if (dlg->exec() == QDialog::Accepted)
        {
            nameItem->setText( dlg->uiVarEdit.lineEditName->text() );
            valueItem->setText( dlg->uiVarEdit.lineEditValue->text() );
            uiConf.envTableWidget->sortItems( 0 );
        }
    }
}

void CConfigDialog::SetAppName( const std::string& name )
{
    uiConf.appEdit->setText( QString::fromStdString( name ) );
}

void CConfigDialog::SetRunBeforeName( const std::string& name )
{
    uiConf.runBeforeEdit->setText( QString::fromStdString( name ) );
}

std::string CConfigDialog::GetAppName() const
{
    return uiConf.appEdit->displayText().toStdString();
}

std::string CConfigDialog::GetRunBeforeName() const
{
    return uiConf.runBeforeEdit->displayText().toStdString();
}

QList<std::pair<QString, QString>> CConfigDialog::GetEnvironmentValues() const
{
    int rows = uiConf.envTableWidget->rowCount();
    QList<std::pair<QString, QString>> lEnv;
    for (int i = 0; i < rows; ++i)
    {
        QTableWidgetItem* nameItem = uiConf.envTableWidget->item( i, 0 );
        QTableWidgetItem* valueItem = uiConf.envTableWidget->item( i, 1 );
        lEnv.push_back( std::make_pair( nameItem->text(), valueItem->text() ) );
    }
    return lEnv;
}

QTableWidget* CConfigDialog::GetEnvTableWidget()
{
    return uiConf.envTableWidget;
}


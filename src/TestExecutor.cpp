#include "StdInc.h"
#include "TestExecutor.h"
#include "console.h"
#include "ConfigDialog.h"
#include <QSettings>
#include <QtWidgets>

TestExecutor::TestExecutor(QWidget *parent) :
    QMainWindow( parent ),
    m_config(new CConfig(this))
{
    ui.setupUi(this);
    ui.tabWidget->addTab( new Console( this ), "Console" );
    readSettings();
    QObject::connect(ui.actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    QObject::connect( ui.actionOptions, SIGNAL( triggered() ), this, SLOT( OptionsDialog() ) );

    setWindowTitle( tr( "MTF test executor" ) );
    setWindowIcon( QIcon( ":/TestExecutor/common/editor.png" ) );
    setUnifiedTitleAndToolBarOnMac( true );
}

void TestExecutor::closeEvent( QCloseEvent* event )
{
    writeSettings();
    event->accept();
}

void TestExecutor::writeSettings()
{
    m_config->GetSettings().put(KEY_GEOMETRY, saveGeometry().toBase64().toStdString() );
    m_config->WriteJsonConfig();
}

void TestExecutor::readSettings()
{
    const QByteArray geometry = QByteArray::fromBase64(QByteArray::fromStdString(m_config->GetSettings().get<std::string>(KEY_GEOMETRY, "")));
    if ( geometry.isEmpty() )
    {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry( this );
        resize( availableGeometry.width() / 3, availableGeometry.height() / 2 );
        move( ( availableGeometry.width() - width() ) / 2,
            ( availableGeometry.height() - height() ) / 2 );
    }
    else
    {
        restoreGeometry( geometry );
    }
}

void TestExecutor::OptionsDialog()
{
    QScopedPointer<CConfigDialog> dlg(new CConfigDialog());
    if ( dlg )
    {
        m_config->FillOptionsDialog( dlg.get() );
        if ( dlg->exec() == QDialog::Accepted )
        {
            m_config->StoreOptionValues( dlg.get() );
        }
    }
}

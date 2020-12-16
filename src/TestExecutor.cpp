#include "TestExecutor.h"
#include "console.h"
#include "Monitor.h"
#include "Logger.h"
#include "logclient/logclient.h"
#include "ConfigDialog.h"
#include <QSettings>
#include <QtWidgets>

TestExecutor::TestExecutor(QWidget *parent) :
    QMainWindow( parent ),
    m_config(new CConfig(this)),
    m_thread( std::bind( &TestExecutor::ThreadIO, this ) ),
    m_console(nullptr)
{
    ui.setupUi(this);

    auto& logger = m_config->GetSettings().get_child( "logger.client" );
    CLogClient::Get( logger );
    ui.tabWidget->addTab( new CLogger( logger, this ), QString::fromStdString( logger.get<std::string>( "name", "Logger" ) ) );

    m_console = new Console( this );
    ui.tabWidget->addTab( m_console, "Console" );
    QObject::connect( this, &TestExecutor::Run, m_console, &Console::RunCommand );

    auto& monitors = m_config->GetSettings().get_child( "test-monitor" );
    for (auto& it : monitors)
    {
        ui.tabWidget->addTab( new CMonitor( m_ioCtx, it.second, this ), QString::fromStdString( it.second.get<std::string>( "name", it.first) ) );
    }

    readSettings();
    QObject::connect(ui.actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    QObject::connect( ui.actionOptions, SIGNAL( triggered() ), this, SLOT( OptionsDialog() ) );

    setWindowTitle( tr( "MTF test executor" ) );
    setWindowIcon( QIcon( ":/TestExecutor/common/editor.png" ) );
    setUnifiedTitleAndToolBarOnMac( true );

    m_config->SetWorkDirectory();
    emit Run( "test_app -ln" );
}


TestExecutor::~TestExecutor()
{
    m_ioCtx.stop();
    m_thread.join();
    CLogClient::Get()->Stop();
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
        const QRect availableGeometry = screen()->availableGeometry();
        resize( availableGeometry.width() / 3, availableGeometry.height() / 2 );
        move( ( availableGeometry.width() - width() ) / 2,
            ( availableGeometry.height() - height() ) / 2 );
    }
    else
    {
        restoreGeometry( geometry );
    }
}

void TestExecutor::ThreadIO()
{
    boost::asio::io_context::work work( m_ioCtx );
    m_ioCtx.run();
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

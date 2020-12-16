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

    // run logger
    auto& logger = m_config->GetSettings().get_child( "logger.client" );
    try
    {
        CLogClient::Get( logger );
        ui.tabWidget->addTab( new CLogger( logger, this ), QString::fromStdString( logger.get<std::string>( "name", "Logger" ) ) );
    }
    catch (const std::exception& e)
    {
        qDebug() << "Logger is not available: " << e.what();
        throw;
    }

    // run console
    m_console = new Console( this );
    QObject::connect( this, &TestExecutor::Run, m_console, &Console::RunCommand );
    ui.tabWidget->addTab( m_console, "Console" );

    // run monitors
    auto& monitors = m_config->GetSettings().get_child( "test-monitor" );
    for (auto& it : monitors)
    {
        CMonitor* monitor = nullptr;
        try
        {
            monitor = new CMonitor( m_ioCtx, it.second, this );
            if (!monitor->isConnect())
            {
                delete monitor;
                monitor = nullptr;
            }
        }
        catch (const std::exception&)
        {
            if (monitor)
            {
                delete monitor;
                monitor = nullptr;
            }
        }

        if (monitor)
        {
            QObject::connect( this, &TestExecutor::ShutdownMonitors, monitor, &CMonitor::shutdown );
            ui.tabWidget->addTab( monitor, QString::fromStdString( it.second.get<std::string>( "name", it.first ) ) );
        }
    }

    // other settings
    readSettings();
    QObject::connect(ui.actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    QObject::connect( ui.actionOptions, SIGNAL( triggered() ), this, SLOT( OptionsDialog() ) );

    setWindowTitle( tr( "MTF test executor" ) );
    setWindowIcon( QIcon( ":/TestExecutor/common/editor.png" ) );
    setUnifiedTitleAndToolBarOnMac( true );

    // get tests list
    m_config->SetWorkDirectory();
#ifdef WIN32
    emit Run( "test_app -ln" );
#else
    emit Run( "./test_app -ln" );
#endif
}


TestExecutor::~TestExecutor()
{
    emit ShutdownMonitors();
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
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        const QRect availableGeometry = QApplication::desktop()->availableGeometry();
#else
        const QRect availableGeometry = screen()->availableGeometry();
#endif
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
#if (QT_VERSION < QT_VERSION_CHECK(5, 11, 0))
        CConfigDialog* ptr = dlg.data();
#else
        CConfigDialog* ptr = dlg.get();
#endif
        m_config->FillOptionsDialog( ptr );
        if ( dlg->exec() == QDialog::Accepted )
        {
            m_config->StoreOptionValues( ptr );
        }
    }
}

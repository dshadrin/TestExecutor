#include "TestExecutor.h"
#include "Console.h"
#include "QConsole.h"
#include "Monitor.h"
#include "Logger.h"
#include "logclient/LogClient.h"
#include "AppConfigDialog.h"
#include <QSettings>
#include <QtWidgets>

IMPLEMENT_MODULE_TAG( TestExecutor, "EXEC" );
extern std::string g_jsonConfigName;

TestExecutor::TestExecutor(QWidget *parent) :
    QMainWindow( parent ),
    m_config( g_jsonConfigName ),
    m_thread( std::bind( &TestExecutor::ThreadIO, this ) ),
    m_console(nullptr)
{
    ui.setupUi(this);
/*
    // run logger
    try
    {
        CValueViewAdapter appLoggerLink = m_config->GetProperties( KEY_CURRENT_SESSION "." KEY_SELECTED_LOGGER );
        if (appLoggerLink.size() == 1)
        {
            CValueViewAdapter appLoggerProps = m_config->GetProperties( appLoggerLink.get<QString>( "default logger" ) );

            boost::property_tree::ptree pt;
            pt.put<std::string>( "host", appLoggerProps.get<std::string>( "host" ) );
            pt.put<std::string>( "name", appLoggerProps.get<std::string>( "name" ) );
            pt.put<std::string>( "severity", appLoggerProps.get<std::string>( "severity" ) );
            pt.put<int>( "port", appLoggerProps.get<int>( "port" ) );
            pt.put<int>( "retry", appLoggerProps.get<int>( "retry" ) );
            pt.put<int>( "max_message_size", appLoggerProps.get<int>( "max_message_size" ) );
            pt.put<int>( "module_tag_size", appLoggerProps.get<int>( "module_tag_size" ) );

            CLogClient::Get( pt ); // start logger
            ui.tabWidget->addTab( new CLogger( appLoggerProps, this ), appLoggerProps.get<QString>( "name" ) );
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "Logger is not available: " << e.what();
        throw;
    }

    LOG_INFO << "Run console";
    // run console
    CValueViewAdapter appPropsLink = m_config->GetProperties( KEY_CURRENT_SESSION "." KEY_SELECTED_SESSION );
    if (appPropsLink.size() == 1)
    {
        QString sessionName = appPropsLink.GetName( 0 );
        if (!sessionName.isNull() && !sessionName.isEmpty())
        {
            CValueViewAdapter appProps = m_config->GetProperties( appPropsLink.get<QString>( sessionName ) );
            m_console = new Console( appProps.GetValues(), this );
            QObject::connect( this, &TestExecutor::Run, m_console, &Console::RunCommand );
            ui.tabWidget->addTab( m_console, sessionName );
        }
    }

    // run qconsole
    //QConsole* qConsole = new QConsole( this, "#>" );
    //QObject::connect( this, &TestExecutor::Run, m_console, &Console::RunCommand );
    //ui.tabWidget->addTab( qConsole, "QConsole" );

    // run monitors
//     auto& monitors = m_config->GetSettings().get_child( "test-monitor" );
//     for (auto& it : monitors)
//     {
//         CMonitor* monitor = nullptr;
//         try
//         {
//             monitor = new CMonitor( m_ioCtx, it.second, this );
//             if (!monitor->isConnect())
//             {
//                 delete monitor;
//                 monitor = nullptr;
//             }
//         }
//         catch (const std::exception&)
//         {
//             if (monitor)
//             {
//                 delete monitor;
//                 monitor = nullptr;
//             }
//         }
// 
//         if (monitor)
//         {
//             QObject::connect( this, &TestExecutor::ShutdownMonitors, monitor, &CMonitor::shutdown );
//             ui.tabWidget->addTab( monitor, QString::fromStdString( it.second.get<std::string>( "name", it.first ) ) );
//         }
//     }
*/
    // other settings
    readSettings();
    QObject::connect(ui.actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    QObject::connect( ui.actionOptions, SIGNAL( triggered() ), this, SLOT( OptionsDialog() ) );

//    emit Run( "-ln" );

}


TestExecutor::~TestExecutor()
{
    emit ShutdownMonitors();
    m_ioCtx.stop();
    m_thread.join();
//     CLogClient::Get()->Stop();
}

void TestExecutor::closeEvent( QCloseEvent* event )
{
    writeSettings();
    event->accept();
}

void TestExecutor::writeSettings()
{
    m_config.SaveGeometry( saveGeometry().toBase64() );
}

void TestExecutor::readSettings()
{
    const QByteArray geometry = QByteArray::fromBase64( m_config.GetGeometry() );
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
    QScopedPointer<CAppConfigDialog> dlg( new CAppConfigDialog( m_config ) );
    if ( dlg )
    {
        QDialog::DialogCode code = (QDialog::DialogCode)dlg->exec();
        if ( code == QDialog::Accepted )
        {
        }
        else
        {
        }
    }
}

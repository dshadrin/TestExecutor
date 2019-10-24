#include "TestExecutor.h"
#include "console.h"
#include <QSettings>
#include <QtWidgets>

TestExecutor::TestExecutor(QWidget *parent) :
    QMainWindow(parent),
    m_config(this)
{
    ui.setupUi(this);
    ui.tabWidget->addTab( new Console( this ), "Console" );
    readSettings();
    QObject::connect( ui.actionAbout_Qt, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );
}

void TestExecutor::closeEvent( QCloseEvent* event )
{
    writeSettings();
    event->accept();
}

void TestExecutor::writeSettings()
{
    m_config.GetSettings().setValue( "geometry", saveGeometry() );
}

void TestExecutor::readSettings()
{
    const QByteArray geometry = m_config.GetSettings().value( "geometry", QByteArray() ).toByteArray();
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

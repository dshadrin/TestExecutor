#include "TestExecutor.h"
#include <QSettings>
#include <QtWidgets>

extern QString confName;

TestExecutor::TestExecutor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
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
    QSettings settings( confName, QSettings::IniFormat, Q_NULLPTR );
    settings.setValue( "geometry", saveGeometry() );
}

void TestExecutor::readSettings()
{
    QSettings settings( confName, QSettings::IniFormat, Q_NULLPTR );
    const QByteArray geometry = settings.value( "geometry", QByteArray() ).toByteArray();
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

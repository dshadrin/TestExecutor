#include "StdInc.h"
#include "TestExecutor.h"
#include <QApplication>
#include <QSettings>
#include <QMessageBox>

// configuration file path
std::string g_configName;

int main(int argc, char *argv[])
{
    int result = -1;

    try
    {
        fs::path confPath( fs::current_path() / fs::path(argv[0]).filename().replace_extension( ".json" ) );
        g_configName = confPath.string();

        QApplication a(argc, argv);
        TestExecutor w;
        w.show();
        result = a.exec();
    }
    catch ( const std::exception & e )
    {
        QMessageBox::critical( Q_NULLPTR, QString( "Critical error" ), QString::fromStdString( e.what() ) );
        result = -1;
    }

    return result;
}

#include "Config.h"
#include "ConfigDialog.h"
#include "boost/property_tree/json_parser.hpp"

//////////////////////////////////////////////////////////////////////////
namespace bpt = boost::property_tree;

//////////////////////////////////////////////////////////////////////////
extern std::string g_configName;
#define KEY_LOCALE "locale"
IMPLEMENT_MODULE_TAG( CConfig, "CONF" );

//////////////////////////////////////////////////////////////////////////
CConfig::CConfig( QObject* parent ) :
    QObject(parent),
    m_jsonConfigPath(g_configName)
{
    if (fs::exists(m_jsonConfigPath))
    {
        bpt::read_json(m_jsonConfigPath, m_pt, std::locale() );
    }
    else
    {
        bpt::ptree p;
        p.put<std::string>(KEY_PATH, "");
        m_pt.add_child(KEY_TEST_APP, p);
        WriteJsonConfig();
    }
}

CConfig::~CConfig()
{

}

void CConfig::WriteJsonConfig()
{
    bpt::write_json(m_jsonConfigPath, m_pt, std::locale(), true);
}

void CConfig::StoreTestAppPath(const std::string& path)
{
    m_pt.put<std::string>(KEY_TEST_APP_PATH, path);
    WriteJsonConfig();
    SetWorkDirectory();
}

std::string CConfig::ReadTestAppPath()
{
    return m_pt.get<std::string>(KEY_TEST_APP_PATH, "");
}

void CConfig::StoreRunBeforePath(const std::string& path)
{
    m_pt.put<std::string>(KEY_RUN_BEFORE_PATH, path);
    WriteJsonConfig();
}

std::string CConfig::ReadRunBeforePath()
{
    return m_pt.get<std::string>(KEY_RUN_BEFORE_PATH, "");
}

void CConfig::FillOptionsDialog( CConfigDialog* dlg )
{
    std::string testAppPath = m_pt.get<std::string>( KEY_TEST_APP_PATH, "" );
    LOG_INFO << "Get test application path" << (testAppPath.empty() ? " empty" : ": " + testAppPath);
    dlg->SetAppName( testAppPath );
    
    std::string runBeforeAppPath = m_pt.get<std::string>( KEY_RUN_BEFORE_PATH, "" );
    LOG_INFO << "Get run before tests application path" << (runBeforeAppPath.empty() ? " empty" : ": " + runBeforeAppPath);
    dlg->SetRunBeforeName( runBeforeAppPath );

    boost::optional<boost::property_tree::ptree&> pt = m_pt.get_child_optional( KEY_ENV_PATH );
    if (pt.has_value() && !pt.get().empty())
    {
        LOG_INFO << "Get environment:";
        QTableWidget* table = dlg->GetEnvTableWidget();
        for (const auto& it : pt.get())
        {
            const int row = table->rowCount();
            table->setRowCount( row + 1 );
            const std::string eName = it.first;
            const std::string eValue = it.second.get_value<std::string>( "" );
            QTableWidgetItem* nameItem = new QTableWidgetItem( QString::fromStdString( eName ) );
            QTableWidgetItem* valueItem = new QTableWidgetItem( QString::fromStdString( eValue ) );
            table->setItem( row, 0, nameItem );
            table->setItem( row, 1, valueItem );
            LOG_INFO << "    " << eName << "=" << eValue;
        }
        table->sortItems( 0 );
    }
    else
    {
        LOG_INFO << "Get environment empty list";
    }
}

void CConfig::StoreOptionValues( CConfigDialog* dlg )
{
    std::string testAppPath = dlg->GetAppName();
    LOG_INFO << "Store test application path" << (testAppPath.empty() ? " empty" : ": " + testAppPath);
    m_pt.put( KEY_TEST_APP_PATH, testAppPath );

    std::string runBeforeAppPath = dlg->GetRunBeforeName();
    LOG_INFO << "Store run before tests application path" << (runBeforeAppPath.empty() ? " empty" : ": " + runBeforeAppPath);
    m_pt.put( KEY_RUN_BEFORE_PATH, runBeforeAppPath );

    QList<std::pair<QString, QString>> lEnv = dlg->GetEnvironmentValues();
    boost::property_tree::ptree pt;
    if (!lEnv.empty())
    {
        LOG_INFO << "Store environment:";
        for (auto& it : lEnv)
        {
            const std::string eName = it.first.toStdString();
            const std::string eValue = it.second.toStdString();
            pt.put<std::string>( eName, eValue );
            LOG_INFO << "    " << eName << "=" << eValue;
        }
    }
    else
    {
        LOG_INFO << "Store environment empty list";
    }
    m_pt.put_child( KEY_ENV_PATH, pt );
    WriteJsonConfig();
}

void CConfig::SetWorkDirectory()
{
    auto appPath = ReadTestAppPath();
    if (!appPath.empty() && fs::exists(appPath))
    {
        fs::path p = fs::path(appPath).parent_path();
        fs::current_path( p );
        LOG_INFO << "Set current directory: " << p;
    }
    else
    {
        LOG_WARN << "Test application path is empty or not existed: " << appPath;
    }
}

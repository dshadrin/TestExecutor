#include "StdInc.h"
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

void CConfig::StoreProxyPath(const std::string& path)
{
    m_pt.put<std::string>(KEY_PROXY_PATH, path);
    WriteJsonConfig();
}

std::string CConfig::ReadProxyPath()
{
    return m_pt.get<std::string>(KEY_PROXY_PATH, "");
}

void CConfig::FillOptionsDialog( CConfigDialog* dlg )
{
    dlg->SetAppName( m_pt.get<std::string>( KEY_TEST_APP_PATH, "" ) );
    dlg->SetProxyName( m_pt.get<std::string>( KEY_PROXY_PATH, "" ) );

}

void CConfig::StoreOptionValues( CConfigDialog* dlg )
{
    m_pt.put( KEY_TEST_APP_PATH, dlg->GetAppName() );
    m_pt.put( KEY_PROXY_PATH, dlg->GetProxyName() );

    WriteJsonConfig();
}

void CConfig::SetWorkDirectory()
{
    auto appPath = ReadTestAppPath();
    if (!appPath.empty())
    {
        fs::path p = fs::path(appPath).parent_path();
        fs::current_path( p );
        LOG_INFO << "Set current directory: " << p;
    }
    else
    {
        LOG_WARN << "Test application path is empty";
    }
}

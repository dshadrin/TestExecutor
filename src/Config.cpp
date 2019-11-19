#include "StdInc.h"
#include "Config.h"
#include "boost/property_tree/json_parser.hpp"

//////////////////////////////////////////////////////////////////////////
namespace bpt = boost::property_tree;

//////////////////////////////////////////////////////////////////////////
extern std::string g_configName;
#define KEY_LOCALE "locale"

//////////////////////////////////////////////////////////////////////////
CConfig::CConfig( QObject* parent ) :
    QObject(parent),
    m_jsonConfigPath(g_configName)
{
    if (fs::exists(m_jsonConfigPath))
    {
        bpt::read_json(m_jsonConfigPath, m_pt, GetLocale() );
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

std::locale CConfig::GetLocale()
{
    if ( m_localeString.empty() )
    {
        std::string locStr = m_pt.get<std::string>(KEY_LOCALE, "");
        if ( locStr.empty() )
        {
            locStr = "ru_RU";
        }
        m_localeString = locStr;
    }
    return std::locale( m_localeString );
}

void CConfig::WriteJsonConfig()
{
    bpt::write_json(m_jsonConfigPath, m_pt, GetLocale(), true);
}

void CConfig::StoreTestAppPath(const std::string& path)
{
    m_pt.put<std::string>(KEY_TEST_APP_PATH, path);
    WriteJsonConfig();
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

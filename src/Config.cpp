#include "StdInc.h"
#include "Config.h"
#include "boost/property_tree/json_parser.hpp"

//////////////////////////////////////////////////////////////////////////
namespace bpt = boost::property_tree;

//////////////////////////////////////////////////////////////////////////
extern QString g_QConfigName;

//////////////////////////////////////////////////////////////////////////
CConfig::CConfig(QWidget* parent) :
    m_settings(g_QConfigName, QSettings::IniFormat, Q_NULLPTR),
    m_jsonConfigPath((fs::current_path() / fs::path(g_QConfigName.toStdString()).filename().replace_extension(".json")).string())
{
    if (fs::exists(m_jsonConfigPath))
    {
        bpt::read_json(m_jsonConfigPath, m_pt, std::locale("ru_RU"));
    }
    else
    {
        bpt::ptree p;
        p.put<std::string>(KEY_PATH, "");
        m_pt.add_child(KEY_TEST_APP, p);
        WriteJsonConfig();
    }
}

void CConfig::WriteJsonConfig() const
{
    bpt::write_json(m_jsonConfigPath, m_pt, std::locale("ru_RU"), true);
}

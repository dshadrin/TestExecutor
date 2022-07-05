#include "StdInc.h"
#include "JsonConfig.h"
#include <algorithm>
#include <fstream>
#include <boost/algorithm/string.hpp>


//////////////////////////////////////////////////////////////////////////
// Application config structure (example)
//////////////////////////////////////////////////////////////////////////

const size_t TI_NAME = 0;
const size_t TI_MAIN_OBJ_FLAG = 1;
const size_t TI_LINK_OBJ_FLAG = 2;

//const std::tuple<std::string, uint32_t> CJsonConfig::s_rootConfigNode
//{
//    "Configuration", JO_CONST
//};
//
//const std::vector<std::tuple<std::string, uint32_t>> CJsonConfig::s_vMainConfObjects
//{
//    {KEY_CURRENT_SESSION,         JO_CONST  | JO_CURRENT_SESSION},
//    {KEY_DEFINED_SESSIONS,        JO_APPEND | JO_SESSIONS       },
//    {KEY_DEFINED_MONITORS,        JO_APPEND | JO_MONITORS       },
//    {KEY_DEFINED_LOGGERS,         JO_APPEND | JO_LOGGERS        },
//    {KEY_DEFINED_CONNECTIONS,     JO_APPEND | JO_CONNECTIONS    }
//};
//
//const std::vector<std::tuple<std::string, ETypeValue, uint32_t>> CJsonConfig::s_vCurrentSessionConfObjects
//{
//    {KEY_SELECTED_SESSION,    ETypeValue::arraj_value, JO_APPEND | JO_SESSIONS    | JO_LINKS   },
//    {KEY_SELECTED_MONITORS,   ETypeValue::arraj_value, JO_APPEND | JO_MONITORS    | JO_LINKS   },
//    {KEY_SELECTED_LOGGER,     ETypeValue::arraj_value, JO_APPEND | JO_LOGGERS     | JO_LINKS   },
//    {KEY_SELECTED_CONNECTION, ETypeValue::arraj_value, JO_APPEND | JO_CONNECTIONS | JO_LINKS}
//};
//
//const VectorValues g_loggerTemplate
//{
//    { "name",                "TestExecutor log", "STRING"  },
//    { "host",                "localhost",        "STRING"  },
//    { "port",                "2100",             "INTEGER" },
//    { "retry",               "5",                "INTEGER" },
//    { "max_message_size",    "2048",             "INTEGER" },
//    { "module_tag_size",     "4",                "INTEGER" },
//    { "severity",            "DEBUG",            "STRING"  },
//    { "bg-color",            "lightGray",        "STRING"  },
//    { "text-color",          "darkBlue",         "STRING"  },
//    { "font-name",           "Courier New",      "STRING"  },
//    { "font-weight",         "10",               "INTEGER" }
//};
//
//const VectorValues g_monitorTemplate
//{
//    { "name",                "RTOS camera monitor", "STRING"  },
//    { "connection-string",   "localhost:2002",      "STRING"  },
//    { "font-name",           "Lucida Console",      "STRING"  },
//    { "font-weight",         "9",                   "INTEGER" },
//    { "bg-color",            "lightGray",           "STRING"  },
//    { "cmd-text-color",      "darkBlue",            "STRING"  },
//    { "camera-text-color",   "darkBlue",            "STRING"  }
//};
//
//const VectorValues g_connectionTemplate
//{
//    { "name",                "Local environment execution", "STRING"  },
//    { "is-remote-location",  "false",                       "BOOLEAN" },
//    { "host",                "",                            "STRING"  },
//    { "port",                "22",                          "INTEGER" },
//    { "password",            "",                            "STRING"  },
//    { "private-key-file",    "",                            "STRING"  }
//};
//
//const VectorValues g_sessionTemplate
//{
//    { "execution-file",      "", "STRING" },
//    { "command-line",        "", "STRING" },
//    { "environment",         "[]", "ARRAY"}
//};
//
//const VectorValues g_emptyTemplate {};
//
//const size_t TI_FLAG = 0;
//const size_t TI_UNIT_NAME = 1;
//const size_t TI_MULTIPLE = 2;
//const size_t TI_TEMPLATE = 3;
//
//const std::vector<std::tuple<uint32_t, std::string, bool, const VectorValues&>> CJsonConfig::s_vFlagToUnit
//{
//    {JO_SESSIONS,    "Session",     false, g_sessionTemplate   },
//    {JO_MONITORS,    "Monitor",     true,  g_monitorTemplate   },
//    {JO_LOGGERS,     "Logger",      false, g_loggerTemplate    },
//    {JO_CONNECTIONS, "Connection",  false, g_connectionTemplate}
//};
//
//const std::vector<SValueType> g_mapStrToKind
//{
//    {"BOOLEAN",     Json::ValueType::booleanValue, ETypeValue::boolean_value},
//    {"INTEGER",     Json::ValueType::intValue,     ETypeValue::integer_number},
//    {"UNSIGNED",    Json::ValueType::uintValue,    ETypeValue::unsigned_number},
//    {"DOUBLE",      Json::ValueType::realValue,    ETypeValue::float_number},
//    {"STRING",      Json::ValueType::stringValue,  ETypeValue::string_value},
//    {"HEXSTRING",   Json::ValueType::stringValue,  ETypeValue::hex_number},
//    {"ARRAY",       Json::ValueType::arrayValue,   ETypeValue::arraj_value},
//    {"OBJECT",      Json::ValueType::objectValue,  ETypeValue::object_value},
//    {"LINK",        Json::ValueType::stringValue,  ETypeValue::link}
//};

namespace
{
const Json::Value nullValue;
}

//////////////////////////////////////////////////////////////////////////
CJsonConfig::CJsonConfig( const std::string& congName ) :
    m_fileName( congName )
{
    Init();
}

CJsonConfig::~CJsonConfig()
{

}

std::vector<std::string> CJsonConfig::SplitPath( const std::string& path )
{
    std::string p = boost::algorithm::trim_copy( path );
    std::vector<std::string> v;
    boost::algorithm::split( v, p, boost::algorithm::is_any_of( "." ), boost::algorithm::token_compress_on );
    // trim every name
    for (auto& s : v)
    {
        boost::algorithm::trim( s );
    }
    // remove empty
    std::vector<std::string>::iterator it = std::remove( v.begin(), v.end(), "" );
    if (it != v.end())
    {
        v.erase( it, v.end() );
    }
    return std::move(v);
}

std::vector<std::string> CJsonConfig::SplitPathWithCheckEmpty( const std::string& path )
{
    std::vector<std::string> v = SplitPath( path );
    if (v.size() == 0)
    {
        BOOST_THROW_EXCEPTION( std::logic_error( "Wrong json path" ) );
    }
    return std::move( v );
}

void CJsonConfig::Init()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (fs::exists( m_fileName ))
    {
        std::ifstream cfgfile( m_fileName );
        Json::String errs;
        bool ok = ReadFromStream(cfgfile, m_storage, errs);
        cfgfile.close();
        if (!ok)
        {
            BOOST_THROW_EXCEPTION(std::logic_error("Parse json error: " + errs));
        }
    }
}

bool CJsonConfig::ReadFromStream(std::istream& is, Json::Value& jValue, Json::String& errs)
{
    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    return parseFromStream(builder, is, &jValue, &errs);
}

void CJsonConfig::Flush()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    std::ofstream ofs( m_fileName, std::fstream::trunc);
    WriteToStream(ofs, m_storage);
    ofs.close();
}

void CJsonConfig::WriteToStream(std::ostream& os, const Json::Value& jValue)
{
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    builder["commentStyle"] = "All";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    writer->write( jValue, &os );
}

Json::Value& CJsonConfig::FindJsonValue( const std::string& path )
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    std::vector<std::string> p = SplitPathWithCheckEmpty( path );
    return ValueReference(m_storage, p, 0);
}

Json::Value& CJsonConfig::ValueReference(Json::Value& parent, const std::vector<std::string>& p, size_t idx)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (idx < p.size())
    {
        Json::Value& val = parent[p[idx++]];
        return (idx < p.size() ? ValueReference(val, p, idx) : val);
    }
    BOOST_THROW_EXCEPTION(std::out_of_range("Out of range: " + std::to_string(idx)));
}

const Json::Value& CJsonConfig::ValueReference(const Json::Value& parent, const std::vector<std::string>& p, size_t idx) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (idx < p.size())
    {
        if (parent.isMember(p[idx]))
        {
            const Json::Value& val = parent[p[idx++]];
            return (idx < p.size() ? ValueReference(val, p, idx) : val);
        }
        else
        {
            BOOST_THROW_EXCEPTION(std::logic_error("Member does not found: " + p[idx]));
        }
    }
    BOOST_THROW_EXCEPTION(std::out_of_range("Out of range: " + std::to_string(idx)));
}

const Json::Value& CJsonConfig::FindJsonValue( const std::string& path ) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    std::vector<std::string> p = SplitPathWithCheckEmpty( path );
    return ValueReference(m_storage, p, 0);
}

bool CJsonConfig::IsUndoAvailable() const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return !m_undo.empty();
}

void CJsonConfig::SaveCurrentPropertySet()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_undo.push(m_storage);
}

void CJsonConfig::Undo()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_storage = m_undo.top();
    m_undo.pop();
    Flush();
}

void CJsonConfig::ClearUndo()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    std::stack<Json::Value> emptyStack;
    m_undo.swap(emptyStack);
}

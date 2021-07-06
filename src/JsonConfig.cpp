#include "JsonConfig.h"
#include "JsonConfigDialog.h"
#include <QMessageBox>
#include <fstream>

//////////////////////////////////////////////////////////////////////////
namespace bj = boost::json;

//////////////////////////////////////////////////////////////////////////
extern std::string g_jsonConfigName;
#define KEY_LOCALE "locale"
IMPLEMENT_MODULE_TAG( CJsonConfig, "CONF" );

const std::vector<std::string> CJsonConfig::s_vMainConfObjects
{
    "CurrentSession",
    "Sessions",
    "Monitors",
    "Logger",
    "Connections"
};

//////////////////////////////////////////////////////////////////////////
CJsonConfig::CJsonConfig( QObject* parent ) :
    QObject(parent),
    m_jsonConfigPath( g_jsonConfigName ),
    m_jMain( bj::object_kind )
{
    if (fs::exists(m_jsonConfigPath))
    {
        size_t jFileSize = fs::file_size( m_jsonConfigPath );
        if (jFileSize > 0)
        {
            std::ifstream ifs( m_jsonConfigPath );
            if (ifs.good())
            {
                // read file and remove extra data
                std::string jStr;
                std::string line;
                while (!ifs.eof())
                {
                    std::getline( ifs, line );
                    jStr.append( ba::trim_copy( line ) );
                }
                ifs.close();

                // parse json
                bj::parse_options opt;                              // all extensions default to off
                opt.allow_comments = true;                          // permit C and C++ style comments to appear in whitespace
                opt.allow_trailing_commas = true;                   // allow an additional trailing comma in object and array element lists
                opt.allow_invalid_utf8 = true;                      // skip utf-8 validation of keys and strings

                bj::stream_parser p( {}, opt );
                bj::error_code ec;
                p.write( jStr.c_str(), jStr.length(), ec );
                if (!ec)
                {
                    p.finish( ec );
                    if (!ec)
                    {
                        m_jMain = p.release();
                    }
                }
                if (ec)
                {
                    QMessageBox::critical( Q_NULLPTR, QString( "Parsing failed" ), QString::fromStdString( ec.message() ) );
                }
            }
        }
    }

    if (GetSettings().empty())
    {
        for (const std::string& name : s_vMainConfObjects)
        {
            GetSettings()[name] = bj::object();
        }
        
        WriteJsonConfig();
    }
}

CJsonConfig::~CJsonConfig()
{

}

void CJsonConfig::WriteJsonConfig()
{
    std::ofstream ofs( m_jsonConfigPath );
    if (ofs.good())
    {
        PrettyPrint( ofs, m_jMain );
        ofs.close();
    }
}

void CJsonConfig::SaveGeometry( const std::string& value )
{
    GetSettings()[KEY_GEOMETRY] = value;
    WriteJsonConfig();
}

std::string CJsonConfig::GetGeometry()
{
    if (GetSettings()[KEY_GEOMETRY].is_null())
    {
        return "";
    }
    return GetSettings()[KEY_GEOMETRY].as_string().c_str();
}

void CJsonConfig::StoreOptionValues( CJsonConfigDialog* dlg )
{
//     std::string testAppPath = dlg->GetAppName();
//     LOG_INFO << "Store test application path" << (testAppPath.empty() ? " empty" : ": " + testAppPath);
//     m_pt.put( KEY_TEST_APP_PATH, testAppPath );
// 
//     std::string runBeforeAppPath = dlg->GetRunBeforeName();
//     LOG_INFO << "Store run before tests application path" << (runBeforeAppPath.empty() ? " empty" : ": " + runBeforeAppPath);
//     m_pt.put( KEY_RUN_BEFORE_PATH, runBeforeAppPath );
// 
//     QList<std::pair<QString, QString>> lEnv = dlg->GetEnvironmentValues();
//     boost::property_tree::ptree pt;
//     if (!lEnv.empty())
//     {
//         LOG_INFO << "Store environment:";
//         for (auto& it : lEnv)
//         {
//             const std::string eName = it.first.toStdString();
//             const std::string eValue = it.second.toStdString();
//             pt.put<std::string>( eName, eValue );
//             LOG_INFO << "    " << eName << "=" << eValue;
//         }
//     }
//     else
//     {
//         LOG_INFO << "Store environment empty list";
//     }
//     m_pt.put_child( KEY_ENV_PATH, pt );
//     WriteJsonConfig();
}

void CJsonConfig::SetWorkDirectory()
{
//     auto appPath = ReadTestAppPath();
//     if (!appPath.empty() && fs::exists(appPath))
//     {
//         fs::path p = fs::path(appPath).parent_path();
//         fs::current_path( p );
//         LOG_INFO << "Set current directory: " << p;
//     }
//     else
//     {
//         LOG_WARN << "Test application path is empty or not existed: " << appPath;
//     }
}

void CJsonConfig::PrettyPrint( std::ostream& os, boost::json::value const& jv, size_t indentSize, std::string* indent )
{
    std::string indent_;
    if (!indent) indent = &indent_;

    switch (jv.kind())
    {
    case bj::kind::object:
    {
        os << "{\n";
        indent->append( indentSize, ' ' );
        auto const& obj = jv.get_object();
        if (!obj.empty())
        {
            auto it = obj.begin();
            for (;;)
            {
                os << *indent << bj::serialize( it->key() ) << " : ";
                PrettyPrint( os, it->value(), indentSize, indent );
                if (++it == obj.end())
                    break;
                os << ",\n";
            }
            os << "\n";
        }
        indent->resize( indent->size() - indentSize );
        os << *indent << "}";
        break;
    }

    case bj::kind::array:
    {
        os << "[\n";
        indent->append( indentSize, ' ' );
        auto const& arr = jv.get_array();
        if (!arr.empty())
        {
            auto it = arr.begin();
            for (;;)
            {
                os << *indent;
                PrettyPrint( os, *it, indentSize, indent );
                if (++it == arr.end())
                    break;
                os << ",\n";
            }
        }
        os << "\n";
        indent->resize( indent->size() - indentSize );
        os << *indent << "]";
        break;
    }

    case bj::kind::string:
    {
        os << bj::serialize( jv.get_string() );
        break;
    }

    case bj::kind::uint64:
        os << jv.get_uint64();
        break;

    case bj::kind::int64:
        os << jv.get_int64();
        break;

    case bj::kind::double_:
        os << jv.get_double();
        break;

    case bj::kind::bool_:
        if (jv.get_bool())
            os << "true";
        else
            os << "false";
        break;

    case bj::kind::null:
        os << "null";
        break;
    }

    if (indent->empty())
        os << "\n";
}

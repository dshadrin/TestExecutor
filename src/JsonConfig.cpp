#include "JsonConfig.h"
#include "JsonConfigDialog.h"
#include <QMessageBox>
#include <tuple>

//////////////////////////////////////////////////////////////////////////
extern std::string g_jsonConfigName;
IMPLEMENT_MODULE_TAG( CJsonConfig, "CONF" );

//////////////////////////////////////////////////////////////////////////
// Application config structure (example)
//////////////////////////////////////////////////////////////////////////
/*
{
    "CurrentSession": {
        "Flags": "0000",
        "Session": {
            "name": "ISWC test",
            "type": "LINK",
            "value": "Sessions.Iswc-session"
        },
        "Monitors": {
            "Flags": "0000",
            "Links": [
                {
                    "name": "Left camera monitor",
                    "type": "LINK",
                    "value": "Monitors.RtosDefault"
                },
                {
                    "name": "Right camera monitor",
                    "type": "LINK",
                    "value": "Monitors.RtosRight"
                }
            ]
        },
        "Logger": {
            "Flags": "0000",
            "name": "Default logger",
            "type": "LINK",
            "value": "Loggers.Default-oscar"
        },
        "Connection": {
            "Flags": "0000",
            "name": "Local",
            "type": "LINK",
            "value": "Connections.Local"
        }
    },
    "Sessions" : {
        "Flags": "0000",
        "Iswc-session": [
            {
                "name": "prop1",
                "type": "STRING",
                "value": "val1"
            },
            ...
        ],
        "Crwc-session": [
            {
                "name": "prop1",
                "type": "STRING",
                "value": "val1"
            },
            ...
        ]
    },
    "Monitors": {
        "Flags": "0000",
        "RtosDefault": [
            {
                "name": "prop1",
                "type": "STRING",
                "value": "val1"
            },
            ...
        ],
        "RtosRight": [
            {
                "name": "prop1",
                "type": "STRING",
                "value": "val1"
            },
            ...
        ],
        "LinuxDefault": [
            {
                "name": "prop1",
                "type": "STRING",
                "value": "val1"
            },
            ...
        ]
    },
    "Loggers": {
        "Flags": "0000",
        "Default-oscar": [
            {
                "name": "prop1",
                "type": "STRING",
                "value": "val1"
            },
            ...
        ]
    },
    "Connections": {
        "Flags": "0000",
        "Local": [
            {
                "name": "prop1",
                "type": "STRING",
                "value": "val1"
            },
            ...
        ],
        "Remote-wsl": [
            {
                "name": "prop1",
                "type": "STRING",
                "value": "val1"
            },
            ...
        ],
        "Remote-linux": [
            {
                "name": "prop1",
                "type": "STRING",
                "value": "val1"
            },
            ...
        ]
    }
}
*/

const size_t TI_NAME = 0;
const size_t TI_MAIN_OBJ_FLAG = 1;
const size_t TI_LINK_OBJ_FLAG = 2;

const std::tuple<std::string, uint32_t> CJsonConfig::s_rootConfigNode
{
    "Configuration", JO_CONST
};

const std::vector<std::tuple<std::string, uint32_t>> CJsonConfig::s_vMainConfObjects
{
    {"CurrentSession",          JO_CONST  | JO_CURRENT_SESSION},
    {"Defined Sessions",        JO_APPEND | JO_SESSIONS       },
    {"Defined Monitors",        JO_APPEND | JO_MONITORS       },
    {"Defined Loggers",         JO_APPEND | JO_LOGGERS        },
    {"Defined Connections",     JO_APPEND | JO_CONNECTIONS    }
};

const std::vector<std::tuple<std::string, ETypeValue, uint32_t>> CJsonConfig::s_vCurrentSessionConfObjects
{
    {"Selected Session",    ETypeValue::object_value, JO_APPEND | JO_SESSIONS | JO_LINKS   },
    {"Selected Monitors",   ETypeValue::object_value, JO_APPEND | JO_MONITORS | JO_LINKS   },
    {"Selected Logger",     ETypeValue::object_value, JO_APPEND | JO_LOGGERS | JO_LINKS    },
    {"Selected Connection", ETypeValue::object_value, JO_APPEND | JO_CONNECTIONS | JO_LINKS}
};

const size_t TI_FLAG = 0;
const size_t TI_UNIT_NAME = 1;
const size_t TI_MULTIPLE = 2;

const std::vector<std::tuple<uint32_t, std::string, bool>> CJsonConfig::s_vFlagToUnitName
{
    {JO_SESSIONS,    "Session",     false},
    {JO_MONITORS,    "Monitor",     true },
    {JO_LOGGERS,     "Logger",      false},
    {JO_CONNECTIONS, "Connection",  false}
};

const std::vector<SValueType> g_mapStrToKind
{
    {"BOOLEAN",     Json::ValueType::booleanValue, ETypeValue::boolean_value},
    {"INTEGER",     Json::ValueType::intValue,     ETypeValue::integer_number},
    {"UNSIGNED",    Json::ValueType::uintValue,    ETypeValue::unsigned_number},
    {"DOUBLE",      Json::ValueType::realValue,    ETypeValue::float_number},
    {"STRING",      Json::ValueType::stringValue,  ETypeValue::string_value},
    {"HEXSTRING",   Json::ValueType::stringValue,  ETypeValue::hex_number},
    {"ARRAY",       Json::ValueType::arrayValue,   ETypeValue::arraj_value},
    {"OBJECT",      Json::ValueType::objectValue,  ETypeValue::object_value},
    {"LINK",        Json::ValueType::stringValue,  ETypeValue::link}
};

const std::vector<SValueView> g_loggerTemplate
{
    { "name",                "TestExecutor log", "STRING"  },
    { "host",                "localhost",        "STRING"  },
    { "port",                "2100",             "INTEGER" },
    { "retry",               "5",                "INTEGER" },
    { "max_message_size",    "2048",             "INTEGER" },
    { "module_tag_size",     "4",                "INTEGER" },
    { "severity",            "DEBUG",            "STRING"  },
    { "bg-color",            "lightGray",        "STRING"  },
    { "text-color",          "darkBlue",         "STRING"  },
    { "font-name",           "Courier New",      "STRING"  },
    { "font-weight",         "10",               "INTEGER" }
};

std::string CJsonConfig::s_tmpConfigPath;

//////////////////////////////////////////////////////////////////////////
CJsonConfig::CJsonConfig( QObject* parent ) :
    QObject(parent),
    m_jsonConfigPath( g_jsonConfigName ),
    m_jMain( Json::objectValue )
{
    if (s_tmpConfigPath.empty())
    {
        fs::path p = m_jsonConfigPath;
        p.replace_extension( ".tmp" );
        s_tmpConfigPath = p.string();
        RemoveTmpConfig();
    }
    InitConfig();
}

CJsonConfig::~CJsonConfig()
{

}

void CJsonConfig::InitConfig()
{
    m_jMain.clear();

    // load config file
    if (fs::exists( m_jsonConfigPath ))
    {
        size_t jFileSize = fs::file_size( m_jsonConfigPath );
        if (jFileSize > 0)
        {
            std::ifstream ifs( m_jsonConfigPath );
            if (ifs.good())
            {
                std::string jStr( jFileSize, 0 );
                ifs.read( jStr.data(), jFileSize );
                ifs.close();

                Json::Reader reader;

                if (!reader.parse( jStr, m_jMain ))
                {
                    QMessageBox::critical( Q_NULLPTR, QString( "ERROR!" ), QString( "Config parsing failed" ) );
                }
            }
        }
    }

    // check several objects are defined
    for (const auto& [name, flag] : s_vMainConfObjects)
    {
        if (!GetSettings().isMember( name ))
        {
            GetSettings()[name] = Json::Value( Json::objectValue );
        }
        // check and set default objects for CurrentSession
        if (flag == (JO_CONST | JO_CURRENT_SESSION))
        {
            Json::Value& currentSession = GetSettings()[name];
            for (const auto& [link, type, lflag] : s_vCurrentSessionConfObjects)
            {
                if (!currentSession.isMember( link ))
                {
                    switch (type)
                    {
                    case ETypeValue::object_value:
                        currentSession[link] = Json::Value( Json::objectValue );
                        break;
                    default:
                        throw std::runtime_error( "CurrentSession cannot contains simple type properties" );
                        break;
                    }
                }
            }
        }
    }

    WriteJsonConfig();
}

void CJsonConfig::WriteJsonConfig()
{
    std::ofstream ofs( m_jsonConfigPath );
    if (ofs.good())
    {
        Json::StyledStreamWriter( "  " ).write( ofs, GetSettings() );
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
    if (GetSettings()[KEY_GEOMETRY].isNull())
    {
        return "";
    }
    return GetSettings()[KEY_GEOMETRY].asCString();
}

std::vector<SValueView> CJsonConfig::GetProperties( const QString path )
{
    std::vector<SValueView> propsSet;
    OJsonValue v = GetValue( path );
    if (v && (v->isArray() || v->isObject()))
    {
        const int count = v->size();
        for (int i = 0; i < count; ++i)
        {
            SValueView vv;
            Json::Value& item = v.get()[i];
            vv.name = (item.isMember( KEY_PROP_NAME )) ? item[KEY_PROP_NAME].asCString() : "";
            vv.type = (item.isMember( KEY_PROP_TYPE )) ? item[KEY_PROP_TYPE].asCString() : "STRING";
            vv.value = (item.isMember( KEY_PROP_VALUE )) ? item[KEY_PROP_VALUE].asCString() : "";
            if (item.isMember( KEY_PROP_VALUE ))
            {
                ETypeValue t = StringTypeToProjectType( vv.type );
                Json::Value value = item[KEY_PROP_VALUE];
                switch (t)
                {
                case ETypeValue::arraj_value:
                case ETypeValue::object_value:
                    vv.value = Json::FastWriter().write( item ).c_str();
                    break;
                case ETypeValue::null:
                case ETypeValue::boolean_value:
                case ETypeValue::integer_number:
                case ETypeValue::unsigned_number:
                case ETypeValue::float_number:
                case ETypeValue::string_value:
                case ETypeValue::hex_number:
                case ETypeValue::link:
                default:
                    vv.value = value.asCString();
                    break;
                }
            }
            else
            {
                vv.value = "";
            }
            propsSet.push_back( vv );
        }
    }
    return propsSet;
}

bool CJsonConfig::IsNodeExists( const QString path )
{
    OJsonValue ov = GetValue( path );
    return ov.has_value();
}

void CJsonConfig::RemoveTmpConfig()
{
    if (fs::exists( s_tmpConfigPath ))
    {
        fs::remove( s_tmpConfigPath );
    }
}

void CJsonConfig::CreateTmpConfig()
{
    fs::copy_file( m_jsonConfigPath, s_tmpConfigPath, fs::copy_options::overwrite_existing );
}

void CJsonConfig::RestoreConfig()
{
    if (fs::exists( s_tmpConfigPath ))
    {
        fs::copy_file( s_tmpConfigPath, m_jsonConfigPath, fs::copy_options::overwrite_existing );
        InitConfig();
        RemoveTmpConfig();
    }
}

Json::Value& CJsonConfig::FindObject( const QString path )
{
    auto& obj = GetSettings();
    for (auto it : QStringTokenizer { path, u'.' })
    {
        std::string tok = it.toString().toStdString();
        if (!tok.empty())
        {
            auto& item = obj[tok];
            if (item.isObject())
            {
                obj = item;
            }
        }
    }
    return obj;
}

OJsonValue CJsonConfig::GetValue( const QString path )
{
    QList<QStringView> parsedPath = util::SplitString( path, u'.' );
    return FindValue( GetSettings(), parsedPath.cbegin(), parsedPath.cend() );
}

OJsonValue CJsonConfig::FindValue( OJsonValue prev, QList<QStringView>::const_iterator it, QList<QStringView>::const_iterator end )
{
    if (prev && it != end)
    {
        const std::string name = it->toString().toStdString();
        OJsonValue next = prev->isMember( name ) ? prev.get()[name] : OJsonValue {};
        return FindValue( next, ++it, end );
    }
    return prev;
}

ETypeValue CJsonConfig::StringTypeToProjectType( const QString& str )
{
    std::vector<SValueType>::const_iterator it = std::find_if( g_mapStrToKind.cbegin(), g_mapStrToKind.cend(), [&str]( const SValueType& t ) -> bool
    {
        return str == t.name;
    } );
    return (it != g_mapStrToKind.cend()) ? it->prjType : ETypeValue::null;
}

Json::ValueType CJsonConfig::StringTypeToJsonType( const QString& str )
{
    std::vector<SValueType>::const_iterator it = std::find_if( g_mapStrToKind.cbegin(), g_mapStrToKind.cend(), [&str]( const SValueType& t ) -> bool
    {
        return str == t.name;
    } );
    return (it != g_mapStrToKind.cend()) ? it->jType : Json::ValueType::nullValue;
}

QString CJsonConfig::JsonTypeToStringType( Json::ValueType kind )
{
    std::vector<SValueType>::const_iterator it = std::find_if( g_mapStrToKind.cbegin(), g_mapStrToKind.cend(), [&kind]( const SValueType& t ) -> bool
    {
        return kind == t.jType;
    } );
    return (it != g_mapStrToKind.cend()) ? it->name : QString {};
}

QList<QString> CJsonConfig::GetValueTypesList()
{
    QList<QString> result;
    for (const auto& t : g_mapStrToKind)
    {
        result.append( t.name );
    }
    return result;
}

uint32_t CJsonConfig::GetNodeFlagsByName( const QString& nameNode )
{
    const std::string str = nameNode.toStdString();
    if (str == std::get<TI_NAME>( s_rootConfigNode ))
    {
        return std::get<TI_MAIN_OBJ_FLAG>( s_rootConfigNode );
    }
    std::vector<std::tuple<std::string, uint32_t>>::const_iterator i =
        std::find_if( s_vMainConfObjects.cbegin(), s_vMainConfObjects.cend(), [&str]( auto& item ) -> bool
    {
        return str == std::get<TI_NAME>( item );
    } );

    if (i != s_vMainConfObjects.cend())
    {
        return std::get<TI_MAIN_OBJ_FLAG>( *i );
    }
    else
    {
        std::vector<std::tuple<std::string, ETypeValue, uint32_t>>::const_iterator j =
            std::find_if( s_vCurrentSessionConfObjects.cbegin(), s_vCurrentSessionConfObjects.cend(), [&str]( auto& item ) -> bool
        {
            return str == std::get<TI_NAME>( item );
        } );

        if (j != s_vCurrentSessionConfObjects.cend())
        {
            return std::get<TI_LINK_OBJ_FLAG>( *j );
        }
    }
    return 0;
}

QString CJsonConfig::GetRootNodeName()
{
    return std::get<TI_NAME>( s_rootConfigNode ).c_str();
}

QString CJsonConfig::GetUnitNameByFlag( uint32_t flag, bool* isMultiple )
{
    const std::vector<std::tuple<uint32_t, std::string, bool>>::const_iterator it =
        std::find_if( s_vFlagToUnitName.cbegin(), s_vFlagToUnitName.cend(), [flag]( const auto& item ) ->bool
    {
        return flag == std::get<TI_FLAG>( item );
    } );

    if (isMultiple)
    {
        *isMultiple = (it != s_vFlagToUnitName.cend()) ? std::get<TI_MULTIPLE>( *it ) : false;
    }

    return (it != s_vFlagToUnitName.cend()) ? std::get<TI_UNIT_NAME>( *it ).c_str() : "";
}


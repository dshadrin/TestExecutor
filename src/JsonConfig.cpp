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
        "Session": {
            "name": "ISWC test",
            "type": "LINK",
            "value": "Sessions.Iswc-session"
        },
        "Monitors": [
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
        ],
        "Logger": {
            "name": "Default logger",
            "type": "LINK",
            "value": "Loggers.Default-oscar"
        },
        "Connection": {
            "name": "Local",
            "type": "LINK",
            "value": "Connections.Local"
        }
    },
    "Sessions" : {
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

const std::vector<std::string> CJsonConfig::s_vMainConfObjects
{
    "CurrentSession",
    "Sessions",
    "Monitors",
    "Loggers",
    "Connections"
};

const std::vector<std::pair<std::string, ETypeValue>> CJsonConfig::s_vCurrentSessionConfObjects
{
    {"Session link", ETypeValue::object_value},
    {"Monitor links", ETypeValue::arraj_value},
    {"Logger link", ETypeValue::object_value},
    {"Connection link", ETypeValue::object_value}
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

//////////////////////////////////////////////////////////////////////////
CJsonConfig::CJsonConfig( QObject* parent ) :
    QObject(parent),
    m_jsonConfigPath( g_jsonConfigName ),
    m_jMain( Json::objectValue )
{
    if (fs::exists( m_jsonConfigPath ))
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

                Json::Reader reader;

                if (!reader.parse( jStr, m_jMain ))
                {
                    QMessageBox::critical( Q_NULLPTR, QString("ERROR!"), QString("Config parsing failed"));
                }
            }
        }
    }

    // check several objects are defined
    for (const std::string& name : s_vMainConfObjects)
    {
        if (!GetSettings().isMember(name))
        {
            GetSettings()[name] = Json::Value( Json::objectValue );
        }
        // check and set default objects for CurrentSession
        if (name == "CurrentSession")
        {
            Json::Value& currentSession = GetSettings()[name];
            for (const auto& prop : s_vCurrentSessionConfObjects)
            {
                if (!currentSession.isMember(prop.first))
                {
                    switch (prop.second)
                    {
                    case ETypeValue::object_value:
                        currentSession[prop.first] = Json::Value( Json::objectValue );
                        break;
                    case ETypeValue::arraj_value:
                        currentSession[prop.first] = Json::Value( Json::arrayValue );
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

CJsonConfig::~CJsonConfig()
{

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


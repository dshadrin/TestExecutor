#include "JsonConfig.h"
#include "JsonConfigDialog.h"
#include <QMessageBox>

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
        "Monitors": {
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
    {"Selected Monitors",   ETypeValue::arraj_value,  JO_APPEND | JO_MONITORS | JO_LINKS   },
    {"Selected Logger",     ETypeValue::object_value, JO_APPEND | JO_LOGGERS | JO_LINKS    },
    {"Selected Connection", ETypeValue::object_value, JO_APPEND | JO_CONNECTIONS | JO_LINKS}
};

const VectorValues g_loggerTemplate
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

const VectorValues g_monitorTemplate
{
    { "name",                "RTOS camera monitor", "STRING"  },
    { "connection-string",   "localhost:2002",      "STRING"  },
    { "font-name",           "Lucida Console",      "STRING"  },
    { "font-weight",         "9",                   "INTEGER" },
    { "bg-color",            "lightGray",           "STRING"  },
    { "cmd-text-color",      "darkBlue",            "STRING"  },
    { "camera-text-color",   "darkBlue",            "STRING"  }
};

const VectorValues g_connectionTemplate
{
    { "name",                "Local environment execution", "STRING"  },
    { "is-remote-location",  "false",                       "BOOLEAN" },
    { "host",                "",                            "STRING"  },
    { "port",                "22",                          "INTEGER" },
    { "password",            "",                            "STRING"  },
    { "private-key-file",    "",                            "STRING"  }
};

const VectorValues g_sessionTemplate
{
    { "execution-file",      "", "STRING" },
    { "command-line",        "", "STRING" },
    { "environment",         "[]", "ARRAY"}
};

const VectorValues g_emptyTemplate {};

const size_t TI_FLAG = 0;
const size_t TI_UNIT_NAME = 1;
const size_t TI_MULTIPLE = 2;
const size_t TI_TEMPLATE = 3;

const std::vector<std::tuple<uint32_t, std::string, bool, const VectorValues&>> CJsonConfig::s_vFlagToUnit
{
    {JO_SESSIONS,    "Session",     false, g_sessionTemplate   },
    {JO_MONITORS,    "Monitor",     true,  g_monitorTemplate   },
    {JO_LOGGERS,     "Logger",      false, g_loggerTemplate    },
    {JO_CONNECTIONS, "Connection",  false, g_connectionTemplate}
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

//////////////////////////////////////////////////////////////////////////
CJsonConfig::CJsonConfig( QObject* parent ) :
    QObject(parent),
    m_jsonConfigPath( g_jsonConfigName ),
    m_jMain( Json::objectValue )
{
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
                    case ETypeValue::arraj_value:
                        currentSession[link] = Json::Value( Json::arrayValue );
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

VectorValues CJsonConfig::GetProperties( const QString& path )
{
    VectorValues propsSet;
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
//             vv.value = (item.isMember( KEY_PROP_VALUE )) ? item[KEY_PROP_VALUE].asCString() : "";
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
                    vv.value = "";
                    vv.type = "STRING";
                    break;
                case ETypeValue::boolean_value:
                    vv.value = value.asBool() ? "true" : "false";
                    break;
                case ETypeValue::integer_number:
                    vv.value = QString::number( value.asInt64() );
                    break;
                case ETypeValue::unsigned_number:
                    vv.value = QString::number( value.asUInt64() );
                    break;
                case ETypeValue::float_number:
                    vv.value = QString::number( value.asDouble() );
                    break;
                case ETypeValue::string_value:
                case ETypeValue::hex_number:
                case ETypeValue::link:
                default:
                    vv.value = QString::fromStdString( value.asString() );
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

bool CJsonConfig::IsNodeExists( const QString& path )
{
    OJsonValue ov = GetValue( path );
    return ov.has_value();
}

bool CJsonConfig::SaveProperties( const QString& path, const VectorValues& props, bool isNew )
{
    OJsonValue jOldVal = GetValue( path );
    if (isNew && jOldVal.has_value())
    {
        QMessageBox::warning( Q_NULLPTR, QString( "Error" ), QString( "Configuration: Node " + path + " exists already" ) );
        return false; // node exist
    }

    QList<QStringView> parsedPath = util::SplitString( path, u'.' );
    std::string key = parsedPath.rbegin()->toString().toStdString();
    parsedPath.removeLast();
    OJsonValue jParent = FindValue( GetSettings(), parsedPath.cbegin(), parsedPath.cend() );
    if (!jParent.has_value())
    {
        // parent not exist - logical error
        throw std::logic_error( "Configuration: Cannot create node " + path.toStdString() );
    }

    Json::Value jVal;
    jVal = Json::Value( Json::arrayValue );
    for (const SValueView& view : props)
    {
        ETypeValue jt = StringTypeToProjectType( view.type );
        if (jt == ETypeValue::null)
            jt = ETypeValue::string_value;  // default type is string

        Json::Value jObj = Json::Value( Json::objectValue );
        jObj[KEY_PROP_NAME] = view.name.toStdString();
        jObj[KEY_PROP_TYPE] = view.type.toStdString();
        QString sVal = view.value;

        bool status = util::CheckStringValue( sVal, jt );
        if (!status)
        {
            QMessageBox::warning( Q_NULLPTR, QString( "Error data type" ), QString( "Configuration: Value " + sVal + " does not " + view.type + " type" ) );
            return false; // error data type
        }

        switch (jt)
        {
        case ETypeValue::boolean_value:
            jObj[KEY_PROP_VALUE] = ( sVal.compare( "true", Qt::CaseSensitivity::CaseInsensitive ) == 0 ||
                                     sVal.compare( "on", Qt::CaseSensitivity::CaseInsensitive ) == 0 ||
                                     sVal.compare( "auto", Qt::CaseSensitivity::CaseInsensitive ) == 0 ||
                                     sVal.compare( "0" ) != 0) ? true : false;
            break;
        case ETypeValue::float_number:
            jObj[KEY_PROP_VALUE] = sVal.toDouble();
            break;
        case ETypeValue::integer_number:
            jObj[KEY_PROP_VALUE] = sVal.toLongLong();
            break;
        case ETypeValue::unsigned_number:
            jObj[KEY_PROP_VALUE] = sVal.toULongLong();
            break;
        case ETypeValue::arraj_value:
        case ETypeValue::object_value:
        {
            Json::Value v;
            Json::Reader().parse( sVal.toStdString(), v );
            jObj[KEY_PROP_VALUE] = v;
        }
            break;
        case ETypeValue::hex_number:
        case ETypeValue::null:
        case ETypeValue::string_value:      // always true
        default:
            jObj[KEY_PROP_VALUE] = sVal.toStdString();
        }

        jVal.append( jObj );
    }
    jParent.value()[key] = jVal;
    WriteJsonConfig();
    return true;
}

bool CJsonConfig::SaveValue( const QString& path, const SValueView& value, bool isNew )
{
    bool status = false;
    OJsonValue jVal = GetValue( path );
    if (jVal.has_value())
    {
        if (jVal->isObject())
        {
            jVal->clear();
            jVal.get()[KEY_PROP_NAME] = value.name.toStdString();
            jVal.get()[KEY_PROP_VALUE] = value.value.toStdString();
            jVal.get()[KEY_PROP_TYPE] = value.type.toStdString();
            status = true;
        }
        else if (jVal->isArray())
        {
            Json::Value jv = Json::objectValue;
            jv[KEY_PROP_NAME] = value.name.toStdString();
            jv[KEY_PROP_VALUE] = value.value.toStdString();
            jv[KEY_PROP_TYPE] = value.type.toStdString();
            Json::ArrayIndex index = jVal->size();
            jVal->insert( index, jv );
            status = true;
        }
        WriteJsonConfig();
    }
    return status;
}

void CJsonConfig::RemoveNode( const QString& path, const QString& nodeName )
{
    if (!path.isEmpty() && !nodeName.isEmpty())
    {
        OJsonValue jVal = GetValue( path );
        if (jVal.has_value())
        {
            jVal->removeMember( nodeName.toStdString() );
        }
    }
}

void CJsonConfig::RenameKeyName( const QString& path, const QString& oldKey, const QString& newKey )
{
    OJsonValue jv = GetValue( path );
    if (jv.has_value())
    {
        if (jv->isObject())
        {
            if (jv->isMember( oldKey.toStdString() ))
            {
                Json::Value value = jv.get()[oldKey.toStdString()];
                jv->removeMember( oldKey.toStdString() );
                jv.get()[newKey.toStdString()] = value;
            }
        }
        else if (jv->isArray())
        {
            // TODO:
        }
        WriteJsonConfig();
    }
}

void CJsonConfig::RemoveBackup()
{
    m_jBackup.clear();
    WriteJsonConfig();
}

void CJsonConfig::CreateBackup()
{
    m_jBackup = m_jMain;
    WriteJsonConfig();
}

void CJsonConfig::RestoreFromBackup()
{
    m_jMain = m_jBackup;
    WriteJsonConfig();
    InitConfig();
    m_jBackup.clear();
}

OJsonValue CJsonConfig::GetValue( const QString& path )
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

QString CJsonConfig::GetUnitNameByFlags( uint32_t flag, bool* isMultiple )
{
    auto& it = FindUnitDefinition( flag );

    if (isMultiple)
    {
        *isMultiple = (it != s_vFlagToUnit.cend()) ? std::get<TI_MULTIPLE>( *it ) : false;
    }

    return (it != s_vFlagToUnit.cend()) ? std::get<TI_UNIT_NAME>( *it ).c_str() : "";
}

const VectorValues& CJsonConfig::GetUnitTemplateByFlags( uint32_t flag )
{
    auto& it = FindUnitDefinition( flag );
    return (it != s_vFlagToUnit.cend()) ? std::get<TI_TEMPLATE>( *it ) : g_emptyTemplate;
}

const std::vector<std::tuple<uint32_t, std::string, bool, const VectorValues&>>::const_iterator CJsonConfig::FindUnitDefinition( uint32_t flag )
{
    const uint32_t unitFlag = flag & JO_MAIN_MASK;
    const std::vector<std::tuple<uint32_t, std::string, bool, const VectorValues&>>::const_iterator it =
        std::find_if( s_vFlagToUnit.cbegin(), s_vFlagToUnit.cend(), [unitFlag]( const auto& item ) ->bool
    {
        return unitFlag == std::get<TI_FLAG>( item );
    } );
    return it;
}

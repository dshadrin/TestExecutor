#include "AppConfig.h"
#include "utils/Exceptions.h"
#include <QRegularExpression>
#include <boost/describe.hpp>

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_MODULE_TAG( CAppConfig, "CONF" );

//////////////////////////////////////////////////////////////////////////
// Application config structure
//////////////////////////////////////////////////////////////////////////
namespace
{
const std::string sDefinitions = " definitions";
const std::string sActivity = "Activity";
const std::string sComponents = "Components";
const std::string sCurrentSet = "CurrentSet";
const std::string sApplication = "Application";
const std::string sConnection = "Connection";
const std::string sSession = "Session";
const std::string sMonitors = "Console";
const std::string sLogger = "Logger";
const std::string sConnectionsArray = sConnection + sDefinitions;
const std::string sSessionsArray = sSession + sDefinitions;
const std::string sMonitorsArray = sMonitors + sDefinitions;
const std::string sLoggersArray = sLogger + sDefinitions;

const ListNodesProperty listHighNodes
{
    {QString::fromStdString( sActivity ),   1, ETypeValue::ObjectValue, JO_CONST},
    {QString::fromStdString( sComponents ), 1, ETypeValue::ObjectValue, JO_CONST}
};

const ListNodesProperty listActivityNodes
{
    {QString::fromStdString( sCurrentSet ),  1, ETypeValue::ObjectValue, JO_CONST | JO_CURRENT_SESSION},
    {QString::fromStdString( sApplication ), 0, ETypeValue::ObjectValue, JO_APPLICATION}
};

const ListNodesProperty listComponents // strings arrays
{
    {QString::fromStdString( sConnection ), 1, ETypeValue::ObjectValue, JO_APPEND | JO_CONNECTIONS | JO_LINKS | JO_REQUIRED},
    {QString::fromStdString( sSession ),    1, ETypeValue::ObjectValue, JO_APPEND | JO_SESSIONS    | JO_LINKS | JO_REQUIRED},
    {QString::fromStdString( sMonitors ),  -1, ETypeValue::ObjectValue, JO_APPEND | JO_MONITORS    | JO_LINKS},
    {QString::fromStdString( sLogger ),     1, ETypeValue::ObjectValue, JO_APPEND | JO_LOGGERS     | JO_LINKS | JO_REQUIRED}
};

const ListNodesProperty listComponentDefinitions // objects set
{
    {QString::fromStdString( sConnectionsArray ), -1, ETypeValue::ArrajValue, JO_APPEND | JO_CONNECTIONS | JO_REQUIRED},
    {QString::fromStdString( sSessionsArray ),    -1, ETypeValue::ArrajValue, JO_APPEND | JO_SESSIONS    | JO_REQUIRED},
    {QString::fromStdString( sMonitorsArray ),    -1, ETypeValue::ArrajValue, JO_APPEND | JO_MONITORS},
    {QString::fromStdString( sLoggersArray ),     -1, ETypeValue::ArrajValue, JO_APPEND | JO_LOGGERS     | JO_REQUIRED}
};

const ListNodesProperty listNodesEmpty;
const std::string gsGeometryPath( sActivity + "." + sApplication + "." + "Geometry" );

const SLoggerProperty defaultLogger
{
    // "TestExecutor log",
    "localhost",
    2100,
    5,
    2048,
    4,
    "DEBUG",
    "lightGray",
    "darkBlue",
    "Courier New",
    10
};

const SMonitorProperty defaultMonitor
{
    // "RTOS camera monitor",
    "localhost",
    2002,
    "lightGray",
    "darkBlue",
    "darkGreen",
    "Lucida Console",
    9
};

const SSessionProperty defaultSession
{
    // "Project name",
    "test_app.exe",
    "-ln",
    "[]"
};

const SConnectionProperty defaultConnection
{
    // "Local"
    false
};

static const std::vector<QString> g_boolFalseValus
{
    "false",
    "manual",
    "off"
};

static const std::vector<QString> g_boolTrueValus
{
    "true",
    "auto",
    "on"
};

bool IsFalseTextValue( const QString& text )
{
    return std::find( g_boolFalseValus.cbegin(), g_boolFalseValus.cend(), text.toLower() ) != g_boolFalseValus.cend();
}

bool IsTrueTextValue( const QString& text )
{
    return std::find( g_boolTrueValus.cbegin(), g_boolTrueValus.cend(), text.toLower() ) != g_boolTrueValus.cend();
}

bool IsTextFloatNumber( const QString& text )
{
    QRegularExpression expr { R"(^[+-]{0,1}\d+\.{0,1}\d*([eE]{1}[+-]{0,1}\d+){0,1}$)" };
    QRegularExpressionMatch match = expr.match( text );
    return match.captured( 0 ) == text;
}

bool IsTextHexNumber( const QString& text )
{
    QRegularExpression expr { R"(^[[:xdigit:]]+$)" };
    QRegularExpressionMatch match = expr.match( text );
    return match.captured( 0 ) == text;
}

bool IsTextUnsignedInteger( const QString& text )
{
    QRegularExpression expr { R"(^+{0,1}[[:digit:]]+$)" };
    QRegularExpressionMatch match = expr.match( text );
    return match.captured( 0 ) == text;
}

bool IsTextInteger( const QString& text )
{
    QRegularExpression expr { R"(^[+-]{0,1}[[:digit:]]+$)" };
    QRegularExpressionMatch match = expr.match( text );
    return match.captured( 0 ) == text;
}

}

//////////////////////////////////////////////////////////////////////////
CAppConfig::CAppConfig( std::string_view congName ) :
    CJsonConfig(congName)
{
    CheckRequiredNodes();
}

CAppConfig::~CAppConfig()
{

}

void CAppConfig::SaveGeometry( const QByteArray& geometry )
{
    const std::string value = geometry.toStdString();
    SetValue<std::string>( gsGeometryPath, value );
}

QByteArray CAppConfig::GetGeometry() const
{
    top::optional<std::string> value = GetValue<std::string>( gsGeometryPath );
    return QByteArray::fromStdString( value.has_value() ? value.get() : "" );
}

void CAppConfig::CheckRequiredNodes()
{
    // high level
    rng::for_each( listHighNodes, [this]( const auto& n )
    {
        FindJsonValue( n.name.toStdString() );
    } );

    // activity
    rng::for_each( listActivityNodes, [this]( const auto& n )
    {
        FindJsonValue( sActivity + "." + n.name.toStdString() );
    } );

    // activity current set components
    rng::for_each( listComponents, [this]( const auto& n )
    {
        FindJsonValue( sActivity + "." + sCurrentSet + "." + n.name.toStdString() );
    } );

    // components
    rng::for_each( listComponentDefinitions, [this]( const auto& n )
    {
        FindJsonValue( sComponents + "." + n.name.toStdString() );
    } );

    Flush();
}

const ListNodesProperty& CAppConfig::GetListMainNodes( const QString& parentName ) const
{
    if (parentName.isEmpty())
        return listHighNodes;

    else if (parentName == QString::fromStdString( sActivity ))
        return listActivityNodes;

    else if (parentName == QString::fromStdString( sCurrentSet ))
        return listComponents;

    else if (parentName == QString::fromStdString( sComponents ))
        return listComponentDefinitions;

    return listNodesEmpty;
}

ListNodesProperty CAppConfig::GetListPropertyNodes( const QString& path, quint32 mask ) const
{
    ListNodesProperty props;
    if (!path.isEmpty())
    {
        top::optional<const Json::Value&> nodeOp = FindJsonValue( path.toStdString() );
        if (nodeOp.has_value() && !IsNull( nodeOp.get() ) && !Empty( nodeOp.get() ) && (mask & JO_APPEND))
        {
            auto fillProp = [&props, mask]( const std::string& name ) -> void
            {
                SNodeProperty prop {
                    .name = QString::fromStdString( name ),
                    .maxCount = 0,
                    .type = ETypeValue::ObjectValue,
                    .mask = mask & JO_COMPONENT_MASK
                };
                
                if (mask & JO_LINKS)
                {
                    prop.mask |= JO_LINKS;
                    prop.type = ETypeValue::LinkValue;
                }
                props.append( prop );
            };
            
            const Json::Value& node = nodeOp.get();
            if (IsArray( node ) && (mask & JO_LINKS) != 0) // component links
            {
                unsigned count = Size( node );
                for (unsigned i : rng::views::iota( 0u, count ))
                {
                    fillProp( ToString( At( node, i ) ) );
                }
            }
            else if (IsObject( node )) // components set
            {
                auto names = GetMemberNames( node );
                rng::for_each( names, fillProp );
            }
        }
    }
    return props;
}

ETypeValue CAppConfig::TypeFromString( const QString& str )
{
    ETypeValue val;
    bool status = boost::describe::enum_from_string( str.toStdString().c_str(), val );
    if (!status)
    {
        tex::ThrowException<std::logic_error>( fmt::format( FMT_STRING( "Error convert {:s} to ETypeValue" ), str.toStdString() ) );
    }
    return val;
}

QString CAppConfig::TypeToString( ETypeValue val )
{
    std::string def = "(unnamed" + std::to_string( static_cast<std::underlying_type_t<ETypeValue>>(val) ) + ")";
    return boost::describe::enum_to_string( val, def.c_str() );
}

bool CAppConfig::CheckStringValue( const QString& text, ETypeValue type )
{
    bool status = true;
    switch (type)
    {
    case ETypeValue::NullValue:
        status = text.isNull();
        break;
    case ETypeValue::BooleanValue:
        status = (IsFalseTextValue( text ) || IsTrueTextValue( text ));
        break;
    case ETypeValue::FloatValue:
        status = IsTextFloatNumber( text );
        break;
    case ETypeValue::HexNumberString:
        status = IsTextHexNumber( text );
        break;
    case ETypeValue::IntegerValue:
        status = IsTextInteger( text );
        break;
    case ETypeValue::UnsignedValue:
        status = IsTextUnsignedInteger( text );
        break;
    case ETypeValue::ArrajValue:
        status = CJsonConfig::IsTextJsonArray( text.toStdString() );
        break;
    case ETypeValue::ObjectValue:
        status = CJsonConfig::IsTextJsonObject( text.toStdString() );
        break;
    case ETypeValue::StringValue:      // always true
    default:;
    }
    return status;
}

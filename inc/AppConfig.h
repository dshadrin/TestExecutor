#pragma once

//////////////////////////////////////////////////////////////////////////
#include "logclient/LogClient.h"
#include "Common.h"
#include "utils/JsonConfig.h"
#include <QVector>
#include <QString>
#include <QMessageBox>

//////////////////////////////////////////////////////////////////////////
// Config item masks
//////////////////////////////////////////////////////////////////////////
enum ENodeMask : quint32
{
    JO_UNSPECIFIED = 0,
    // components
    JO_SESSIONS          = 0x0001,
    JO_CONNECTIONS       = 0x0002,
    JO_MONITORS          = 0x0003,
    JO_LOGGERS           = 0x0004,
    JO_COMPONENT_MASK    = 0x000F,
    // domain
    JO_APPLICATION       = 0x0010,
    JO_REQUIRED          = 0x0020,
    JO_CURRENT_SESSION   = 0x0040,
    JO_DOMAIN_MASK       = 0x00F0,
    // assess
    JO_CONST             = 0x0100,
    JO_APPEND            = 0x0200,
    JO_LINKS             = 0x0400,
    JO_ACCESS_MASK       = 0x0F00
};

Q_DECLARE_METATYPE( ETypeValue )

//////////////////////////////////////////////////////////////////////////
// description
//////////////////////////////////////////////////////////////////////////
/*
    --------------------------------------------------
    Config have two high level trees
    1. Active configuration
    2. Components variants configuration
    --------------------------------------------------

    --------------------------------------------------
    Active configuration
    1. Components set
    2. Application values set

    Components set
    1. Connection set name
    2. Monitor sets names
    3. Logger set name
    4. Session set name

    Application values set
    1. Geometry
    --------------------------------------------------

    --------------------------------------------------
    Components variants configuration
    1. Connection sets
    2. Monitor sets
    3. Logger sets
    4. Session sets
    --------------------------------------------------
*/

//////////////////////////////////////////////////////////////////////////
// types
//////////////////////////////////////////////////////////////////////////

// tree nodes property
struct SNodeProperty
{
    QString name;
    qint32 maxCount;
    ETypeValue type;
    quint32 mask;
};

BOOST_DESCRIBE_STRUCT(SNodeProperty, (), (name, maxCount, mask, type, mask))

typedef QVector<SNodeProperty> ListNodesProperty;

// Logger property
struct SLoggerProperty
{
    //std::string componentName               = "TestExecutor log";
    std::string connectionHost              = "localhost";
    uint16_t connectionPort                 = 2100;
    uint16_t retryConnectiSLoggerPropertyon = 5;
    uint16_t maxMessageSize                 = 2048;
    uint16_t moduleTagSize                  = 4;
    std::string severity                    = "DEBUG";
    std::string bgColor                     = "lightGray";
    std::string textColor                   = "darkBlue";
    std::string fontName                    = "Courier New";
    uint16_t fontWeight                     = 10;
};

BOOST_DESCRIBE_STRUCT(SLoggerProperty, (), (/* componentName, */ connectionHost, connectionPort, retryConnectiSLoggerPropertyon, maxMessageSize, moduleTagSize, severity, bgColor, textColor, fontName, fontWeight))

// Logger property
struct SMonitorProperty
{
    //std::string componentName               = "RTOS camera monitor";
    std::string connectionHost              = "localhost";
    uint16_t connectionPort                 = 2002;
    std::string bgColor                     = "lightGray";
    std::string cmdTextColor                = "darkBlue";
    std::string cameraTextColor             = "darkBlue";
    std::string fontName                    = "Lucida Console";
    uint16_t fontWeight                     = 9;
};

BOOST_DESCRIBE_STRUCT(SMonitorProperty, (), (/* componentName, */ connectionHost, connectionPort, bgColor, cmdTextColor, cameraTextColor, fontName, fontWeight))

// Logger property
struct SSessionProperty
{
    //std::string componentName;
    std::string executionPath;
    std::string cmdLine;
    std::string environment                 = "[]";
};

BOOST_DESCRIBE_STRUCT(SSessionProperty, (), (/* componentName, */ executionPath, cmdLine, environment))

// Logger property
struct SConnectionProperty
{
    //std::string componentName;
    bool isRemote;
};

BOOST_DESCRIBE_STRUCT( SConnectionProperty, (), (/* componentName, */ isRemote) )

//////////////////////////////////////////////////////////////////////////
class CAppConfig : public CJsonConfig
{
public:
    CAppConfig(std::string_view congName);
    ~CAppConfig() override;

    void SaveGeometry( const QByteArray& geometry );
    QByteArray GetGeometry() const;

    const ListNodesProperty& GetListMainNodes( const QString& parentName = "" ) const;
    ListNodesProperty GetListPropertyNodes( const QString& path, quint32 mask ) const;

    template<class _PropsSet>
    _PropsSet GetByPath(const QString& path) const;

    template<class _PropsSet>
    void SetByPath(const QString& path, const _PropsSet& props);

    static ETypeValue TypeFromString( const QString& str );
    static QString TypeToString( ETypeValue val );
    static bool CheckStringValue( const QString& text, ETypeValue type );

private:
    void CheckRequiredNodes();
    DECLARE_MODULE_TAG;
};


template<class _PropsSet>
inline _PropsSet CAppConfig::GetByPath(const QString& path) const
{
    top::optional<const Json::Value&> jvCppOp = FindJsonValue( path.toStdString() );
    if (jvCppOp.has_value() && !IsNull(jvCppOp.get()))
    {
        std::ostringstream oss;
        CJsonConfig::WriteToStream( oss, jvCppOp.get() );
        return CJsonConfig::FromJsonString<_PropsSet>(oss.str());
    }
    return _PropsSet{};
}

template<class _PropsSet>
inline void CAppConfig::SetByPath(const QString& path, const _PropsSet& props)
{
    Json::Value& jvCpp = FindJsonValue(path.toStdString());
    std::istringstream iss(CJsonConfig::ToJsonString<_PropsSet>(props));
    Json::Value jv;
    std::string err;
    bool status = CJsonConfig::ReadFromStream(iss, jv, err);
    if (status)
    {
        jvCpp = jv;
        Flush();
    }
    else
    {
        QMessageBox::warning( Q_NULLPTR, QString( "Error store data in config" ), QString::fromStdString( err ) );
    }
}

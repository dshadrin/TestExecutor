#pragma once

//////////////////////////////////////////////////////////////////////////
#include "logclient/LogClient.h"
#include "Common.h"
#include "utils/JsonConfig.h"
#include <QVector>
#include <QString>

//////////////////////////////////////////////////////////////////////////
// forward declaration
//////////////////////////////////////////////////////////////////////////
class CAppConfigDialog;

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
};

// Logger property
struct SLoggerProperty
{
    std::string componentName;
    std::string connectionHost;
    uint16_t connectionPort;
    uint16_t retryConnection;
    uint16_t maxMessageSize;
    uint16_t moduleTagSize;
    std::string severity;
    std::string bgColor;
    std::string textColor;
    std::string fontName;
    uint16_t fontWeight;
};

// Logger property
struct SMonitorProperty
{
    std::string componentName;
    std::string connectionHost;
    uint16_t connectionPort;
    std::string bgColor;
    std::string cmdTextColor;
    std::string cameraTextColor;
    std::string fontName;
    uint16_t fontWeight;
};

// Logger property
struct SSessionProperty
{
    std::string componentName;
    std::string executionPath;
    std::string cmdLine;
    std::string environment;
};

// Logger property
struct SConnectionProperty
{
    std::string componentName;
};


//////////////////////////////////////////////////////////////////////////
class CAppConfig : public CJsonConfig
{
public:
    CAppConfig(const std::string& congName);
    ~CAppConfig() override;

    void SaveGeometry( const QByteArray& geometry );
    QByteArray GetGeometry() const;

    const QVector<SNodeProperty>& GetNodeProperties( const QString& parentName = "") const;

private:
    void CheckRequiredNodes();
    DECLARE_MODULE_TAG;
};

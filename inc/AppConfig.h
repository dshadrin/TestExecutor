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
struct SNodeProperty
{
//    SNodeProperty(const QString& s, qint32 c, ETypeValue t) : name(s), maxCount(c), type(t) {}
    QString name;
    qint32 maxCount;
    ETypeValue type;
};

//////////////////////////////////////////////////////////////////////////
class CAppConfig : public CJsonConfig
{
public:
    CAppConfig(const std::string& congName);
    ~CAppConfig() override;

    void SaveGeometry( const QByteArray& geometry );
    QByteArray GetGeometry() const;

private:
    void CheckRequiredNodes();
    DECLARE_MODULE_TAG;
};

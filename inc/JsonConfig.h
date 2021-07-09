#pragma once

//////////////////////////////////////////////////////////////////////////
#include "logclient/LogClient.h"
#include "Common.h"
#include <json/json.h>
#include <boost/optional.hpp>
#include <QList>

#define KEY_GEOMETRY "Geometry"
#define KEY_PROPS_ARRAY_NAME "Properties"
#define KEY_PROP_NAME "Name"
#define KEY_PROP_VALUE "Value"
#define KEY_PROP_TYPE "Type"

class CJsonConfigDialog;

struct SValueType
{
    QString name;
    Json::ValueType jType;      // json type
    ETypeValue prjType;         // internal project type
};

typedef boost::optional<SValueView> OValueView;
typedef boost::optional<Json::Value&> OJsonValue;

//////////////////////////////////////////////////////////////////////////
class CJsonConfig : public QObject
{
    Q_OBJECT

public:
    CJsonConfig(QObject* parent = Q_NULLPTR);
    ~CJsonConfig();

    Json::Value& GetSettings() { return m_jMain; }
    void WriteJsonConfig();
    void SaveGeometry( const std::string& value );
    std::string GetGeometry();
    std::vector<SValueView> GetProperties(const QString path);

    template<typename _Type>
    OValueView Get(const QString& path, int index);

    static ETypeValue StringTypeToProjectType( const QString& str );
    static Json::ValueType StringTypeToJsonType( const QString& str );
    static QList<QString> GetValueTypesList();

private:
    Json::Value& FindObject( const QString path );
    OJsonValue GetValue( const QString path );
    // recursive find by list
    OJsonValue FindValue( OJsonValue prev, QList<QStringView>::const_iterator it, QList<QStringView>::const_iterator end );

private:
    std::string m_jsonConfigPath;
    std::string m_localeString;
    Json::Value m_jMain;
    static const std::vector<std::string> s_vMainConfObjects;
    DECLARE_MODULE_TAG;
};

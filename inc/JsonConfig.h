#pragma once

//////////////////////////////////////////////////////////////////////////
#include "logclient/LogClient.h"
#include "Common.h"
#include <QList>
#include <json/json.h>
#include <boost/optional.hpp>
#include <tuple>

#define KEY_GEOMETRY "Geometry"
#define KEY_PROPS_ARRAY_NAME "Properties"
#define KEY_PROP_NAME "Name"
#define KEY_PROP_VALUE "Value"
#define KEY_PROP_TYPE "Type"

#define KEY_CURRENT_SESSION      "CurrentSession"
#define KEY_DEFINED_SESSIONS     "Defined Sessions"
#define KEY_DEFINED_MONITORS     "Defined Monitors"
#define KEY_DEFINED_LOGGERS      "Defined Loggers"
#define KEY_DEFINED_CONNECTIONS  "Defined Connections"
#define KEY_SELECTED_SESSION     "Session"
#define KEY_SELECTED_MONITORS    "Monitors"
#define KEY_SELECTED_LOGGER      "Logger"
#define KEY_SELECTED_CONNECTION  "Connection"


class CJsonConfigDialog;

struct SValueType
{
    QString name;
    Json::ValueType jType;      // json type
    ETypeValue prjType;         // internal project type
};

typedef boost::optional<SValueView> OValueView;
typedef boost::optional<Json::Value&> OJsonValue;
typedef boost::optional<const Json::Value&> OCJsonValue;

enum : uint32_t
{
    JO_UNSPECIFIED = 0,
    JO_CURRENT_SESSION = 0x0001,
    JO_SESSIONS = 0x0002,
    JO_MONITORS = 0x0003,
    JO_LOGGERS = 0x0004,
    JO_CONNECTIONS = 0x0005,
    JO_MAIN_MASK = 0x00FF,
    JO_LINKS = 0x0100,
    JO_CONST = 0x1000,
    JO_APPEND = 0x2000,
    JO_ACCESS_MASK = 0xF000
};

typedef std::vector<SValueView> VectorValues;

//////////////////////////////////////////////////////////////////////////
class CJsonConfig : public QObject
{
    Q_OBJECT

public:
    CJsonConfig(QObject* parent = Q_NULLPTR);
    ~CJsonConfig();

    const Json::Value& GetSettings() const { return m_jMain; }
    QString GetGeometry() const;
    bool IsNodeExists( const QString& path ) const;
    VectorValues GetProperties( const QString& path ) const;

    void SaveGeometry( const QString& value );
    bool SaveProperties( const QString& path, const VectorValues& props, bool isNew = false );
    bool SaveValue( const QString& path, const SValueView& value, bool isNew = false );
    void RemoveNode( const QString& path, const QString& nodeName );
    void RenameKeyName( const QString& path, const QString& oldKey, const QString& newKey );

//     template<typename _Type>
//     OValueView Get(const QString& path, int index);

    static ETypeValue StringTypeToProjectType( const QString& str );
    static Json::ValueType StringTypeToJsonType( const QString& str );
    static QString JsonTypeToStringType( Json::ValueType kind );
    static QList<QString> GetValueTypesList();
    static uint32_t GetNodeFlagsByName( const QString& nameNode );
    static QString GetRootNodeName();
    static QString GetUnitNameByFlags( uint32_t flag, bool* isMultiple = nullptr );
    static const VectorValues& GetUnitTemplateByFlags( uint32_t flag );

    // config backup/undo operations
    void RemoveBackup() const;
    void CreateBackup() const;
    void RestoreFromBackup();

private:
    void InitConfig();
    void WriteJsonConfig() const;
    OJsonValue GetValue( const QString& path ) const;
    OJsonValue FindValue( OJsonValue prev, QList<QStringView>::const_iterator it, QList<QStringView>::const_iterator end ) const; // recursive find by list
    Json::Value& GetSettings() { return m_jMain; }

    static const std::vector<std::tuple<uint32_t, std::string, bool, const VectorValues&>>::const_iterator FindUnitDefinition( uint32_t flag );

private:
    std::string m_jsonConfigPath;
    std::string m_localeString;
    Json::Value m_jMain;
    mutable Json::Value m_jBackup;
    static const std::tuple<std::string, uint32_t> s_rootConfigNode;
    static const std::vector<std::tuple<std::string, uint32_t>> s_vMainConfObjects;
    static const std::vector<std::tuple<std::string, ETypeValue, uint32_t>> s_vCurrentSessionConfObjects;
    static const std::vector<std::tuple<uint32_t, std::string, bool, const VectorValues&>> s_vFlagToUnit;
    DECLARE_MODULE_TAG;
};

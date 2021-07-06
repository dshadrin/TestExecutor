#pragma once

//////////////////////////////////////////////////////////////////////////
#include "logclient/logclient.h"
#include <QSettings>
#include <boost/json.hpp>

#define KEY_GEOMETRY "geometry"

class CJsonConfigDialog;

//////////////////////////////////////////////////////////////////////////
class CJsonConfig : public QObject
{
    Q_OBJECT

public:
    CJsonConfig(QObject* parent = Q_NULLPTR);
    ~CJsonConfig();

    boost::json::object& GetSettings() { return m_jMain.as_object(); }
    void WriteJsonConfig();
    void SaveGeometry( const std::string& value );
    std::string GetGeometry();

    // old
    void StoreOptionValues( CJsonConfigDialog* dlg );
    void SetWorkDirectory();

private:
    void PrettyPrint( std::ostream& os, boost::json::value const& jv, size_t indentSize = 2, std::string* indent = nullptr );

private:
    std::string m_jsonConfigPath;
    std::string m_localeString;
    boost::json::value m_jMain;
    static const std::vector<std::string> s_vMainConfObjects;
    DECLARE_MODULE_TAG;
};

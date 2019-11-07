#pragma once

//////////////////////////////////////////////////////////////////////////

#include <QSettings>
#include <boost/property_tree/ptree.hpp>

//////////////////////////////////////////////////////////////////////////
#define KEY_TEST_APP "test-application"
#define KEY_PROXY_APP "proxy-application"
#define KEY_PATH "path"
#define KEY_TEST_APP_PATH KEY_TEST_APP "." KEY_PATH
#define KEY_PROXY_PATH KEY_PROXY_APP "." KEY_PATH
#define KEY_GEOMETRY "geometry"

class CConfig;

//////////////////////////////////////////////////////////////////////////
class CConfig : public QObject
{
    Q_OBJECT

public:
    CConfig(QObject* parent = Q_NULLPTR);
    ~CConfig();

    boost::property_tree::ptree& GetSettings() { return m_pt; }
    void WriteJsonConfig();

    void StoreTestAppPath(const std::string& path);
    std::string ReadTestAppPath();

    void StoreProxyPath(const std::string& path);
    std::string ReadProxyPath();

private:
    std::locale GetLocale();

private:
    std::string m_jsonConfigPath;
    std::string m_localeString;
    boost::property_tree::ptree m_pt;
};

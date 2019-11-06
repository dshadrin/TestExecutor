#pragma once

//////////////////////////////////////////////////////////////////////////

#include <QSettings>
#include <boost/property_tree/ptree.hpp>

//////////////////////////////////////////////////////////////////////////
const char KEY_TEST_APP[]{ "test-application" };
const char KEY_PATH[]{ "path" };

class CConfig;

//////////////////////////////////////////////////////////////////////////
class CConfig : public QObject
{
    Q_OBJECT

public:
    CConfig(QObject* parent = Q_NULLPTR);
    ~CConfig();

    QSettings& GetSettings() { return m_settings; }

private:
    void WriteJsonConfig();
    std::locale GetLocale();

private:
    QSettings m_settings;
    std::string m_jsonConfigPath;
    std::string m_localeString;
    boost::property_tree::ptree m_pt;
};

#pragma once

//////////////////////////////////////////////////////////////////////////
#include "logclient/LogClient.h"
#include <QSettings>
#include <boost/property_tree/ptree.hpp>

//////////////////////////////////////////////////////////////////////////
#define KEY_TEST_APP "test-application"
#define KEY_RUN_BEFORE_APP "run-before-application"
#define KEY_PATH "path"
#define KEY_ENV "environment"
#define KEY_TEST_APP_PATH KEY_TEST_APP "." KEY_PATH
#define KEY_RUN_BEFORE_PATH KEY_RUN_BEFORE_APP "." KEY_PATH
#define KEY_ENV_PATH KEY_TEST_APP "." KEY_ENV
#define KEY_GEOMETRY "geometry"

class CConfigDialog;

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

    void StoreRunBeforePath(const std::string& path);
    std::string ReadRunBeforePath();

    void FillOptionsDialog( CConfigDialog* dlg );
    void StoreOptionValues( CConfigDialog* dlg );

    void SetWorkDirectory();

private:
    std::string m_jsonConfigPath;
    std::string m_localeString;
    boost::property_tree::ptree m_pt;
    DECLARE_MODULE_TAG;
};

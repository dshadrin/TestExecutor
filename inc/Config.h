#pragma once

//////////////////////////////////////////////////////////////////////////

#include "ui_SessionConfig.h"
#include <QDialog>
#include <QSettings>
#include <boost/property_tree/ptree.hpp>

//////////////////////////////////////////////////////////////////////////
const char KEY_TEST_APP[]{ "test-application" };
const char KEY_PATH[]{ "path" };

class CConfig;

//////////////////////////////////////////////////////////////////////////
class CConfig : public QDialog
{
    Q_OBJECT

public:
    CConfig(QWidget* parent = Q_NULLPTR);
    ~CConfig();

    QSettings& GetSettings() { return m_settings; }

private:
    void WriteJsonConfig();
    std::locale GetLocale();

    void ConfigureDialog();

private:
    QSettings m_settings;
    std::string m_jsonConfigPath;
    std::string m_localeString;
    boost::property_tree::ptree m_pt;

    Ui::Dialog ui;
};

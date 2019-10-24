#pragma once

//////////////////////////////////////////////////////////////////////////

#include <QDialog>
#include <QSettings>
#include <boost/property_tree/ptree.hpp>

//////////////////////////////////////////////////////////////////////////
const char KEY_TEST_APP[]{ "test-application" };
const char KEY_PATH[]{ "path" };

//////////////////////////////////////////////////////////////////////////
class CConfig : public QDialog
{
    Q_OBJECT

public:
    CConfig(QWidget* parent = Q_NULLPTR);

    QSettings& GetSettings() { return m_settings; }

private:
    void WriteJsonConfig() const;

private:
    QSettings m_settings;
    std::string m_jsonConfigPath;
    boost::property_tree::ptree m_pt;
};

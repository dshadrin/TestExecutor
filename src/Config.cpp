#include "StdInc.h"
#include "Config.h"
#include "boost/property_tree/json_parser.hpp"

//////////////////////////////////////////////////////////////////////////
namespace bpt = boost::property_tree;

//////////////////////////////////////////////////////////////////////////
extern QString g_QConfigName;

//////////////////////////////////////////////////////////////////////////
CConfig::CConfig(QWidget* parent) :
    m_settings(g_QConfigName, QSettings::IniFormat, Q_NULLPTR),
    m_jsonConfigPath((fs::current_path() / fs::path(g_QConfigName.toStdString()).filename().replace_extension(".json")).string())
{
    if (fs::exists(m_jsonConfigPath))
    {
        bpt::read_json(m_jsonConfigPath, m_pt, GetLocale() );
    }
    else
    {
        bpt::ptree p;
        p.put<std::string>(KEY_PATH, "");
        m_pt.add_child(KEY_TEST_APP, p);
        WriteJsonConfig();
    }

    ConfigureDialog();
}

CConfig::~CConfig()
{
    m_settings.sync();
}

std::locale CConfig::GetLocale()
{
    if ( m_localeString.empty() )
    {
        QString locStr = GetSettings().value( "locale", "" ).toString();
        if ( locStr.isEmpty() )
        {
            locStr = "ru_RU";
            GetSettings().setValue( "locale", locStr );
        }
        m_localeString = locStr.toStdString();
    }
    return std::locale( m_localeString );
}

void CConfig::WriteJsonConfig()
{
    bpt::write_json(m_jsonConfigPath, m_pt, GetLocale(), true);
}

void CConfig::ConfigureDialog()
{
    ui.setupUi( this );
    setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint );
    setAttribute( Qt::WA_CustomWhatsThis );

    QFont headerFont( "Times", 10, QFont::Bold );

    QTableWidgetItem* nameHeaderItem = new QTableWidgetItem( tr( "Name" ) );
    //nameHeaderItem->setIcon( QIcon( QPixmap( ":/Images/cubed.png" ) ) );
    nameHeaderItem->setTextAlignment( Qt::AlignLeft );
    nameHeaderItem->setFont( headerFont );
    ui.envTableWidget->setHorizontalHeaderItem( 0, nameHeaderItem );
    ui.paramTableWidget->setHorizontalHeaderItem( 0, nameHeaderItem );

    QTableWidgetItem* valHeaderItem = new QTableWidgetItem( tr( "Value" ) );
    //valHeaderItem->setIcon( QIcon( QPixmap( ":/Images/cubed.png" ) ) );
    valHeaderItem->setTextAlignment( Qt::AlignLeft );
    valHeaderItem->setFont( headerFont );
    ui.envTableWidget->setHorizontalHeaderItem( 1, valHeaderItem );
    ui.paramTableWidget->setHorizontalHeaderItem( 1, valHeaderItem );
}

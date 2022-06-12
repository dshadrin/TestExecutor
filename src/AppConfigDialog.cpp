#include "AppConfigDialog.h"
#include "Common.h"

//////////////////////////////////////////////////////////////////////////
const int HEX_BASE = 16;

//////////////////////////////////////////////////////////////////////////
CAppConfigDialog::CAppConfigDialog( CAppConfig* pConfig, QWidget* parent) :
    QDialog( parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint ),
    m_pConfig( pConfig ),
    m_treeContextMnu(nullptr),
    m_bDirty(false)
{
}

CAppConfigDialog::~CAppConfigDialog()
{

}

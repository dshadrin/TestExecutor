#include "StdInc.h"
#include "VarEditDialog.h"

//////////////////////////////////////////////////////////////////////////
CVarEditor::CVarEditor( const QString& name, QWidget* parent)
{
    uiVarEdit.setupUi( this );
    setWindowTitle( name );
    setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint );
    setAttribute( Qt::WA_CustomWhatsThis );
}

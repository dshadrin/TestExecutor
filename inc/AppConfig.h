#pragma once

//////////////////////////////////////////////////////////////////////////
#include "logclient/LogClient.h"
#include "Common.h"
#include "utils/JsonConfig.h"

class CJsonConfigDialog;

//////////////////////////////////////////////////////////////////////////
class CAppConfig : public CJsonConfig
{
public:
    CAppConfig();
    ~CAppConfig();

    void SaveGeometry( const QByteArray& geometry );
    QByteArray GetGeometry() const;

private:
    DECLARE_MODULE_TAG;
};

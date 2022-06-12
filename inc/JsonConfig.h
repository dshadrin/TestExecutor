#pragma once

//////////////////////////////////////////////////////////////////////////
#include "logclient/LogClient.h"
#include "Common.h"
#include <json/json.h>

class CJsonConfigDialog;

//////////////////////////////////////////////////////////////////////////
class CJsonConfig
{
    Q_OBJECT

public:
    CJsonConfig();
    ~CJsonConfig();

private:
    DECLARE_MODULE_TAG;
};

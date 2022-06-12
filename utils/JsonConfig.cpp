#include "JsonConfig.h"

//////////////////////////////////////////////////////////////////////////
// Application config structure (example)
//////////////////////////////////////////////////////////////////////////

const size_t TI_NAME = 0;
const size_t TI_MAIN_OBJ_FLAG = 1;
const size_t TI_LINK_OBJ_FLAG = 2;

//const std::tuple<std::string, uint32_t> CJsonConfig::s_rootConfigNode
//{
//    "Configuration", JO_CONST
//};
//
//const std::vector<std::tuple<std::string, uint32_t>> CJsonConfig::s_vMainConfObjects
//{
//    {KEY_CURRENT_SESSION,         JO_CONST  | JO_CURRENT_SESSION},
//    {KEY_DEFINED_SESSIONS,        JO_APPEND | JO_SESSIONS       },
//    {KEY_DEFINED_MONITORS,        JO_APPEND | JO_MONITORS       },
//    {KEY_DEFINED_LOGGERS,         JO_APPEND | JO_LOGGERS        },
//    {KEY_DEFINED_CONNECTIONS,     JO_APPEND | JO_CONNECTIONS    }
//};
//
//const std::vector<std::tuple<std::string, ETypeValue, uint32_t>> CJsonConfig::s_vCurrentSessionConfObjects
//{
//    {KEY_SELECTED_SESSION,    ETypeValue::arraj_value, JO_APPEND | JO_SESSIONS    | JO_LINKS   },
//    {KEY_SELECTED_MONITORS,   ETypeValue::arraj_value, JO_APPEND | JO_MONITORS    | JO_LINKS   },
//    {KEY_SELECTED_LOGGER,     ETypeValue::arraj_value, JO_APPEND | JO_LOGGERS     | JO_LINKS   },
//    {KEY_SELECTED_CONNECTION, ETypeValue::arraj_value, JO_APPEND | JO_CONNECTIONS | JO_LINKS}
//};
//
//const VectorValues g_loggerTemplate
//{
//    { "name",                "TestExecutor log", "STRING"  },
//    { "host",                "localhost",        "STRING"  },
//    { "port",                "2100",             "INTEGER" },
//    { "retry",               "5",                "INTEGER" },
//    { "max_message_size",    "2048",             "INTEGER" },
//    { "module_tag_size",     "4",                "INTEGER" },
//    { "severity",            "DEBUG",            "STRING"  },
//    { "bg-color",            "lightGray",        "STRING"  },
//    { "text-color",          "darkBlue",         "STRING"  },
//    { "font-name",           "Courier New",      "STRING"  },
//    { "font-weight",         "10",               "INTEGER" }
//};
//
//const VectorValues g_monitorTemplate
//{
//    { "name",                "RTOS camera monitor", "STRING"  },
//    { "connection-string",   "localhost:2002",      "STRING"  },
//    { "font-name",           "Lucida Console",      "STRING"  },
//    { "font-weight",         "9",                   "INTEGER" },
//    { "bg-color",            "lightGray",           "STRING"  },
//    { "cmd-text-color",      "darkBlue",            "STRING"  },
//    { "camera-text-color",   "darkBlue",            "STRING"  }
//};
//
//const VectorValues g_connectionTemplate
//{
//    { "name",                "Local environment execution", "STRING"  },
//    { "is-remote-location",  "false",                       "BOOLEAN" },
//    { "host",                "",                            "STRING"  },
//    { "port",                "22",                          "INTEGER" },
//    { "password",            "",                            "STRING"  },
//    { "private-key-file",    "",                            "STRING"  }
//};
//
//const VectorValues g_sessionTemplate
//{
//    { "execution-file",      "", "STRING" },
//    { "command-line",        "", "STRING" },
//    { "environment",         "[]", "ARRAY"}
//};
//
//const VectorValues g_emptyTemplate {};
//
//const size_t TI_FLAG = 0;
//const size_t TI_UNIT_NAME = 1;
//const size_t TI_MULTIPLE = 2;
//const size_t TI_TEMPLATE = 3;
//
//const std::vector<std::tuple<uint32_t, std::string, bool, const VectorValues&>> CJsonConfig::s_vFlagToUnit
//{
//    {JO_SESSIONS,    "Session",     false, g_sessionTemplate   },
//    {JO_MONITORS,    "Monitor",     true,  g_monitorTemplate   },
//    {JO_LOGGERS,     "Logger",      false, g_loggerTemplate    },
//    {JO_CONNECTIONS, "Connection",  false, g_connectionTemplate}
//};
//
//const std::vector<SValueType> g_mapStrToKind
//{
//    {"BOOLEAN",     Json::ValueType::booleanValue, ETypeValue::boolean_value},
//    {"INTEGER",     Json::ValueType::intValue,     ETypeValue::integer_number},
//    {"UNSIGNED",    Json::ValueType::uintValue,    ETypeValue::unsigned_number},
//    {"DOUBLE",      Json::ValueType::realValue,    ETypeValue::float_number},
//    {"STRING",      Json::ValueType::stringValue,  ETypeValue::string_value},
//    {"HEXSTRING",   Json::ValueType::stringValue,  ETypeValue::hex_number},
//    {"ARRAY",       Json::ValueType::arrayValue,   ETypeValue::arraj_value},
//    {"OBJECT",      Json::ValueType::objectValue,  ETypeValue::object_value},
//    {"LINK",        Json::ValueType::stringValue,  ETypeValue::link}
//};

//////////////////////////////////////////////////////////////////////////
CJsonConfig::CJsonConfig()
{

}

CJsonConfig::~CJsonConfig()
{

}

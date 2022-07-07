#include "Common.h"
#include "utils/JsonConfig.h"
#include "utils/Exceptions.h"
#include <QFileDialog>
#include <QRegularExpression>
#include <QList>

//////////////////////////////////////////////////////////////////////////
namespace
{
static const std::vector<QString> g_boolFalseValus
{
    "false",
    "manual",
    "off"
};

static const std::vector<QString> g_boolTrueValus
{
    "true",
    "auto",
    "on"
};

bool IsFalseTextValue( const QString& text )
{
    return std::find( g_boolFalseValus.cbegin(), g_boolFalseValus.cend(), text.toLower() ) != g_boolFalseValus.cend();
}

bool IsTrueTextValue( const QString& text )
{
    return std::find( g_boolTrueValus.cbegin(), g_boolTrueValus.cend(), text.toLower() ) != g_boolTrueValus.cend();
}

bool IsTextFloatNumber( const QString& text )
{
    QRegularExpression expr { R"(^[+-]{0,1}\d+\.{0,1}\d*([eE]{1}[+-]{0,1}\d+){0,1}$)" };
    QRegularExpressionMatch match = expr.match( text );
    return match.captured( 0 ) == text;
}

bool IsTextHexNumber( const QString& text )
{
    QRegularExpression expr { R"(^[[:xdigit:]]+$)" };
    QRegularExpressionMatch match = expr.match( text );
    return match.captured( 0 ) == text;
}

bool IsTextUnsignedInteger( const QString& text )
{
    QRegularExpression expr { R"(^+{0,1}[[:digit:]]+$)" };
    QRegularExpressionMatch match = expr.match( text );
    return match.captured( 0 ) == text;
}

bool IsTextInteger( const QString& text )
{
    QRegularExpression expr { R"(^[+-]{0,1}[[:digit:]]+$)" };
    QRegularExpressionMatch match = expr.match( text );
    return match.captured( 0 ) == text;
}

bool IsTextJsonArray( const QString& text )
{
    Json::Value value;
    std::istringstream iss(text.toStdString());
    std::string errs;
    bool status = CJsonConfig::ReadFromStream(iss, value, errs);
    if (status && !value.isArray())
    {
        status = false;
    }
    return status;
}

bool IsTextJsonObject( const QString& text )
{
    Json::Value value;
    std::istringstream iss(text.toStdString());
    std::string errs;
    bool status = CJsonConfig::ReadFromStream(iss, value, errs);
    if (status && !value.isObject())
    {
        status = false;
    }
    return status;
}
}

//////////////////////////////////////////////////////////////////////////
namespace util
{
    QString FindFile( const QString& path, const QString& title, const QString& filter)
    {
        QString currPath = path.isEmpty() ? QDir::currentPath() : path;
        QString filename = QFileDialog::getOpenFileName(Q_NULLPTR, title, currPath, filter);
        if (filename.isNull())
        {
            filename = "";
        }
        return filename;
    }

    QString FindFolder( const QString& path, const QString& title)
    {
        QString currPath = path.isEmpty() ? QDir::currentPath() : path;
        QString dirname = QFileDialog::getExistingDirectory(Q_NULLPTR, title, currPath);
        if (dirname.isNull())
        {
            dirname = "";
        }
        return dirname;
    }
}

namespace Qt {
    BOOST_DESCRIBE_ENUM(GlobalColor,
        color0,
        color1,
        black,
        white,
        darkGray,
        gray,
        lightGray,
        red,
        green,
        blue,
        cyan,
        magenta,
        yellow,
        darkRed,
        darkGreen,
        darkBlue,
        darkCyan,
        darkMagenta,
        darkYellow,
        transparent
    )
}

Qt::GlobalColor util::ColorFromString( const std::string& color )
{
    Qt::GlobalColor val;
    bool status = boost::describe::enum_from_string<Qt::GlobalColor>(color.c_str(), val);
    if (!status)
    {
        util::dex::ThrowException<std::logic_error>( fmt::format( FMT_STRING( "Error convert to Qt::GlobalColor: {:s}" ), color ) );
    }
    return val;
}

bool util::CheckStringValue( const QString& text, ETypeValue type )
{
    bool status = true;
    switch (type)
    {
    case ETypeValue::NullValue:
        status = text.isNull();
        break;
    case ETypeValue::BooleanValue:
        status = (IsFalseTextValue(text) || IsTrueTextValue(text));
        break;
    case ETypeValue::FloatValue:
        status = IsTextFloatNumber( text );
        break;
    case ETypeValue::HexNumberString:
        status = IsTextHexNumber( text );
        break;
    case ETypeValue::IntegerValue:
        status = IsTextInteger( text );
        break;
    case ETypeValue::UnsignedValue:
        status = IsTextUnsignedInteger( text );
        break;
    case ETypeValue::ArrajValue:
        status = IsTextJsonArray( text );
        break;
    case ETypeValue::ObjectValue:
        status = IsTextJsonObject( text );
        break;
    case ETypeValue::StringValue:      // always true
    default:;
    }
    return status;
}

QList<QStringView> util::SplitString( const QString& str, QChar ch )
{
    if (!str.isNull() && !str.isEmpty())
    {
        auto tokens = QStringTokenizer { str, ch }.toContainer();
        return tokens;
    }
    return QList<QStringView>{};
}

ETypeValue util::TypeFromString(const QString& str)
{
    ETypeValue val;
    bool status = boost::describe::enum_from_string(str.toStdString().c_str(), val);
    if (!status)
    {
        util::dex::ThrowException<std::logic_error>( fmt::format( FMT_STRING( "Error convert {:s} to ETypeValue" ), str.toStdString() ) );
    }
    return val;
}

QString util::TypeToString(ETypeValue val)
{
    std::string def = "(undefined " + std::to_string(static_cast<std::underlying_type_t<ETypeValue>>(val)) + ")";
    return boost::describe::enum_to_string(val, def.c_str());
}

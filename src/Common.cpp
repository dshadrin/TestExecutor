#include "Common.h"
#include "utils/JsonConfig.h"
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

Qt::GlobalColor util::ColorFromString( const std::string& color )
{
    if (color == "color1") return Qt::color1;
    else if (color == "black") return Qt::black;
    else if (color == "white") return Qt::white;
    else if (color == "darkGray") return Qt::darkGray;
    else if (color == "gray") return Qt::gray;
    else if (color == "lightGray") return Qt::lightGray;
    else if (color == "red") return Qt::red;
    else if (color == "green") return Qt::green;
    else if (color == "blue") return Qt::blue;
    else if (color == "cyan") return Qt::cyan;
    else if (color == "magenta") return Qt::magenta;
    else if (color == "yellow") return Qt::yellow;
    else if (color == "darkRed") return Qt::darkRed;
    else if (color == "darkGreen") return Qt::darkGreen;
    else if (color == "darkBlue") return Qt::darkBlue;
    else if (color == "darkCyan") return Qt::darkCyan;
    else if (color == "darkMagenta") return Qt::darkMagenta;
    else if (color == "darkYellow") return Qt::darkYellow;
    else if (color == "transparent") return Qt::transparent;
    else return Qt::color0;
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
    if (status)
    {
        return val;
    }
    BOOST_THROW_EXCEPTION(std::logic_error("Error convert to ETypeValue: " + str.toStdString()));
}

QString util::TypeFromString(ETypeValue val)
{
    std::string def = "(undefined " + std::to_string(static_cast<std::underlying_type_t<ETypeValue>>(val)) + ")";
    return boost::describe::enum_to_string(val, def.c_str());
}

#include "Common.h"
#include "utils/JsonConfig.h"
#include "utils/Exceptions.h"
#include <QFileDialog>
#include <QList>

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
        tex::ThrowException<std::logic_error>( fmt::format( FMT_STRING( "Error convert to Qt::GlobalColor: {:s}" ), color ) );
    }
    return val;
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

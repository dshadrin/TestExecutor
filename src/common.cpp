#include "StdInc.h"
#include "common.h"
#include "Config.h"
#include <QFileDialog>


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

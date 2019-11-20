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
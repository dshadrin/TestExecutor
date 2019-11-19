#include "StdInc.h"
#include "common.h"
#include "Config.h"
#include <QFileDialog>


namespace util
{
    QString FindFile(CConfig* config, const QString& title, const QString& filter)
    {
        QString currPath = QDir::currentPath();
        if (config)
        {
            //
        }

        QString filename = QFileDialog::getOpenFileName(Q_NULLPTR, title, currPath, filter);
        if (filename.isNull())
        {
            filename = "";
        }

        if (config && !filename.isEmpty())
        {
            //
        }

        return filename;
    }

    QString FindFolder(CConfig* config, const QString& title)
    {
        QString currPath = QDir::currentPath();
        if (config)
        {
            //
        }

        QString dirname = QFileDialog::getExistingDirectory(Q_NULLPTR, title, currPath);
        if (dirname.isNull())
        {
            dirname = "";
        }

        if (config && !dirname.isEmpty())
        {
            //
        }

        return dirname;
    }
}
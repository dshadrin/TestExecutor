/*
* Copyright (C) 2019 Dmitriy Shadrin
* All rights reserved.
*/

#pragma once
#include <QString>

#ifdef WIN32
const char LIST_PATH_SEPARATOR = ';';
#else
const char LIST_PATH_SEPARATOR = ':';
#endif

//////////////////////////////////////////////////////////////////////////
namespace util
{
    QString FindFile(const QString& path = "", const QString& title = QString("Find file"), const QString& filter = QString("All files(*.*)"));
    QString FindFolder(const QString& path = "", const QString& title = QString("Select a Directory"));
    Qt::GlobalColor ColorFromString( const std::string& color );
}

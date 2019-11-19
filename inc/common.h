/*
* Copyright (C) 2019 Dmitriy Shadrin
* All rights reserved.
*/

#pragma once
#include <QString>

class CConfig;

//////////////////////////////////////////////////////////////////////////
namespace util
{
    QString FindFile(CConfig* config = Q_NULLPTR, const QString& title = QString("Find file"), const QString& filter = QString("All files(*.*)"));
    QString FindFolder(CConfig* config = Q_NULLPTR, const QString& title = QString("Select a Directory"));
}

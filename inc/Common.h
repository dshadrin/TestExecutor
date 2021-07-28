/*
* Copyright (C) 2019 Dmitriy Shadrin
* All rights reserved.
*/

#pragma once
#include <QString>
#include <QObject>

#ifdef WIN32
const char LIST_PATH_SEPARATOR = ';';
#else
const char LIST_PATH_SEPARATOR = ':';
#endif

//////////////////////////////////////////////////////////////////////////
enum class ETypeValue
{
    null,
    boolean_value,
    integer_number,
    unsigned_number,
    float_number,
    string_value,
    hex_number,
    arraj_value,    // json format array (check with json parser)
    object_value,   // json format object (check with json parser)
    link            // full path to linked object
};

struct SValueView
{
    QString name;
    QString value;
    QString type;
};

//////////////////////////////////////////////////////////////////////////
namespace util
{
    QString FindFile(const QString& path = "", const QString& title = QString("Find file"), const QString& filter = QString("All files(*)"));
    QString FindFolder(const QString& path = "", const QString& title = QString("Select a Directory"));
    Qt::GlobalColor ColorFromString( const std::string& color );
    bool CheckStringValue( const QString& text, ETypeValue type );
    QList<QStringView> SplitString( const QString& str, QChar ch );
}

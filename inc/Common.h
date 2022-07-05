/*
* Copyright (C) 2019 Dmitriy Shadrin
* All rights reserved.
*/

#pragma once
#include <QString>
#include <QObject>
#include <boost/describe.hpp>

#ifdef WIN32
const char LIST_PATH_SEPARATOR = ';';
#else
const char LIST_PATH_SEPARATOR = ':';
#endif

//////////////////////////////////////////////////////////////////////////
BOOST_DEFINE_ENUM_CLASS(ETypeValue,
    NullValue,
    BooleanValue,
    IntegerValue,
    UnsignedValue,
    FloatValue,
    StringValue,
    HexNumberString,    /** number as hex string */
    ArrajValue,         /** json format array (check with json parser) */
    ObjectValue,        /** json format object (check with json parser) */
    LinkValue           /** full path to linked object */
)

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

    ETypeValue TypeFromString(const QString& str);
    QString TypeFromString(ETypeValue val);
}

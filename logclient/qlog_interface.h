#pragma once

#include <QObject>

//////////////////////////////////////////////////////////////////////////
class QLogClient : public QObject
{
    Q_OBJECT

Q_SIGNALS:
    void PrintLog( const std::string& log );
};

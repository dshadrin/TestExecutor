#pragma once
#include <QPlainTextEdit>
#include "JsonConfig.h"

class CLogger : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit CLogger( const CValueViewAdapter& props, QWidget *parent = 0);
    ~CLogger();

protected:
    void scrollDown();
    void mousePressEvent( QMouseEvent* );
    void mouseDoubleClickEvent( QMouseEvent* ) {}
    void contextMenuEvent( QContextMenuEvent* ) {}

protected Q_SLOTS:
    void printData( const std::string& );

private:
    QTextCharFormat m_charFormat;
};

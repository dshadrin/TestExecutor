#pragma once
#include "StdInc.h"
#include <QPlainTextEdit>
#include <boost/property_tree/ptree.hpp>

class CLogger : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit CLogger( const boost::property_tree::ptree& pt, QWidget *parent = 0);
    ~CLogger();

protected:
    void scrollDown();
    void mousePressEvent( QMouseEvent* );
    void mouseDoubleClickEvent( QMouseEvent* ) {}
    void contextMenuEvent( QContextMenuEvent* ) {}

protected Q_SLOTS:
    void printData( const std::string& );
    static Qt::GlobalColor ColorFromString( const std::string& color );

private:
    Qt::GlobalColor m_bgColor;
    Qt::GlobalColor m_fgColor;
};

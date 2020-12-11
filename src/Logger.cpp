#include "Logger.h"
#include "logclient/logclient.h"
#include <QScrollBar>

CLogger::CLogger( const boost::property_tree::ptree& pt, QWidget *parent ) :
	QPlainTextEdit(parent),
    m_bgColor( ColorFromString( pt.get<std::string>( "bg-color", "black" ) ) ),
    m_fgColor( ColorFromString( pt.get<std::string>( "text-color", "green" ) ) )
{
    QPalette p = palette();
    p.setColor( QPalette::Base, m_bgColor );
    p.setColor(QPalette::Text, m_fgColor );
    setPalette(p);

    QObject::connect( CLogClient::Get( pt ), &CLogClient::PrintLog, this, &CLogger::printData );
}

CLogger::~CLogger()
{

}

void CLogger::printData( const std::string& s)
{
    auto tc = textCursor();
    tc.movePosition( QTextCursor::End );
    setTextCursor( tc );

    textCursor().insertBlock();
    QTextCharFormat format;
    format.setForeground( m_fgColor );
    format.setFont( QFont( "Lucida Console", 8 ) );
    textCursor().setBlockCharFormat(format);
    textCursor().insertText( QString::fromStdString( s ) );
    scrollDown();
}

Qt::GlobalColor CLogger::ColorFromString( const std::string& color )
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

void CLogger::scrollDown()
{
    QScrollBar *vbar = verticalScrollBar();
    vbar->setValue(vbar->maximum());
}

void CLogger::mousePressEvent( QMouseEvent* )
{
    setFocus();
}

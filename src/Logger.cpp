#include "Logger.h"
#include "common.h"
#include "logclient/logclient.h"
#include <QScrollBar>

CLogger::CLogger( const boost::property_tree::ptree& pt, QWidget *parent ) :
	QPlainTextEdit(parent)
{
    QPalette p = palette();
    p.setColor( QPalette::Base, util::ColorFromString( pt.get<std::string>( "bg-color", "black" ) ) );
    p.setColor(QPalette::Text, util::ColorFromString( pt.get<std::string>( "text-color", "green" ) ) );
    setPalette(p);
    setLineWrapMode( QPlainTextEdit::WidgetWidth );

    m_charFormat.setForeground( util::ColorFromString( pt.get<std::string>( "text-color", "green" ) ) );
    m_charFormat.setFont( QFont( QString::fromStdString( pt.get<std::string>( "font-name", "Courier New" ) ), pt.get<int>( "font-weight", 12 ) ) );

    QObject::connect( CLogClient::Get( pt ), &CLogClient::PrintLog, this, &CLogger::printData );
}

CLogger::~CLogger()
{

}

void CLogger::printData( const std::string& s)
{
    textCursor().insertBlock();
    textCursor().setBlockCharFormat( m_charFormat );
    QTextBlockFormat block = textCursor().blockFormat();
    block.setLineHeight( 0.0f, QTextBlockFormat::MinimumHeight );
    textCursor().setBlockFormat( block );
    textCursor().insertText( QString::fromStdString( s ) );
    scrollDown();
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

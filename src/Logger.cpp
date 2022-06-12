#include "Logger.h"
#include "Common.h"
#include "logclient/LogClient.h"
#include <QScrollBar>
#include <boost/property_tree/ptree.hpp>

CLogger::CLogger( const CValueViewAdapter& props, QWidget *parent ) :
	QPlainTextEdit(parent)
{
    QPalette p = palette();
    //p.setColor( QPalette::Base, util::ColorFromString( props.get<std::string>( "bg-color" ) ) );
    //p.setColor( QPalette::Text, util::ColorFromString( props.get<std::string>( "text-color" ) ) );
    setPalette( p );
    setLineWrapMode( QPlainTextEdit::WidgetWidth );

    //m_charFormat.setForeground( util::ColorFromString( props.get<std::string>( "text-color" ) ) );
    //m_charFormat.setFont( QFont( props.get<QString>( "font-name" ), props.get<int>( "font-weight" ) ) );
    QObject::connect( CLogClient::Get(), &CLogClient::PrintLog, this, &CLogger::printData );
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

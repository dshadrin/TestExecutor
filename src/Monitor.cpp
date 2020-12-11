#include "StdInc.h"
#include "Monitor.h"
#include <QScrollBar>
#include <boost/asio.hpp>

#define IN_BUFFER_SIZE 512
IMPLEMENT_MODULE_TAG( CMonitor, "MON1" );

CMonitor::CMonitor( boost::asio::io_context& ioCtx, const boost::property_tree::ptree& pt, QWidget *parent ) :
	QPlainTextEdit(parent),
    m_ioCtx(ioCtx),
    m_socket(m_ioCtx),
    m_inBuffSize(IN_BUFFER_SIZE),
    m_inBuffer(m_inBuffSize),
    m_isLocked(false),
    m_historyPos(0),
    m_bgColor( ColorFromString( pt.get<std::string>( "bg-color", "black" ) ) ),
    m_fgColor( ColorFromString( pt.get<std::string>( "cmd-text-color", "green" ) ) ),
    m_camColor( ColorFromString( pt.get<std::string>( "camera-text-color", "white" ) ) ),
    m_name( pt.get<std::string>( "name", "Camera monitor" ) )
{
    QPalette p = palette();
    p.setColor( QPalette::Base, m_bgColor );
    p.setColor(QPalette::Text, m_fgColor );
    setPalette(p);

    std::string connStr = pt.get<std::string>( "connection-string", "localhost:2002" );
    if (!connStr.empty())
    {
        static rgx::regex expr{ R"(^(.+):(\d+)$)" };
        rgx::smatch result;
        if (rgx::regex_search( connStr, result, expr ))
        {
            m_host = result[1];
            m_port = std::stoi( result[2] );
            QObject::connect( this, &CMonitor::onData, this, &CMonitor::printData );
            Connect();
        }
    }
}

CMonitor::~CMonitor()
{
    Stop();
}

void CMonitor::printData(QString s, Qt::GlobalColor txtColor)
{
    auto tc = textCursor();
    tc.movePosition( QTextCursor::End );
    setTextCursor( tc );

    textCursor().insertBlock();
    QTextCharFormat format;
    format.setForeground( txtColor );
    format.setFont( QFont( "Lucida Console", 8 ) );
    textCursor().setBlockCharFormat(format);
    textCursor().insertText(s);
    scrollDown();
}


void CMonitor::Stop()
{
    if (m_socket.is_open())
    {
        LOG_WARN << m_name << " close";
        QObject::disconnect( this, &CMonitor::onData, this, &CMonitor::printData );
        m_socket.shutdown( boost::asio::socket_base::shutdown_both );
        m_socket.close();
    }
}


bool CMonitor::Connect()
{
    boost::asio::ip::tcp::resolver resolver( m_ioCtx );
    boost::asio::ip::tcp::resolver::query query( m_host, std::to_string( m_port ) );
    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve( query );
    boost::system::error_code ec;
    bool status = true;
    boost::asio::connect( m_socket, iter, ec );
    if (ec.value() != 0)
    {
        status = false;
        LOG_WARN << m_name << " connection error: " << ec.message();
    }
    else
    {
        m_socket.async_read_some( boost::asio::buffer( m_inBuffer.data(), m_inBuffSize ), std::bind( &CMonitor::ReadHandler, this, std::placeholders::_1, std::placeholders::_2 ) );
        LOG_INFO << m_name << " connected: " << m_socket.local_endpoint( ec ).address().to_string() << ":" << m_socket.local_endpoint( ec ).port() << " -> "
            << m_socket.remote_endpoint( ec ).address().to_string() << ":" << m_socket.remote_endpoint( ec ).port();
    }

    return status;
}


void CMonitor::ReadHandler( const boost::system::error_code& ec, std::size_t bytesTransferred )
{
    if (!ec)
    {
        QByteArray arr( m_inBuffer.data(), bytesTransferred );
        m_socket.async_read_some( boost::asio::buffer( m_inBuffer ), std::bind( &CMonitor::ReadHandler, this, std::placeholders::_1, std::placeholders::_2 ) );
        emit onData( arr, m_camColor );
    }
    else
    {
        Stop();
    }
}


void CMonitor::WriteHandler( const boost::system::error_code& ec, std::size_t bytesTransferred )
{
    if (!ec)
    {
        m_transferred += bytesTransferred;
        if (m_transferred < m_currentMessage.size())
        { // send reminder
            boost::asio::async_write( m_socket,
                                      boost::asio::buffer( &m_currentMessage[m_transferred], m_currentMessage.size() - m_transferred ),
                                      std::bind( &CMonitor::WriteHandler, this, std::placeholders::_1, std::placeholders::_2 ) );
        }
        else
        {
            m_currentMessage.clear();
            m_isLocked = false;
        }
    }
}

void CMonitor::onEnter()
{
    m_isLocked = true;
    QString cmd = QString::fromStdString( m_currentMessage );
    historyAdd( cmd );

    m_transferred = 0;
    m_currentMessage.append( 1, '\n' );
    boost::asio::async_write( m_socket,
                              boost::asio::buffer( m_currentMessage.data(), m_currentMessage.size() ),
                              std::bind( &CMonitor::WriteHandler, this, std::placeholders::_1, std::placeholders::_2 ) );

    emit onData( cmd, m_fgColor);
}


void CMonitor::historyAdd( QString cmd )
{
    m_history.append( cmd );
    m_historyPos = m_history.length();
}


void CMonitor::historyBack()
{
    if (!m_historyPos)
        return;
    QTextCursor cursor = textCursor();
    cursor.movePosition( QTextCursor::StartOfBlock );
    cursor.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );
    cursor.removeSelectedText();
    cursor.insertText( m_history.at( m_historyPos - 1 ) );
    m_currentMessage = m_history.at( m_historyPos - 1 ).toStdString();
    setTextCursor( cursor );
    m_historyPos--;
}


void CMonitor::historyForward()
{
    if (m_historyPos == m_history.length())
        return;
    QTextCursor cursor = textCursor();
    cursor.movePosition( QTextCursor::StartOfBlock );
    cursor.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );
    cursor.removeSelectedText();
    if (m_historyPos == m_history.length() - 1)
    {
        cursor.insertText( "" );
        m_currentMessage = "";
    }
    else
    {
        cursor.insertText( m_history.at( m_historyPos + 1 ) );
        m_currentMessage = m_history.at( m_historyPos + 1 ).toStdString();
    }
    setTextCursor( cursor );
    m_historyPos++;
}


Qt::GlobalColor CMonitor::ColorFromString( const std::string& color )
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

void CMonitor::scrollDown()
{
    QScrollBar *vbar = verticalScrollBar();
    vbar->setValue(vbar->maximum());
}


void CMonitor::keyPressEvent( QKeyEvent* event)
{
    if (m_isLocked)
    {
        return;
    }

    if (event->key() >= 0x20 && event->key() <= 0x7e
         && (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::ShiftModifier))
    {
        QPlainTextEdit::keyPressEvent( event );
        m_currentMessage.append( event->text().toStdString() );
    }

    if (event->key() == Qt::Key_Backspace
         && event->modifiers() == Qt::NoModifier
         && textCursor().positionInBlock() > 0)
    {
        QPlainTextEdit::keyPressEvent( event );
        if (!m_currentMessage.empty())
        {
            m_currentMessage.pop_back();
        }
    }

    if (event->key() == Qt::Key_Return && event->modifiers() == Qt::NoModifier)
    {
        onEnter();
    }
    if (event->key() == Qt::Key_Up && event->modifiers() == Qt::NoModifier)
    {
        historyBack();
    }
    if (event->key() == Qt::Key_Down && event->modifiers() == Qt::NoModifier)
    {
        historyForward();
    }
}

void CMonitor::mousePressEvent( QMouseEvent* )
{
    setFocus();
}
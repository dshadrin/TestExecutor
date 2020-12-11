#pragma once
#include "StdInc.h"
#include "logclient/logclient.h"
#include <QtGui>
#include <QPlainTextEdit>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/property_tree/ptree.hpp>

class CMonitor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit CMonitor( boost::asio::io_context& ioCtx, const boost::property_tree::ptree& pt, QWidget *parent = 0);
    ~CMonitor();

protected:
    void scrollDown();
    void keyPressEvent( QKeyEvent* );
    void mousePressEvent( QMouseEvent* );
    void mouseDoubleClickEvent( QMouseEvent* ) {}
    void contextMenuEvent( QContextMenuEvent* ) {}

protected Q_SLOTS:
    void printData( QString, Qt::GlobalColor );

private:
    void Stop();
    bool Connect();
    void ReadHandler( const boost::system::error_code& ec, std::size_t bytesTransferred );
    void WriteHandler( const boost::system::error_code& ec, std::size_t bytesTransferred );
    void onEnter();

    void historyAdd( QString );
    void historyBack();
    void historyForward();

private:
    boost::asio::io_context& m_ioCtx;
    boost::asio::ip::tcp::socket m_socket;
    std::string m_host;
    uint16_t m_port;
    std::vector<char>::size_type m_inBuffSize;
    std::vector<char> m_inBuffer;
    size_t m_transferred;
    std::string m_currentMessage;
    size_t m_msgSize;
    bool m_isLocked;
    QStringList m_history;
    int m_historyPos;
    Qt::GlobalColor m_bgColor;
    Qt::GlobalColor m_camColor;
    Qt::GlobalColor m_fgColor;
    std::string m_name;
    QTextCharFormat m_charFormat;
    DECLARE_MODULE_TAG;

Q_SIGNALS:
    void onData( QString, const Qt::GlobalColor );
};

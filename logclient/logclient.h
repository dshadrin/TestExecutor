#pragma once

#include "StdInc.h"
#include "utils/lockqueue.h"
#include "utils/logbuilder.h"
#include "oscar/flap_builder.h"
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree.hpp>
#include <QObject>

//////////////////////////////////////////////////////////////////////////
class CLogClient : public QObject, boost::noncopyable
{
    Q_OBJECT
public:
    CLogClient(const boost::property_tree::ptree& pt);
    ~CLogClient();

    static CLogClient* Get( const boost::property_tree::ptree& pt );
    static CLogClient* Get();
    void Stop();
    void Push(std::shared_ptr<SLogPackage> package);
    ELoggingMode LoggingMode() const { return m_loggingMode; }
    void LoggingMode(ELoggingMode mode) { m_loggingMode = mode; }

private:
    void WaitQueueEmpty();
    void ThreadLogClient();
    bool Connect();
    void WriteHandler(const boost::system::error_code &ec, std::size_t bytesTransferred);
    void WriteNextMessage(); // Attention!!! run under lock (boost::mutex::scoped_lock lock(m_mutex);)
    void CloseLogFile();
    void OpenNewLogFile(const std::string& namePrefix, const std::string& nameSuffix);

Q_SIGNALS:
    void PrintLog( const std::string& log );

private:
    boost::asio::io_service m_ioService;
    CLockQueue<std::shared_ptr<SLogPackage>> m_queue;

    sync::mutex m_mutex;
    trd::thread m_thread;
    bool m_writeInProgress;
    bool m_stopInProgress;
    ELoggingMode m_loggingMode;

    std::string m_host;
    uint16_t m_port;
    ESeverity m_thresholdSeveriry;

    boost::asio::ip::tcp::socket m_socket;
    oscar::flap_storage m_currentMessage;
    size_t m_transferred;

    static std::unique_ptr<CLogClient> s_logClientPtr;
};

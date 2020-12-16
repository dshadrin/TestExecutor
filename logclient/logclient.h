#pragma once

#include "qlog_interface.h"
#include "utils/lockqueue.h"
#include "utils/logbuilder.h"
#include "oscar/flap_builder.h"
#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree.hpp>

#if defined(__MINGW32__) || defined(__MINGW64__) || defined(__linux__)
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <thread>
#include <mutex>
#endif

//////////////////////////////////////////////////////////////////////////
class CLogClient : public QLogClient, boost::noncopyable
{
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

private:
    boost::asio::io_context m_ioService;
    CLockQueue<std::shared_ptr<SLogPackage>> m_queue;

    std::mutex m_mutex;
    std::thread m_thread;
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

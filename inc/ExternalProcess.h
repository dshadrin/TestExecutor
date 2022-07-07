#pragma once

#include <QObject>
#include <QString>
#include <boost/process.hpp>

const uint64_t DEFAULT_TIMEOUT = 30000; // 30s

//////////////////////////////////////////////////////////////////////////
class CExternalProcess : public QObject
{
    Q_OBJECT
public:
    CExternalProcess( QObject* parent = nullptr );
    virtual ~CExternalProcess();

    void Run( const std::string& envLine );
    void PutCommand( const std::string& cmd );
    void SetTimeoutMs( uint64_t durationMs ) { m_durationMs = durationMs; }
    void Stop();
    int  ExitCode() const { return m_exitCode; }

Q_SIGNALS:
    void OnString( QString );

private:
    void ThreadProcess( const std::string& envLine );
    boost::process::environment PrepareEnvironment( const std::string& envLine );
    std::string PrepareCommandLine( const std::string& exeLine, const std::string& cmdLine );

private:
    uint64_t m_durationMs;
    int m_exitCode;
    bool m_isRunning;
    bool m_isStop;
    std::unique_ptr <boost::process::child> m_process;
    boost::process::opstream m_out;
    boost::process::ipstream m_in;
    std::unique_ptr<tth::thread> m_thread;
};

#include "ExternalProcess.h"

//////////////////////////////////////////////////////////////////////////
namespace bp = boost::process;

CExternalProcess::CExternalProcess( QObject* parent ) :
    QObject( parent ),
    m_durationMs( DEFAULT_TIMEOUT ),
    m_exitCode( 0 ),
    m_isRunning( false ),
    m_isStop( false )
{
}

CExternalProcess::~CExternalProcess()
{
    Stop();
}

boost::process::environment CExternalProcess::PrepareEnvironment( const std::string& envLine )
{
    bp::environment env = boost::this_process::environment();;
    std::string envStr = ba::trim_copy_if( envLine, ba::is_any_of( "[]" ) );
    if (!envStr.empty())
    {
        std::vector<std::string> vEnv;
        ba::split( vEnv, envStr, ba::is_any_of( ",;:" ), ba::token_compress_on );
        for (std::string& s : vEnv)
        {
            size_t pos = s.find_first_of( '=' );
            if (pos != std::string::npos)
            {
                std::string name = s.substr( 0, pos );
                std::string val = s.substr( pos + 1 );
                ba::trim( name );
                ba::to_upper( name );
                ba::trim( val );
                env[name] = val;
            }
        }
    }
    return env;
}

std::string CExternalProcess::PrepareCommandLine( const std::string& exeLine, const std::string& cmdLine )
{
    std::string cmd;
    std::string exe = ba::trim_copy( exeLine );
    fs::path p( fs::absolute( exe ) );
    if (boost::filesystem::exists( p ))
    {
        exe = p.string();

        if (exe.find( ' ' ) != std::string::npos);
        {
            exe.insert( 0, 1, '"' );
            exe.append( 1, '"' );
        }
        std::string cmdStr = "cmd ";
        cmdStr.append( exe );
        cmdStr.append( 1, ' ' );
        cmdStr.append( cmdLine );

        p.remove_filename();
        fs::current_path( p );
        cmd = cmdStr;
    }
    return cmd;
}

void CExternalProcess::PutCommand( const std::string& cmd )
{
    m_out << cmd << std::endl;
}

void CExternalProcess::Run( const std::string& envLine )
{
    m_thread.reset( new trd::thread( std::bind( &CExternalProcess::ThreadProcess, this, envLine ) ) );
}

void CExternalProcess::Stop()
{
    m_isStop = true;
    PutCommand( "exit" );
    if (m_thread)
    {
        m_process->terminate();
        m_thread->join();
        m_thread.reset();
    }
}

void CExternalProcess::ThreadProcess( const std::string& envLine )
{
    try
    {
        m_isRunning = true;
        m_isStop = false;
        std::string cmd = "cmd";
        bp::environment env = PrepareEnvironment( envLine );
        std::string line;
        m_process.reset( new bp::child( cmd, env, bp::std_out > m_in, bp::std_err > m_in, bp::std_in < m_out ) );

        while (!m_isStop && m_process->running() && std::getline( m_in, line ) && !line.empty())
        {
            emit OnString( QString::fromStdString( line ) );
        }

        m_process->terminate();
        m_process->wait();
        m_exitCode = m_process->exit_code();
    }
    catch (std::exception& e)
    {
        emit OnString( QString( "Process raises exception: " ) + e.what() );
        m_exitCode = -100;
    }
    m_isRunning = false;
}

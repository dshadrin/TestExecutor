#include "SshSession.h"


//////////////////////////////////////////////////////////////////////////
tth::mutex CSession::s_mtx;
uint32_t CSession::s_counter = 0;

//////////////////////////////////////////////////////////////////////////
CSession::CSession( QObject* parent /*= nullptr */ ) : QObject(parent)
{
    std::unique_lock<tth::mutex> lock( s_mtx );
#ifdef WIN32
    if (s_counter == 0)
    {
        WSADATA wsadata;
        int err = WSAStartup( MAKEWORD( 2, 0 ), &wsadata );
        if (err != 0)
        {
            qDebug( fmt::format( FMT_STRING( "WSAStartup failed with error: {:d}\n" ), err ).c_str() );
            
        }
    }
#endif
    AddRef();
}

CSession::~CSession() noexcept
{
    std::unique_lock<tth::mutex> lock( s_mtx );
    if (s_counter == 1)
    {
#ifdef WIN32
        WSACleanup();
#endif
    }
    DecRef();
}

void CSession::AddRef() noexcept
{
    ++s_counter;
}

void CSession::DecRef() noexcept
{
    if (s_counter > 0) --s_counter;
}

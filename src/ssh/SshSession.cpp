#include "SshSession.h"


//////////////////////////////////////////////////////////////////////////
trds::mutex ssh2::CSession::s_mtx;
uint32_t ssh2::CSession::s_counter = 0;

//////////////////////////////////////////////////////////////////////////
ssh2::CSession::CSession( QObject* parent /*= nullptr */ ) : QObject(parent)
{
    trds::unique_lock<trds::mutex> lock( s_mtx );
    if (s_counter == 0)
    {
#ifdef WIN32
        WSADATA wsadata;
        int err = WSAStartup( MAKEWORD( 2, 0 ), &wsadata );
        if (err != 0)
        {
            qDebug( StringFormat( "WSAStartup failed with error: %d\n", err ).c_str() );
        }
#endif
    }
    AddRef();
}

ssh2::CSession::~CSession() noexcept
{
    trds::unique_lock<trds::mutex> lock( s_mtx );
    if (s_counter == 1)
    {
#ifdef WIN32
        WSACleanup();
#endif
    }
    DecRef();
}

void ssh2::CSession::AddRef() noexcept
{
    ++s_counter;
}

void ssh2::CSession::DecRef() noexcept
{
    if (s_counter > 0) --s_counter;
}

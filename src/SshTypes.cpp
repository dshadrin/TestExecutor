#include "SshTypes.h"


ssh2::error_code_t ssh2::ErrorcodeFromInt( int val )
{
    switch (val)
    {
#ifdef X_LIBSSH2_ERROR_CODE
#define X( Enum, Value ) case Value: return error_code_t::Enum;
        X_LIBSSH2_ERROR_CODE
#undef X
#endif
    default:
        return error_code_t::error_unknown;
    }
}

std::string ssh2::ErrorcodeToString( error_code_t err )
{
    switch (err)
    {
#ifdef X_LIBSSH2_ERROR_CODE
#define X( Enum, Value ) case error_code_t::Enum: return #Value;
        X_LIBSSH2_ERROR_CODE
#undef X
#endif
    default:
        return "ERROR_UNKNOWN_CODE";
    }
}

QString ssh2::ErrorcodeToQString( error_code_t err )
{
    switch (err)
    {
#ifdef X_LIBSSH2_ERROR_CODE
#define X( Enum, Value ) case error_code_t::Enum: return #Value;
        X_LIBSSH2_ERROR_CODE
#undef X
#endif
    default:
        return "ERROR_UNKNOWN_CODE";
    }
}

std::ostream& ssh2::operator<<( std::ostream& oss, error_code_t err )
{
    switch (err)
    {
#ifdef X_LIBSSH2_ERROR_CODE
#define X( Enum, Value ) case error_code_t::Enum: oss << #Value; break;
        X_LIBSSH2_ERROR_CODE
#undef X
#endif
    default:
        oss << "ERROR_UNKNOWN_CODE";
    }
    return oss;
}

ssh2::sftp_status_t ssh2::SftpStatusFromInt( int val )
{
    switch (val)
    {
#ifdef X_SFTP_STATUS_CODE
#define X( Enum, Value ) case Value: return sftp_status_t::Enum;
        X_SFTP_STATUS_CODE
#undef X
#endif
    default:
        return sftp_status_t::fx_unknown;
    }
}

std::string ssh2::SftpStatusToString( sftp_status_t err )
{
    switch (err)
    {
#ifdef X_SFTP_STATUS_CODE
#define X( Enum, Value ) case sftp_status_t::Enum: return #Value;
        X_SFTP_STATUS_CODE
#undef X
#endif
    default:
        return "FX_UNKNOWN";
    }
}

QString ssh2::SftpStatusToQString( sftp_status_t err )
{
    switch (err)
    {
#ifdef X_SFTP_STATUS_CODE
#define X( Enum, Value ) case sftp_status_t::Enum: return #Value;
        X_SFTP_STATUS_CODE
#undef X
#endif
    default:
        return "FX_UNKNOWN";
    }
}

std::ostream& ssh2::operator<<( std::ostream& oss, sftp_status_t err )
{
    switch (err)
    {
#ifdef X_SFTP_STATUS_CODE
#define X( Enum, Value ) case sftp_status_t::Enum: oss << #Value; break;
        X_SFTP_STATUS_CODE
#undef X
#endif
    default:
        oss << "FX_UNKNOWN";
    }
    return oss;
}

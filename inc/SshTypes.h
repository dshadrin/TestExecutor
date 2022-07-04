/*
* Copyright (C) 2019 Dmitriy Shadrin
* All rights reserved.
*/

#pragma once

#include "libssh2.h"
#include "libssh2_sftp.h"
#include <QString>

//////////////////////////////////////////////////////////////////////////
#define ERROR_UNKNOWN_CODE -127
#define X_LIBSSH2_ERROR_CODE \
    X( success, LIBSSH2_ERROR_NONE )\
    X( error_socket, LIBSSH2_ERROR_SOCKET_NONE )\
    X( error_banner_recv, LIBSSH2_ERROR_BANNER_RECV )\
    X( error_banner_send, LIBSSH2_ERROR_BANNER_SEND )\
    X( error_invalid_mac, LIBSSH2_ERROR_INVALID_MAC )\
    X( error_kex_failure, LIBSSH2_ERROR_KEX_FAILURE )\
    X( error_alloc, LIBSSH2_ERROR_ALLOC )\
    X( error_socket_send, LIBSSH2_ERROR_SOCKET_SEND )\
    X( error_key_exchange_failure, LIBSSH2_ERROR_KEY_EXCHANGE_FAILURE )\
    X( error_timeout, LIBSSH2_ERROR_TIMEOUT )\
    X( error_hostkey_init, LIBSSH2_ERROR_HOSTKEY_INIT )\
    X( error_hostkey_sign, LIBSSH2_ERROR_HOSTKEY_SIGN )\
    X( error_decrypt, LIBSSH2_ERROR_DECRYPT )\
    X( error_socket_disconnect, LIBSSH2_ERROR_SOCKET_DISCONNECT )\
    X( error_proto, LIBSSH2_ERROR_PROTO )\
    X( error_password_expired, LIBSSH2_ERROR_PASSWORD_EXPIRED )\
    X( error_file, LIBSSH2_ERROR_FILE )\
    X( error_method_none, LIBSSH2_ERROR_METHOD_NONE )\
    X( error_authentication_failed, LIBSSH2_ERROR_AUTHENTICATION_FAILED )\
    X( error_publickey_unverified, LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED )\
    X( error_channel_outoforder, LIBSSH2_ERROR_CHANNEL_OUTOFORDER )\
    X( error_channel_failure, LIBSSH2_ERROR_CHANNEL_FAILURE )\
    X( error_channel_request_denied, LIBSSH2_ERROR_CHANNEL_REQUEST_DENIED )\
    X( error_channel_unknown, LIBSSH2_ERROR_CHANNEL_UNKNOWN )\
    X( error_channel_window_exceeded, LIBSSH2_ERROR_CHANNEL_WINDOW_EXCEEDED )\
    X( error_channel_packet_exceeded, LIBSSH2_ERROR_CHANNEL_PACKET_EXCEEDED )\
    X( error_channel_closed, LIBSSH2_ERROR_CHANNEL_CLOSED )\
    X( error_channel_eof_sent, LIBSSH2_ERROR_CHANNEL_EOF_SENT )\
    X( error_scp_protocol, LIBSSH2_ERROR_SCP_PROTOCOL )\
    X( error_zlib, LIBSSH2_ERROR_ZLIB )\
    X( error_socket_timeout, LIBSSH2_ERROR_SOCKET_TIMEOUT )\
    X( error_sftp_protocol, LIBSSH2_ERROR_SFTP_PROTOCOL )\
    X( error_request_denied, LIBSSH2_ERROR_REQUEST_DENIED )\
    X( error_method_not_supported, LIBSSH2_ERROR_METHOD_NOT_SUPPORTED )\
    X( error_inval, LIBSSH2_ERROR_INVAL )\
    X( error_invalid_poll_type, LIBSSH2_ERROR_INVALID_POLL_TYPE )\
    X( error_publickey_protocol, LIBSSH2_ERROR_PUBLICKEY_PROTOCOL )\
    X( error_eagain, LIBSSH2_ERROR_EAGAIN )\
    X( error_buffer_too_small, LIBSSH2_ERROR_BUFFER_TOO_SMALL )\
    X( error_bad_use, LIBSSH2_ERROR_BAD_USE )\
    X( error_compress, LIBSSH2_ERROR_COMPRESS )\
    X( error_out_of_boundary, LIBSSH2_ERROR_OUT_OF_BOUNDARY )\
    X( error_agent_protocol, LIBSSH2_ERROR_AGENT_PROTOCOL )\
    X( error_socket_recv, LIBSSH2_ERROR_SOCKET_RECV )\
    X( error_encrypt, LIBSSH2_ERROR_ENCRYPT )\
    X( error_bad_socket, LIBSSH2_ERROR_BAD_SOCKET )\
    X( error_known_hosts, LIBSSH2_ERROR_KNOWN_HOSTS )\
    X( error_channel_window_full, LIBSSH2_ERROR_CHANNEL_WINDOW_FULL )\
    X( error_keyfile_auth_failed, LIBSSH2_ERROR_KEYFILE_AUTH_FAILED )

#define FX_UNKNOWN 127UL
#define X_SFTP_STATUS_CODE\
    X( fx_ok, LIBSSH2_FX_OK )\
    X( fx_eof, LIBSSH2_FX_EOF  )\
    X( fx_no_such_file, LIBSSH2_FX_NO_SUCH_FILE )\
    X( fx_permission_denied, LIBSSH2_FX_PERMISSION_DENIED )\
    X( fx_failure, LIBSSH2_FX_FAILURE )\
    X( fx_bad_message, LIBSSH2_FX_BAD_MESSAGE )\
    X( fx_no_connection, LIBSSH2_FX_NO_CONNECTION )\
    X( fx_connection_lost, LIBSSH2_FX_CONNECTION_LOST )\
    X( fx_op_unsupported, LIBSSH2_FX_OP_UNSUPPORTED )\
    X( fx_invalid_handle, LIBSSH2_FX_INVALID_HANDLE )\
    X( fx_no_such_path, LIBSSH2_FX_NO_SUCH_PATH )\
    X( fx_file_already_exists, LIBSSH2_FX_FILE_ALREADY_EXISTS )\
    X( fx_write_protect, LIBSSH2_FX_WRITE_PROTECT )\
    X( fx_no_media, LIBSSH2_FX_NO_MEDIA )\
    X( fx_no_space_on_filesystem, LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM )\
    X( fx_quota_exceeded, LIBSSH2_FX_QUOTA_EXCEEDED )\
    X( fx_unknown_principal, LIBSSH2_FX_UNKNOWN_PRINCIPAL )\
    X( fx_lock_conflict, LIBSSH2_FX_LOCK_CONFLICT )\
    X( fx_dir_not_empty, LIBSSH2_FX_DIR_NOT_EMPTY )\
    X( fx_not_a_directory, LIBSSH2_FX_NOT_A_DIRECTORY )\
    X( fx_invalid_filename, LIBSSH2_FX_INVALID_FILENAME )\
    X( fx_link_loop, LIBSSH2_FX_LINK_LOOP )

//////////////////////////////////////////////////////////////////////////
namespace ssh2
{
    // Error Codes (defined by libssh2)
    enum class error_code_t : int8_t
    {
#ifdef X_LIBSSH2_ERROR_CODE
#define X( Enum, Value ) Enum = Value,
        X_LIBSSH2_ERROR_CODE
#undef X
#endif
        error_unknown = ERROR_UNKNOWN_CODE
    };

    error_code_t ErrorcodeFromInt( int val );
    std::string ErrorcodeToString( error_code_t err );
    QString ErrorcodeToQString( error_code_t err );
    std::ostream& operator<<( std::ostream& oss, error_code_t err );

    // SFTP Status Codes( returned by libssh2_sftp_last_error() )
    enum class sftp_status_t : int8_t
    {
#ifdef X_SFTP_STATUS_CODE
#define X( Enum, Value ) Enum = Value,
        X_SFTP_STATUS_CODE
#undef X
#endif
        fx_unknown = FX_UNKNOWN
    };

    sftp_status_t SftpStatusFromInt( int val );
    std::string SftpStatusToString( sftp_status_t err );
    QString SftpStatusToQString( sftp_status_t err );
    std::ostream& operator<<( std::ostream& oss, sftp_status_t err );

}

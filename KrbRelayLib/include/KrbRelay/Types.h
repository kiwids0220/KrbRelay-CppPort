#pragma once

// Export/Import macros for DLL
#ifdef KRBRELAY_SHARED
    #ifdef KRBRELAY_EXPORTS
        #define KRBRELAY_API __declspec(dllexport)
    #else
        #define KRBRELAY_API __declspec(dllimport)
    #endif
#else
    #define KRBRELAY_API
#endif

#include <windows.h>
#include <cstdint>
#include <vector>
#include <memory>

namespace KrbRelay {

// Security buffer types
enum class BufferType : uint32_t {
    SECBUFFER_EMPTY = 0,
    SECBUFFER_DATA = 1,
    SECBUFFER_TOKEN = 2,
    SECBUFFER_PKG_PARAMS = 3,
    SECBUFFER_MISSING = 4,
    SECBUFFER_EXTRA = 5,
    SECBUFFER_STREAM_TRAILER = 6,
    SECBUFFER_STREAM_HEADER = 7,
    SECBUFFER_NEGOTIATION_INFO = 8,
    SECBUFFER_PADDING = 9,
    SECBUFFER_STREAM = 10,
    SECBUFFER_MECHLIST = 11,
    SECBUFFER_MECHLIST_SIGNATURE = 12,
    SECBUFFER_TARGET = 13,
    SECBUFFER_CHANNEL_BINDINGS = 14,
    SECBUFFER_CHANGE_PASS_RESPONSE = 15,
    SECBUFFER_TARGET_HOST = 16,
    SECBUFFER_ALERT = 17,
    SECBUFFER_APPLICATION_PROTOCOLS = 18
};

// Authentication levels
enum class AuthenticationLevel : uint32_t {
    RPC_C_AUTHN_LEVEL_DEFAULT = 0,
    RPC_C_AUTHN_LEVEL_NONE = 1,
    RPC_C_AUTHN_LEVEL_CONNECT = 2,
    RPC_C_AUTHN_LEVEL_CALL = 3,
    RPC_C_AUTHN_LEVEL_PKT = 4,
    RPC_C_AUTHN_LEVEL_PKT_INTEGRITY = 5,
    RPC_C_AUTHN_LEVEL_PKT_PRIVACY = 6
};

// Impersonation levels
enum class ImpersonationLevel : uint32_t {
    RPC_C_IMP_LEVEL_DEFAULT = 0,
    RPC_C_IMP_LEVEL_ANONYMOUS = 1,
    RPC_C_IMP_LEVEL_IDENTIFY = 2,
    RPC_C_IMP_LEVEL_IMPERSONATE = 3,
    RPC_C_IMP_LEVEL_DELEGATE = 4
};

// Security status codes
enum class SecurityStatus : int32_t {
    SEC_E_OK = 0,
    SEC_I_CONTINUE_NEEDED = 0x00090312,
    SEC_I_COMPLETE_NEEDED = 0x00090313,
    SEC_I_COMPLETE_AND_CONTINUE = 0x00090314,
    SEC_E_INCOMPLETE_MESSAGE = 0x80090318,
    SEC_E_INVALID_TOKEN = 0x80090308,
    SEC_E_LOGON_DENIED = 0x8009030C,
    SEC_E_NO_CREDENTIALS = 0x8009030E
};

// Context request flags
enum class ContextReqFlags : uint32_t {
    ISC_REQ_DELEGATE = 0x00000001,
    ISC_REQ_MUTUAL_AUTH = 0x00000002,
    ISC_REQ_REPLAY_DETECT = 0x00000004,
    ISC_REQ_SEQUENCE_DETECT = 0x00000008,
    ISC_REQ_CONFIDENTIALITY = 0x00000010,
    ISC_REQ_USE_SESSION_KEY = 0x00000020,
    ISC_REQ_PROMPT_FOR_CREDS = 0x00000040,
    ISC_REQ_USE_SUPPLIED_CREDS = 0x00000080,
    ISC_REQ_ALLOCATE_MEMORY = 0x00000100,
    ISC_REQ_USE_DCE_STYLE = 0x00000200,
    ISC_REQ_DATAGRAM = 0x00000400,
    ISC_REQ_CONNECTION = 0x00000800,
    ISC_REQ_CALL_LEVEL = 0x00001000,
    ISC_REQ_FRAGMENT_SUPPLIED = 0x00002000,
    ISC_REQ_EXTENDED_ERROR = 0x00004000,
    ISC_REQ_STREAM = 0x00008000,
    ISC_REQ_INTEGRITY = 0x00010000,
    ISC_REQ_IDENTIFY = 0x00020000,
    ISC_REQ_NULL_SESSION = 0x00040000
};

// Result structure for operations
struct KRBRELAY_API OperationResult {
    bool success;
    int32_t errorCode;
    std::string message;
    
    OperationResult() : success(false), errorCode(0) {}
    OperationResult(bool s, int32_t ec, const std::string& msg) 
        : success(s), errorCode(ec), message(msg) {}
};

} // namespace KrbRelay

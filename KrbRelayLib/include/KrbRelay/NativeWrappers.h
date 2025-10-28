#pragma once

#include "Types.h"
#include <string>
#include <vector>

#define SECURITY_WIN32
#include <windows.h>
#include <security.h>

namespace KrbRelay {

/**
 * @brief Wrapper for Windows native API functions
 * 
 * Provides C++ wrappers around native Windows authentication and RPC APIs
 */
class KRBRELAY_API NativeWrappers {
public:
    /**
     * @brief Acquire credentials handle for Kerberos/NTLM authentication
     * @param packageName Security package name (e.g., "Kerberos", "Negotiate", "NTLM")
     * @param credHandle Output credentials handle
     * @return Operation result
     */
    static OperationResult AcquireCredentialsHandle(
        const std::string& packageName,
        CredHandle* credHandle
    );

    /**
     * @brief Initialize security context for authentication
     * @param credHandle Credentials handle
     * @param targetName Target SPN
     * @param contextReq Context requirements flags
     * @param inputBuffer Input security buffer (optional)
     * @param contextHandle Context handle (in/out)
     * @param outputBuffer Output security buffer
     * @return Operation result
     */
    static OperationResult InitializeSecurityContext(
        CredHandle* credHandle,
        const std::string& targetName,
        uint32_t contextReq,
        SecBufferDesc* inputBuffer,
        CtxtHandle* contextHandle,
        SecBufferDesc* outputBuffer
    );

    /**
     * @brief Accept security context on the server side
     * @param credHandle Credentials handle
     * @param contextHandle Context handle (in/out)
     * @param inputBuffer Input security buffer
     * @param contextReq Context requirements
     * @param outputBuffer Output security buffer
     * @return Operation result
     */
    static OperationResult AcceptSecurityContext(
        CredHandle* credHandle,
        CtxtHandle* contextHandle,
        SecBufferDesc* inputBuffer,
        uint32_t contextReq,
        SecBufferDesc* outputBuffer
    );

    /**
     * @brief Delete security context
     * @param contextHandle Context handle to delete
     * @return Operation result
     */
    static OperationResult DeleteSecurityContext(CtxtHandle* contextHandle);

    /**
     * @brief Free credentials handle
     * @param credHandle Credentials handle to free
     * @return Operation result
     */
    static OperationResult FreeCredentialsHandle(CredHandle* credHandle);

    /**
     * @brief Query security context attributes
     * @param contextHandle Context handle
     * @param attribute Attribute to query
     * @param buffer Output buffer
     * @return Operation result
     */
    static OperationResult QueryContextAttributes(
        CtxtHandle* contextHandle,
        uint32_t attribute,
        void* buffer
    );

    /**
     * @brief Initialize COM library
     * @return Operation result
     */
    static OperationResult InitializeCOM();

    /**
     * @brief Uninitialize COM library
     */
    static void UninitializeCOM();

    /**
     * @brief Initialize COM security
     * @param authLevel Authentication level
     * @param impLevel Impersonation level
     * @return Operation result
     */
    static OperationResult InitializeCOMSecurity(
        AuthenticationLevel authLevel,
        ImpersonationLevel impLevel
    );

private:
    NativeWrappers() = delete;
    ~NativeWrappers() = delete;
};

} // namespace KrbRelay

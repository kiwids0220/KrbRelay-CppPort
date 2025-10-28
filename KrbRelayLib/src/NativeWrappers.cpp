#include "KrbRelay/NativeWrappers.h"
#include <sstream>

#pragma comment(lib, "secur32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

namespace KrbRelay {

OperationResult NativeWrappers::AcquireCredentialsHandle(
    const std::string& packageName,
    CredHandle* credHandle) {
    
    if (!credHandle) {
        return OperationResult(false, -1, "Invalid credentials handle pointer");
    }

    TimeStamp expiry;
    SECURITY_STATUS status = ::AcquireCredentialsHandleA(
        nullptr,
        const_cast<char*>(packageName.c_str()),
        SECPKG_CRED_INBOUND,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        credHandle,
        &expiry
    );

    if (status != SEC_E_OK) {
        std::ostringstream oss;
        oss << "AcquireCredentialsHandle failed with status: 0x" 
            << std::hex << status;
        return OperationResult(false, status, oss.str());
    }

    return OperationResult(true, 0, "Success");
}

OperationResult NativeWrappers::InitializeSecurityContext(
    CredHandle* credHandle,
    const std::string& targetName,
    uint32_t contextReq,
    SecBufferDesc* inputBuffer,
    CtxtHandle* contextHandle,
    SecBufferDesc* outputBuffer) {
    
    if (!credHandle || !contextHandle || !outputBuffer) {
        return OperationResult(false, -1, "Invalid parameter");
    }

    TimeStamp expiry;
    unsigned long contextAttr;
    
    SECURITY_STATUS status = ::InitializeSecurityContextA(
        credHandle,
        nullptr,
        const_cast<char*>(targetName.c_str()),
        contextReq,
        0,
        SECURITY_NATIVE_DREP,
        inputBuffer,
        0,
        contextHandle,
        outputBuffer,
        &contextAttr,
        &expiry
    );

    if (status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED) {
        std::ostringstream oss;
        oss << "InitializeSecurityContext failed with status: 0x" 
            << std::hex << status;
        return OperationResult(false, status, oss.str());
    }

    return OperationResult(true, status, "Success");
}

OperationResult NativeWrappers::AcceptSecurityContext(
    CredHandle* credHandle,
    CtxtHandle* contextHandle,
    SecBufferDesc* inputBuffer,
    uint32_t contextReq,
    SecBufferDesc* outputBuffer) {
    
    if (!credHandle || !inputBuffer || !outputBuffer) {
        return OperationResult(false, -1, "Invalid parameter");
    }

    TimeStamp expiry;
    unsigned long contextAttr;
    
    SECURITY_STATUS status = ::AcceptSecurityContext(
        credHandle,
        nullptr,
        inputBuffer,
        contextReq,
        SECURITY_NATIVE_DREP,
        contextHandle,
        outputBuffer,
        &contextAttr,
        &expiry
    );

    if (status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED) {
        std::ostringstream oss;
        oss << "AcceptSecurityContext failed with status: 0x" 
            << std::hex << status;
        return OperationResult(false, status, oss.str());
    }

    return OperationResult(true, status, "Success");
}

OperationResult NativeWrappers::DeleteSecurityContext(CtxtHandle* contextHandle) {
    if (!contextHandle) {
        return OperationResult(false, -1, "Invalid context handle");
    }

    SECURITY_STATUS status = ::DeleteSecurityContext(contextHandle);
    
    if (status != SEC_E_OK) {
        std::ostringstream oss;
        oss << "DeleteSecurityContext failed with status: 0x" 
            << std::hex << status;
        return OperationResult(false, status, oss.str());
    }

    return OperationResult(true, 0, "Success");
}

OperationResult NativeWrappers::FreeCredentialsHandle(CredHandle* credHandle) {
    if (!credHandle) {
        return OperationResult(false, -1, "Invalid credentials handle");
    }

    SECURITY_STATUS status = ::FreeCredentialsHandle(credHandle);
    
    if (status != SEC_E_OK) {
        std::ostringstream oss;
        oss << "FreeCredentialsHandle failed with status: 0x" 
            << std::hex << status;
        return OperationResult(false, status, oss.str());
    }

    return OperationResult(true, 0, "Success");
}

OperationResult NativeWrappers::QueryContextAttributes(
    CtxtHandle* contextHandle,
    uint32_t attribute,
    void* buffer) {
    
    if (!contextHandle || !buffer) {
        return OperationResult(false, -1, "Invalid parameter");
    }

    SECURITY_STATUS status = ::QueryContextAttributesA(contextHandle, attribute, buffer);
    
    if (status != SEC_E_OK) {
        std::ostringstream oss;
        oss << "QueryContextAttributes failed with status: 0x" 
            << std::hex << status;
        return OperationResult(false, status, oss.str());
    }

    return OperationResult(true, 0, "Success");
}

OperationResult NativeWrappers::InitializeCOM() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        std::ostringstream oss;
        oss << "CoInitializeEx failed with HRESULT: 0x" << std::hex << hr;
        return OperationResult(false, hr, oss.str());
    }

    return OperationResult(true, 0, "Success");
}

void NativeWrappers::UninitializeCOM() {
    CoUninitialize();
}

OperationResult NativeWrappers::InitializeCOMSecurity(
    AuthenticationLevel authLevel,
    ImpersonationLevel impLevel) {
    
    HRESULT hr = CoInitializeSecurity(
        nullptr,
        -1,
        nullptr,
        nullptr,
        static_cast<DWORD>(authLevel),
        static_cast<DWORD>(impLevel),
        nullptr,
        EOAC_NONE,
        nullptr
    );

    if (FAILED(hr)) {
        std::ostringstream oss;
        oss << "CoInitializeSecurity failed with HRESULT: 0x" << std::hex << hr;
        return OperationResult(false, hr, oss.str());
    }

    return OperationResult(true, 0, "Success");
}

} // namespace KrbRelay

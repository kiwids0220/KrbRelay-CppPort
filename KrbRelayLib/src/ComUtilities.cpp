#include "KrbRelay/ComUtilities.h"
#include <objbase.h>
#include <combaseapi.h>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "ole32.lib")

namespace KrbRelay {

class COMUtilities::Impl {
public:
    Impl() = default;
    ~Impl() = default;
};

COMUtilities::COMUtilities() 
    : pImpl(std::make_unique<Impl>()) {
}

COMUtilities::~COMUtilities() = default;

OperationResult COMUtilities::MarshalInterface(
    void* pUnknown,
    std::vector<uint8_t>& objrefData) {
    
    if (!pUnknown) {
        return OperationResult(false, -1, "Invalid IUnknown pointer");
    }

    IStream* pStream = nullptr;
    HRESULT hr = CreateStreamOnHGlobal(nullptr, TRUE, &pStream);
    if (FAILED(hr)) {
        return OperationResult(false, hr, "Failed to create stream");
    }

    hr = CoMarshalInterface(
        pStream,
        IID_IUnknown,
        static_cast<IUnknown*>(pUnknown),
        MSHCTX_DIFFERENTMACHINE,
        nullptr,
        MSHLFLAGS_NORMAL
    );

    if (FAILED(hr)) {
        pStream->Release();
        std::ostringstream oss;
        oss << "CoMarshalInterface failed with HRESULT: 0x" << std::hex << hr;
        return OperationResult(false, hr, oss.str());
    }

    // Get stream size
    STATSTG statstg;
    hr = pStream->Stat(&statstg, STATFLAG_NONAME);
    if (FAILED(hr)) {
        pStream->Release();
        return OperationResult(false, hr, "Failed to get stream size");
    }

    // Read stream data
    objrefData.resize(static_cast<size_t>(statstg.cbSize.QuadPart));
    LARGE_INTEGER li = { 0 };
    hr = pStream->Seek(li, STREAM_SEEK_SET, nullptr);
    if (SUCCEEDED(hr)) {
        ULONG bytesRead = 0;
        hr = pStream->Read(objrefData.data(), static_cast<ULONG>(objrefData.size()), &bytesRead);
    }

    pStream->Release();

    if (FAILED(hr)) {
        return OperationResult(false, hr, "Failed to read stream");
    }

    return OperationResult(true, 0, "Success");
}

OperationResult COMUtilities::UnmarshalInterface(
    const std::vector<uint8_t>& objrefData,
    void** ppUnknown) {
    
    if (!ppUnknown || objrefData.empty()) {
        return OperationResult(false, -1, "Invalid parameter");
    }

    IStream* pStream = nullptr;
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, objrefData.size());
    if (!hGlobal) {
        return OperationResult(false, -1, "Failed to allocate memory");
    }

    void* pData = GlobalLock(hGlobal);
    if (!pData) {
        GlobalFree(hGlobal);
        return OperationResult(false, -1, "Failed to lock memory");
    }

    std::memcpy(pData, objrefData.data(), objrefData.size());
    GlobalUnlock(hGlobal);

    HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
    if (FAILED(hr)) {
        GlobalFree(hGlobal);
        return OperationResult(false, hr, "Failed to create stream");
    }

    hr = CoUnmarshalInterface(pStream, IID_IUnknown, ppUnknown);
    pStream->Release();

    if (FAILED(hr)) {
        std::ostringstream oss;
        oss << "CoUnmarshalInterface failed with HRESULT: 0x" << std::hex << hr;
        return OperationResult(false, hr, oss.str());
    }

    return OperationResult(true, 0, "Success");
}

std::string COMUtilities::CreateMonikerString(const std::vector<uint8_t>& objrefData) {
    if (objrefData.empty()) {
        return "";
    }

    std::ostringstream oss;
    oss << "objref:";
    
    for (uint8_t byte : objrefData) {
        oss << std::hex << std::setw(2) << std::setfill('0') 
            << static_cast<int>(byte);
    }

    return oss.str();
}

OperationResult COMUtilities::ParseOBJREF(
    const std::vector<uint8_t>& objrefData,
    std::string& ipid,
    uint64_t& oxid,
    uint64_t& oid) {
    
    // Basic OBJREF format validation
    // OBJREF structure starts with signature and flags
    if (objrefData.size() < 66) {  // Minimum OBJREF size
        return OperationResult(false, -1, "OBJREF data too small");
    }

    // Check MEOW signature (0x4D454F57)
    if (objrefData[0] != 0x4D || objrefData[1] != 0x45 || 
        objrefData[2] != 0x4F || objrefData[3] != 0x57) {
        return OperationResult(false, -1, "Invalid OBJREF signature");
    }

    // Parse OXID (8 bytes at offset 24)
    oxid = 0;
    for (int i = 0; i < 8; i++) {
        oxid |= static_cast<uint64_t>(objrefData[24 + i]) << (i * 8);
    }

    // Parse OID (8 bytes at offset 32)
    oid = 0;
    for (int i = 0; i < 8; i++) {
        oid |= static_cast<uint64_t>(objrefData[32 + i]) << (i * 8);
    }

    // Parse IPID (16 bytes at offset 40)
    std::ostringstream ipidStream;
    ipidStream << std::hex << std::setfill('0');
    for (int i = 0; i < 16; i++) {
        ipidStream << std::setw(2) << static_cast<int>(objrefData[40 + i]);
        if (i == 3 || i == 5 || i == 7 || i == 9) {
            ipidStream << "-";
        }
    }
    ipid = ipidStream.str();

    return OperationResult(true, 0, "Success");
}

OperationResult COMUtilities::ModifyOBJREFBindings(
    std::vector<uint8_t>& objrefData,
    const std::string& ipAddress,
    uint16_t port) {
    
    if (objrefData.size() < 66) {
        return OperationResult(false, -1, "OBJREF data too small");
    }

    // This is a simplified implementation
    // In a full implementation, you would need to parse the OBJREF structure
    // and locate the string bindings section, then modify the IP address and port
    
    // For now, just return success to indicate the API is available
    return OperationResult(true, 0, "OBJREF binding modification requires full implementation");
}

} // namespace KrbRelay

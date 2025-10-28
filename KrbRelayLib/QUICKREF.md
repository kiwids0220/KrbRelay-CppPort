# KrbRelay C++ Library - Quick Reference

## Installation

```bash
# Build as DLL
cmake -B build -DBUILD_SHARED_LIBS=ON
cmake --build build --config Release
cmake --install build --prefix C:/KrbRelay

# Build as Static Library  
cmake -B build -DBUILD_SHARED_LIBS=OFF
cmake --build build --config Release
```

## Integration

### CMakeLists.txt
```cmake
find_package(KrbRelay REQUIRED)
target_link_libraries(YourApp PRIVATE KrbRelay::KrbRelay)
```

## Core Classes

### KerberosRelay - Main relay class
```cpp
#include <KrbRelay/KerberosRelay.h>

KrbRelay::KerberosRelay relay;
relay.Configure("ldap/dc01.contoso.com", false);
relay.SetUserContext("CONTOSO", "Administrator");
relay.SetAuthenticationCallback([](auto& ticket, bool isKerberos) {
    // Handle intercepted ticket
    return true;
});
```

### SecurityBufferManager - Buffer management
```cpp
#include <KrbRelay/SecurityBuffer.h>

KrbRelay::SecurityBufferManager buffer;
std::vector<uint8_t> data = {0x60, 0x82, 0x01, 0x23};
buffer.CreateBuffer(data, KrbRelay::BufferType::SECBUFFER_TOKEN);

// Get for Windows APIs
SecBuffer* pBuf = buffer.GetSecBuffer();
```

### NativeWrappers - Windows API wrappers
```cpp
#include <KrbRelay/NativeWrappers.h>

// Initialize COM
KrbRelay::NativeWrappers::InitializeCOM();

// Acquire credentials
CredHandle cred;
auto result = KrbRelay::NativeWrappers::AcquireCredentialsHandle(
    "Kerberos", &cred);

if (result.success) {
    // Use credentials...
    KrbRelay::NativeWrappers::FreeCredentialsHandle(&cred);
}

// Cleanup
KrbRelay::NativeWrappers::UninitializeCOM();
```

### COMUtilities - COM marshalling
```cpp
#include <KrbRelay/ComUtilities.h>

KrbRelay::COMUtilities comUtil;

// Marshal interface
std::vector<uint8_t> objrefData;
comUtil.MarshalInterface(pUnknown, objrefData);

// Parse OBJREF
std::string ipid;
uint64_t oxid, oid;
comUtil.ParseOBJREF(objrefData, ipid, oxid, oid);

// Create moniker
std::string moniker = comUtil.CreateMonikerString(objrefData);
```

## Helper Functions

```cpp
#include <KrbRelay/KerberosRelay.h>

// Parse SPN
std::string service, hostname;
KrbRelay::RelayHelpers::ParseSPN("ldap/dc.contoso.com", service, hostname);

// Hex conversion
auto hex = KrbRelay::RelayHelpers::BytesToHexString(bytes);
auto bytes = KrbRelay::RelayHelpers::HexStringToBytes("deadbeef");

// Pattern search
int offset = KrbRelay::RelayHelpers::FindPattern(data, pattern);

// Validate Kerberos ticket
bool valid = KrbRelay::KerberosRelay::ValidateApReq(ticket);
```

## Common Types

### BufferType
```cpp
KrbRelay::BufferType::SECBUFFER_EMPTY
KrbRelay::BufferType::SECBUFFER_DATA
KrbRelay::BufferType::SECBUFFER_TOKEN
KrbRelay::BufferType::SECBUFFER_PKG_PARAMS
```

### AuthenticationLevel
```cpp
KrbRelay::AuthenticationLevel::RPC_C_AUTHN_LEVEL_DEFAULT
KrbRelay::AuthenticationLevel::RPC_C_AUTHN_LEVEL_NONE
KrbRelay::AuthenticationLevel::RPC_C_AUTHN_LEVEL_CONNECT
KrbRelay::AuthenticationLevel::RPC_C_AUTHN_LEVEL_PKT_INTEGRITY
KrbRelay::AuthenticationLevel::RPC_C_AUTHN_LEVEL_PKT_PRIVACY
```

### ImpersonationLevel
```cpp
KrbRelay::ImpersonationLevel::RPC_C_IMP_LEVEL_DEFAULT
KrbRelay::ImpersonationLevel::RPC_C_IMP_LEVEL_ANONYMOUS
KrbRelay::ImpersonationLevel::RPC_C_IMP_LEVEL_IDENTIFY
KrbRelay::ImpersonationLevel::RPC_C_IMP_LEVEL_IMPERSONATE
KrbRelay::ImpersonationLevel::RPC_C_IMP_LEVEL_DELEGATE
```

### OperationResult
```cpp
struct OperationResult {
    bool success;        // true if operation succeeded
    int32_t errorCode;   // Windows error code or 0
    std::string message; // Human-readable message
};

// Usage
auto result = someOperation();
if (!result.success) {
    std::cerr << "Error: " << result.message 
              << " (0x" << std::hex << result.errorCode << ")" << std::endl;
}
```

## Complete Example

```cpp
#include <KrbRelay/KerberosRelay.h>
#include <KrbRelay/SecurityBuffer.h>
#include <KrbRelay/NativeWrappers.h>
#include <iostream>

int main() {
    // 1. Initialize COM
    auto result = KrbRelay::NativeWrappers::InitializeCOM();
    if (!result.success) {
        std::cerr << "COM init failed: " << result.message << std::endl;
        return 1;
    }

    // 2. Configure relay
    KrbRelay::KerberosRelay relay;
    result = relay.Configure("ldap/dc01.contoso.com", false);
    if (!result.success) {
        std::cerr << "Config failed: " << result.message << std::endl;
        return 1;
    }

    relay.SetUserContext("CONTOSO", "Administrator");

    // 3. Set callback for intercepted tickets
    relay.SetAuthenticationCallback([](const std::vector<uint8_t>& ticket, 
                                        bool isKerberos) {
        std::cout << "Intercepted " << (isKerberos ? "Kerberos" : "NTLM") 
                  << " ticket: " << ticket.size() << " bytes" << std::endl;
        return true;
    });

    // 4. Work with security buffers
    KrbRelay::SecurityBufferManager buffer;
    std::vector<uint8_t> ticketData = {0x60, 0x82, 0x01, 0x23};
    buffer.CreateBuffer(ticketData, KrbRelay::BufferType::SECBUFFER_TOKEN);

    // 5. Validate ticket
    if (KrbRelay::KerberosRelay::ValidateApReq(ticketData)) {
        std::cout << "Valid Kerberos AP-REQ" << std::endl;
    }

    // 6. Parse SPN
    std::string service, hostname;
    if (KrbRelay::RelayHelpers::ParseSPN(relay.GetSPN(), service, hostname)) {
        std::cout << "Service: " << service << std::endl;
        std::cout << "Host: " << hostname << std::endl;
    }

    // 7. Cleanup
    KrbRelay::NativeWrappers::UninitializeCOM();
    
    std::cout << "Done!" << std::endl;
    return 0;
}
```

## Error Handling Pattern

```cpp
// Pattern 1: Check result
auto result = SomeOperation();
if (!result.success) {
    // Handle error
    return;
}

// Pattern 2: Early return
if (!result.success) {
    std::cerr << result.message << std::endl;
    return 1;
}

// Pattern 3: Throw exception (if desired)
if (!result.success) {
    throw std::runtime_error(result.message);
}
```

## Building Your Application

### Using DLL
```cmake
find_package(KrbRelay REQUIRED)
add_executable(MyApp main.cpp)
target_link_libraries(MyApp PRIVATE KrbRelay::KrbRelay)

# Copy DLL to output directory
add_custom_command(TARGET MyApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:KrbRelay>
    $<TARGET_FILE_DIR:MyApp>
)
```

### Using Static Library
```cmake
find_package(KrbRelay REQUIRED)
add_executable(MyApp main.cpp)
target_link_libraries(MyApp PRIVATE KrbRelay::KrbRelay)
target_compile_definitions(MyApp PRIVATE KRBRELAY_STATIC)
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Unresolved externals with static lib | Add `KRBRELAY_STATIC` definition |
| DLL not found at runtime | Copy DLL to exe directory or add to PATH |
| COM already initialized | Check for `RPC_E_CHANGED_MODE` - usually okay |
| Link errors | Ensure all Windows libs are linked (see CMakeLists.txt) |

## Resources

- [README.md](README.md) - Overview and features
- [INTEGRATION.md](INTEGRATION.md) - Detailed integration guide
- [ARCHITECTURE.md](ARCHITECTURE.md) - Design and architecture
- [examples/](examples/) - Example programs
- [include/KrbRelay/](include/KrbRelay/) - API headers

## License & Credits

Based on the original C# KrbRelay project by @Cube0x0.
See main repository README for full acknowledgements.

# KrbRelay C++ Library - Integration Guide

This guide shows how to integrate the KrbRelay C++ library into your projects.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Building and Installing](#building-and-installing)
3. [Integration Methods](#integration-methods)
4. [Advanced Usage](#advanced-usage)
5. [Examples](#examples)

## Quick Start

### Prerequisites

- Windows 10/11 or Windows Server 2016+
- Visual Studio 2017 or later with C++17 support
- CMake 3.15 or higher

### 5-Minute Setup

1. Clone and build:
```bash
git clone <repository-url>
cd KrbRelayLib
mkdir build && cd build
cmake .. -DBUILD_SHARED_LIBS=ON
cmake --build . --config Release
```

2. Run example:
```bash
.\bin\Release\basic_example.exe
```

## Building and Installing

### Build Options

#### As Shared Library (DLL)
```bash
cmake -B build -DBUILD_SHARED_LIBS=ON
cmake --build build --config Release
```

Output: `KrbRelay.dll` and `KrbRelay.lib` (import library)

#### As Static Library
```bash
cmake -B build -DBUILD_SHARED_LIBS=OFF
cmake --build build --config Release
```

Output: `KrbRelay_static.lib`

### Installation

```bash
cmake --install build --prefix C:/KrbRelay
```

This creates:
```
C:/KrbRelay/
├── include/
│   └── KrbRelay/
│       ├── Types.h
│       ├── SecurityBuffer.h
│       ├── NativeWrappers.h
│       ├── ComUtilities.h
│       └── KerberosRelay.h
├── lib/
│   ├── KrbRelay.lib (or KrbRelay_static.lib)
│   └── cmake/
│       └── KrbRelay/
└── bin/
    └── KrbRelay.dll (if built as shared)
```

## Integration Methods

### Method 1: CMake Integration (Recommended)

After installation, add to your `CMakeLists.txt`:

```cmake
find_package(KrbRelay REQUIRED)

add_executable(MyApp main.cpp)
target_link_libraries(MyApp PRIVATE KrbRelay::KrbRelay)
```

### Method 2: Manual Integration

#### For DLL:
```cmake
add_executable(MyApp main.cpp)
target_include_directories(MyApp PRIVATE "C:/KrbRelay/include")
target_link_libraries(MyApp PRIVATE "C:/KrbRelay/lib/KrbRelay.lib")
```

Copy `KrbRelay.dll` to your executable directory.

#### For Static Library:
```cmake
add_executable(MyApp main.cpp)
target_include_directories(MyApp PRIVATE "C:/KrbRelay/include")
target_link_libraries(MyApp PRIVATE 
    "C:/KrbRelay/lib/KrbRelay_static.lib"
    ws2_32
    secur32
    wldap32
    ole32
    oleaut32
    rpcrt4
    advapi32
    netapi32
)
target_compile_definitions(MyApp PRIVATE KRBRELAY_STATIC)
```

### Method 3: Subdirectory

Add as a subdirectory in your project:

```cmake
add_subdirectory(KrbRelayLib)
target_link_libraries(MyApp PRIVATE KrbRelay)
```

## Advanced Usage

### Custom Authentication Callback

```cpp
#include <KrbRelay/KerberosRelay.h>

bool MyAuthHandler(const std::vector<uint8_t>& ticket, bool isKerberos) {
    // Process intercepted ticket
    // Return true to continue, false to abort
    
    if (isKerberos) {
        // Handle Kerberos ticket
        std::cout << "Kerberos ticket: " << ticket.size() << " bytes" << std::endl;
    } else {
        // Handle NTLM
        std::cout << "NTLM token: " << ticket.size() << " bytes" << std::endl;
    }
    
    return true;
}

int main() {
    KrbRelay::KerberosRelay relay;
    relay.Configure("ldap/dc01.contoso.com", false);
    relay.SetAuthenticationCallback(MyAuthHandler);
    
    // ... rest of initialization
}
```

### Working with Security Buffers

```cpp
#include <KrbRelay/SecurityBuffer.h>

// Single buffer
KrbRelay::SecurityBufferManager buffer;
std::vector<uint8_t> data = {0x60, 0x82, 0x01, 0x23};
buffer.CreateBuffer(data, KrbRelay::BufferType::SECBUFFER_TOKEN);

// Multiple buffers (descriptor)
KrbRelay::SecurityBufferDescriptor descriptor;
descriptor.AddBuffer(buffer);

// Get underlying structures for Windows APIs
SecBufferDesc* pDesc = descriptor.GetDescriptor();
```

### COM Marshalling

```cpp
#include <KrbRelay/ComUtilities.h>

KrbRelay::COMUtilities comUtil;

// Marshal an interface
IUnknown* pUnknown = /* ... */;
std::vector<uint8_t> objrefData;
auto result = comUtil.MarshalInterface(pUnknown, objrefData);

if (result.success) {
    // Create moniker string
    std::string moniker = comUtil.CreateMonikerString(objrefData);
    
    // Parse OBJREF
    std::string ipid;
    uint64_t oxid, oid;
    comUtil.ParseOBJREF(objrefData, ipid, oxid, oid);
}
```

### Native API Usage

```cpp
#include <KrbRelay/NativeWrappers.h>

// Acquire credentials
CredHandle credHandle;
auto result = KrbRelay::NativeWrappers::AcquireCredentialsHandle(
    "Kerberos", 
    &credHandle
);

if (result.success) {
    // Use credentials...
    
    // Clean up
    KrbRelay::NativeWrappers::FreeCredentialsHandle(&credHandle);
}
```

## Examples

### Example 1: Simple Relay Setup

```cpp
#include <KrbRelay/KerberosRelay.h>
#include <KrbRelay/NativeWrappers.h>
#include <iostream>

int main() {
    // Initialize COM
    KrbRelay::NativeWrappers::InitializeCOM();
    
    // Create relay instance
    KrbRelay::KerberosRelay relay;
    
    // Configure for LDAP relay
    auto result = relay.Configure("ldap/dc01.contoso.com", false);
    if (!result.success) {
        std::cerr << "Configuration failed: " << result.message << std::endl;
        return 1;
    }
    
    // Set user context
    relay.SetUserContext("CONTOSO", "Administrator");
    
    std::cout << "Relay configured successfully" << std::endl;
    std::cout << "SPN: " << relay.GetSPN() << std::endl;
    std::cout << "User: " << relay.GetUserDomain() << "\\" 
              << relay.GetUsername() << std::endl;
    
    // Cleanup
    KrbRelay::NativeWrappers::UninitializeCOM();
    return 0;
}
```

### Example 2: Ticket Validation

```cpp
#include <KrbRelay/KerberosRelay.h>
#include <vector>
#include <iostream>

bool ValidateAndProcessTicket(const std::vector<uint8_t>& ticket) {
    // Validate AP-REQ format
    if (!KrbRelay::KerberosRelay::ValidateApReq(ticket)) {
        std::cerr << "Invalid AP-REQ format" << std::endl;
        return false;
    }
    
    std::cout << "Valid AP-REQ ticket" << std::endl;
    
    // Convert ticket format if needed
    std::vector<uint8_t> convertedTicket = ticket;
    auto result = KrbRelay::KerberosRelay::ConvertTicketFormat(convertedTicket);
    
    if (result.success) {
        // Process converted ticket
        std::cout << "Ticket converted successfully" << std::endl;
    }
    
    return true;
}
```

### Example 3: Helper Utilities

```cpp
#include <KrbRelay/KerberosRelay.h>
#include <iostream>

int main() {
    // Parse SPN
    std::string service, hostname;
    if (KrbRelay::RelayHelpers::ParseSPN("http/exchange.contoso.com", 
                                          service, hostname)) {
        std::cout << "Service: " << service << std::endl;
        std::cout << "Hostname: " << hostname << std::endl;
    }
    
    // Hex conversion
    std::vector<uint8_t> bytes = {0xDE, 0xAD, 0xBE, 0xEF};
    std::string hex = KrbRelay::RelayHelpers::BytesToHexString(bytes);
    std::cout << "Hex: " << hex << std::endl;
    
    // Pattern search
    std::vector<uint8_t> data = {0x00, 0x01, 0x6E, 0x82, 0x03};
    std::vector<uint8_t> pattern = {0x6E, 0x82};
    int offset = KrbRelay::RelayHelpers::FindPattern(data, pattern);
    std::cout << "Pattern found at: " << offset << std::endl;
    
    return 0;
}
```

## Troubleshooting

### Common Issues

#### Link Error: Unresolved External Symbols

**Problem**: Using static library but linking as shared.

**Solution**: Add `KRBRELAY_STATIC` definition:
```cmake
target_compile_definitions(MyApp PRIVATE KRBRELAY_STATIC)
```

#### DLL Not Found at Runtime

**Problem**: `KrbRelay.dll` is not in the executable directory or PATH.

**Solution**: Copy DLL to executable directory or add to PATH:
```cmake
add_custom_command(TARGET MyApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:KrbRelay>
    $<TARGET_FILE_DIR:MyApp>
)
```

#### COM Initialization Fails

**Problem**: COM already initialized in another mode.

**Solution**: Check return value - `RPC_E_CHANGED_MODE` is not fatal:
```cpp
auto result = NativeWrappers::InitializeCOM();
// Check specific error codes if needed
```

## Best Practices

1. **Always initialize/cleanup COM**:
   ```cpp
   NativeWrappers::InitializeCOM();
   // ... your code ...
   NativeWrappers::UninitializeCOM();
   ```

2. **Check operation results**:
   ```cpp
   auto result = relay.Configure(...);
   if (!result.success) {
       // Handle error
   }
   ```

3. **Use RAII for resource management**:
   The library uses PIMPL and smart pointers internally, but manage Windows handles carefully.

4. **Thread safety**:
   Create separate instances for different threads. The library is not thread-safe by default.

## API Reference

See individual header files in `include/KrbRelay/` for detailed API documentation.

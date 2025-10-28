# KrbRelay C++ Library

A reusable C++ library for Kerberos relaying functionality on Windows. This library provides a clean, modern C++ interface for working with Kerberos authentication, COM marshalling, and security buffers.

## Features

- **Security Buffer Management**: Easy-to-use wrappers for Windows security buffers
- **Kerberos Relay**: Core functionality for relaying Kerberos authentication
- **COM Utilities**: OBJREF handling and COM marshalling
- **Native API Wrappers**: C++ wrappers for Windows SSPI and COM APIs
- **Flexible Build**: Can be built as both DLL and static library

## Building

### Requirements

- CMake 3.15 or higher
- Visual Studio 2017 or higher (MSVC)
- Windows SDK

### Build as Shared Library (DLL)

```bash
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=ON
cmake --build . --config Release
```

### Build as Static Library

```bash
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=OFF
cmake --build . --config Release
```

### Build Options

- `BUILD_SHARED_LIBS`: Build as shared library (DLL) when ON, static library when OFF (default: ON)
- `BUILD_EXAMPLES`: Build example programs (default: ON)

## Installation

```bash
cmake --install . --prefix <install_path>
```

This will install:
- Headers to `<install_path>/include/KrbRelay/`
- Libraries to `<install_path>/lib/`
- CMake config files to `<install_path>/lib/cmake/KrbRelay/`

## Usage

### Using in Your CMake Project

```cmake
find_package(KrbRelay REQUIRED)
target_link_libraries(your_target PRIVATE KrbRelay::KrbRelay)
```

### Basic Example

```cpp
#include <KrbRelay/KerberosRelay.h>
#include <KrbRelay/SecurityBuffer.h>
#include <KrbRelay/NativeWrappers.h>
#include <iostream>

using namespace KrbRelay;

int main() {
    // Initialize COM
    auto result = NativeWrappers::InitializeCOM();
    if (!result.success) {
        std::cerr << "Failed to initialize COM: " << result.message << std::endl;
        return 1;
    }

    // Configure Kerberos relay
    KerberosRelay relay;
    relay.Configure("ldap/dc01.contoso.com", false);
    relay.SetUserContext("CONTOSO", "Administrator");

    // Create security buffer
    SecurityBufferManager buffer;
    std::vector<uint8_t> ticketData = {0x60, 0x82, 0x01, 0x23};
    buffer.CreateBuffer(ticketData, BufferType::SECBUFFER_TOKEN);

    // Validate AP-REQ
    if (KerberosRelay::ValidateApReq(ticketData)) {
        std::cout << "Valid Kerberos AP-REQ" << std::endl;
    }

    // Cleanup
    NativeWrappers::UninitializeCOM();
    return 0;
}
```

## API Overview

### KerberosRelay

Main class for Kerberos relay functionality:

- `Configure(spn, useSSL)`: Configure relay with target SPN
- `SetAuthenticationCallback(callback)`: Set callback for intercepted tickets
- `InitializeComServer(clsid, port)`: Initialize COM server for relay
- `ValidateApReq(ticket)`: Validate Kerberos AP-REQ format
- `ExtractKerberosTicket(buffer, ticket)`: Extract ticket from security buffer

### SecurityBufferManager

Manages Windows security buffers:

- `CreateBuffer(data, type)`: Create a security buffer
- `GetData()`: Get buffer data as byte vector
- `GetSecBuffer()`: Get underlying SecBuffer structure

### NativeWrappers

Wrappers for Windows native APIs:

- `AcquireCredentialsHandle(package, handle)`: Acquire credentials
- `InitializeSecurityContext(...)`: Initialize security context
- `AcceptSecurityContext(...)`: Accept security context
- `InitializeCOM()`: Initialize COM library
- `InitializeCOMSecurity(...)`: Initialize COM security

### COMUtilities

COM marshalling and OBJREF handling:

- `MarshalInterface(pUnknown, objrefData)`: Marshal COM interface
- `UnmarshalInterface(objrefData, ppUnknown)`: Unmarshal COM interface
- `CreateMonikerString(objrefData)`: Create moniker from OBJREF
- `ParseOBJREF(...)`: Parse OBJREF structure

### RelayHelpers

Utility functions:

- `BytesToHexString(data)`: Convert bytes to hex string
- `HexStringToBytes(hexStr)`: Convert hex string to bytes
- `FindPattern(data, pattern)`: Find byte pattern in data
- `ParseSPN(spn, service, hostname)`: Parse Service Principal Name

## Architecture

The library is designed for maximum reusability:

- **Header-only interface**: Clean public API in `include/KrbRelay/`
- **Implementation hiding**: PIMPL idiom for ABI stability
- **No server implementations**: Focus on core relay functionality
- **Callback-based design**: Flexible integration with custom logic
- **Windows native**: Leverages Windows SSPI and COM APIs

## License

This is a port of the original KrbRelay project. Please refer to the original project for licensing information.

## Acknowledgements

Based on the original C# KrbRelay project by @Cube0x0, which credits:
- James Forshaw (@tiraniddo) for introducing Kerberos relaying
- Vletoux for RPCForSMBLibrary
- And many others in the security research community

## Security Notice

This library is intended for security research and authorized penetration testing only. 
Misuse of this library may be illegal. Always ensure you have proper authorization before 
using these tools and techniques.

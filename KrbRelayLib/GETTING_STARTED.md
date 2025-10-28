# Getting Started with KrbRelay C++ Library

## 5-Minute Quick Start

### Prerequisites Check

Before starting, ensure you have:
- ✅ Windows 10/11 or Windows Server 2016+
- ✅ Visual Studio 2017 or later
- ✅ CMake 3.15+ ([Download](https://cmake.org/download/))

### Step 1: Clone and Navigate

```cmd
git clone https://github.com/kiwids0220/KrbRelay-CppPort.git
cd KrbRelay-CppPort\KrbRelayLib
```

### Step 2: Build (Choose One)

#### Option A: Build as DLL
```cmd
cmake -B build -DBUILD_SHARED_LIBS=ON
cmake --build build --config Release
```

#### Option B: Build as Static Library
```cmd
cmake -B build -DBUILD_SHARED_LIBS=OFF
cmake --build build --config Release
```

### Step 3: Run Example

```cmd
build\bin\Release\basic_example.exe
```

You should see output demonstrating all library features!

## Next Steps

### Try the API in Your Code

Create `test.cpp`:
```cpp
#include <KrbRelay/KerberosRelay.h>
#include <iostream>

int main() {
    KrbRelay::KerberosRelay relay;
    auto result = relay.Configure("ldap/dc.contoso.com", false);
    
    if (result.success) {
        std::cout << "Configured for: " << relay.GetSPN() << std::endl;
    }
    
    return 0;
}
```

Build with CMake:
```cmake
find_package(KrbRelay REQUIRED)
add_executable(test test.cpp)
target_link_libraries(test PRIVATE KrbRelay::KrbRelay)
```

### Install Library

```cmd
cmake --install build --prefix C:\KrbRelay --config Release
```

Now you can use it from any project!

## Common First Steps

### 1. Parse an SPN
```cpp
std::string service, hostname;
KrbRelay::RelayHelpers::ParseSPN("ldap/dc.contoso.com", service, hostname);
std::cout << "Service: " << service << std::endl;  // "ldap"
std::cout << "Host: " << hostname << std::endl;    // "dc.contoso.com"
```

### 2. Work with Security Buffers
```cpp
KrbRelay::SecurityBufferManager buffer;
std::vector<uint8_t> data = {0x60, 0x82, 0x01, 0x23};
buffer.CreateBuffer(data, KrbRelay::BufferType::SECBUFFER_TOKEN);
```

### 3. Validate Kerberos Tickets
```cpp
std::vector<uint8_t> ticket = /* ... */;
if (KrbRelay::KerberosRelay::ValidateApReq(ticket)) {
    std::cout << "Valid Kerberos AP-REQ" << std::endl;
}
```

### 4. Initialize COM
```cpp
KrbRelay::NativeWrappers::InitializeCOM();
// ... your code ...
KrbRelay::NativeWrappers::UninitializeCOM();
```

## Where to Go From Here

### Learn More
- **API Reference**: See [QUICKREF.md](QUICKREF.md)
- **Integration Guide**: See [INTEGRATION.md](INTEGRATION.md)
- **Architecture**: See [ARCHITECTURE.md](ARCHITECTURE.md)
- **Build Help**: See [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md)

### Example Projects
Look at `examples/basic_example.cpp` for a complete working example.

### Build Your Own Tool
The library provides the foundation - add your specific functionality:
1. Configure relay with your target SPN
2. Set up authentication callbacks
3. Implement your attack logic
4. Handle intercepted tickets

## Troubleshooting First Steps

### "CMake not found"
Download and install from https://cmake.org/download/

### "Cannot find Visual Studio"
Run from **Visual Studio Developer Command Prompt**

### "DLL not found"
Copy `KrbRelay.dll` from `build\Release\` to your exe directory

### "Link errors"
If using static lib, add `KRBRELAY_STATIC` definition

## Quick Reference Card

```cpp
// Initialize
KrbRelay::NativeWrappers::InitializeCOM();

// Create relay
KrbRelay::KerberosRelay relay;
relay.Configure("service/host.domain.com", useSSL);

// Set callback
relay.SetAuthenticationCallback([](auto& ticket, bool isKerberos) {
    // Handle ticket
    return true;
});

// Helpers
auto hex = KrbRelay::RelayHelpers::BytesToHexString(bytes);
bool valid = KrbRelay::KerberosRelay::ValidateApReq(ticket);

// Cleanup
KrbRelay::NativeWrappers::UninitializeCOM();
```

## Support

- **Build Issues**: See [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md)
- **API Questions**: See [QUICKREF.md](QUICKREF.md)
- **Integration**: See [INTEGRATION.md](INTEGRATION.md)
- **Design Questions**: See [ARCHITECTURE.md](ARCHITECTURE.md)

## Security Note

⚠️ This library is for **authorized security testing only**. Ensure you have proper authorization before use.

---

**Ready to build?** Jump back to Step 1 above!

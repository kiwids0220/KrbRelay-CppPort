# KrbRelay C++ Library Architecture

This document describes the architecture and design decisions of the KrbRelay C++ library.

## Design Goals

1. **Reusability**: Create a library that other programmers can easily integrate
2. **Flexibility**: Support both DLL and static library builds
3. **Modern C++**: Use C++17 features and best practices
4. **Windows Native**: Leverage Windows SSPI and COM APIs
5. **Clean API**: Hide implementation details, expose only what's needed
6. **No Server Code**: Focus on core relay functionality, not protocol servers

## Architecture Overview

### Directory Structure

```
KrbRelayLib/
├── include/KrbRelay/         # Public API headers
│   ├── Types.h               # Common types and enums
│   ├── SecurityBuffer.h      # Security buffer management
│   ├── NativeWrappers.h      # Windows API wrappers
│   ├── ComUtilities.h        # COM marshalling utilities
│   └── KerberosRelay.h       # Main relay functionality
├── src/                      # Implementation files
│   ├── SecurityBuffer.cpp
│   ├── NativeWrappers.cpp
│   ├── ComUtilities.cpp
│   └── KerberosRelay.cpp
├── examples/                 # Example programs
│   ├── CMakeLists.txt
│   └── basic_example.cpp
├── CMakeLists.txt           # Build configuration
├── Config.cmake.in          # CMake package config
├── README.md                # Library documentation
└── INTEGRATION.md           # Integration guide
```

## Core Components

### 1. Types.h

Defines common types, enums, and structures used throughout the library:

- **BufferType**: Security buffer types (TOKEN, DATA, etc.)
- **AuthenticationLevel**: RPC authentication levels
- **ImpersonationLevel**: RPC impersonation levels
- **SecurityStatus**: Security operation status codes
- **OperationResult**: Standard result structure for all operations

**Design Decision**: Using strongly-typed enums (enum class) instead of raw integers for type safety.

### 2. SecurityBuffer.h / SecurityBuffer.cpp

Manages Windows security buffers for authentication data.

**Key Classes**:
- `SecurityBufferManager`: Single security buffer wrapper
- `SecurityBufferDescriptor`: Multi-buffer descriptor

**Design Decisions**:
- PIMPL idiom for ABI stability
- RAII for automatic resource management
- Vector-based data storage for safe memory management
- Direct access to underlying Windows structures when needed

**Usage Pattern**:
```cpp
SecurityBufferManager buffer;
buffer.CreateBuffer(ticketData, BufferType::SECBUFFER_TOKEN);
SecBuffer* pSecBuf = buffer.GetSecBuffer();  // For Windows APIs
```

### 3. NativeWrappers.h / NativeWrappers.cpp

Provides C++ wrappers around Windows SSPI and COM APIs.

**Key Functions**:
- Credentials management (Acquire/Free)
- Security context operations (Initialize/Accept/Delete)
- COM initialization and security
- Context attribute queries

**Design Decisions**:
- Static class (all static methods) - no instance needed
- Consistent return type (OperationResult)
- Automatic error message generation
- Pragma comments for required libraries

**Error Handling**:
```cpp
auto result = NativeWrappers::AcquireCredentialsHandle("Kerberos", &cred);
if (!result.success) {
    // Handle error: result.errorCode and result.message available
}
```

### 4. ComUtilities.h / ComUtilities.cpp

Handles COM marshalling, OBJREF parsing, and moniker creation.

**Key Functions**:
- Interface marshalling/unmarshalling
- OBJREF parsing (IPID, OXID, OID extraction)
- Moniker string creation
- OBJREF binding modification

**Design Decisions**:
- Instance-based class (may need state in future)
- PIMPL for encapsulation
- Stream-based marshalling using IStream
- Hex string utilities for OBJREF data

**OBJREF Format**:
```
Offset | Field  | Size
-------|--------|-----
0      | MEOW   | 4 bytes (signature)
...    | ...    | ...
24     | OXID   | 8 bytes
32     | OID    | 8 bytes
40     | IPID   | 16 bytes (GUID)
```

### 5. KerberosRelay.h / KerberosRelay.cpp

Main class providing Kerberos relay functionality.

**Key Features**:
- Configuration management (SPN, SSL)
- User context tracking
- Authentication callbacks
- Ticket validation and conversion
- Helper utilities

**Design Decisions**:
- Callback-based architecture for flexibility
- Placeholder implementations for advanced features
- Separate namespace for helper functions
- State management via PIMPL

**Callback Pattern**:
```cpp
bool OnAuthTicket(const std::vector<uint8_t>& ticket, bool isKerberos) {
    // Custom ticket handling
    return true;  // Continue processing
}

KerberosRelay relay;
relay.SetAuthenticationCallback(OnAuthTicket);
```

## Build System (CMake)

### Build Targets

1. **KrbRelay**: Main library (DLL or static)
2. **basic_example**: Example executable

### Build Options

- `BUILD_SHARED_LIBS`: ON for DLL, OFF for static (default: ON)
- `BUILD_EXAMPLES`: Build example programs (default: ON)

### Library Linking

**Shared Library (DLL)**:
- Exports: Controlled by `KRBRELAY_EXPORTS` and `__declspec(dllexport)`
- Imports: Users link against import library (KrbRelay.lib)
- Runtime: Requires KrbRelay.dll at runtime

**Static Library**:
- Name: KrbRelay_static.lib
- Requires: Users must define `KRBRELAY_STATIC`
- Linking: Must link all Windows system libraries

### Required Windows Libraries

```cmake
ws2_32        # Windows Sockets
secur32       # Security Support Provider Interface
wldap32       # LDAP client library
ole32         # COM library
oleaut32      # OLE Automation
rpcrt4        # RPC runtime
advapi32      # Advanced Windows API
netapi32      # Network API
```

## Design Patterns Used

### 1. PIMPL (Pointer to Implementation)

Used in all main classes to hide implementation details.

**Benefits**:
- ABI stability (implementation changes don't break binary compatibility)
- Faster compilation (reduces header dependencies)
- Cleaner public API

**Example**:
```cpp
class KerberosRelay {
public:
    KerberosRelay();
    ~KerberosRelay();
    // Public API...
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### 2. RAII (Resource Acquisition Is Initialization)

Used for automatic resource management.

**Benefits**:
- No manual cleanup needed
- Exception-safe resource handling
- Prevents resource leaks

**Example**:
```cpp
{
    SecurityBufferManager buffer;
    buffer.CreateBuffer(data, type);
    // Automatic cleanup when buffer goes out of scope
}
```

### 3. Callback Pattern

Used for extensibility without inheritance.

**Benefits**:
- Flexible integration
- No need for virtual functions
- Users can use lambdas or function pointers

**Example**:
```cpp
relay.SetAuthenticationCallback([](auto& ticket, bool isKerberos) {
    // Handle ticket
    return true;
});
```

### 4. Result Object Pattern

Consistent error handling across all operations.

**Benefits**:
- Standard error reporting
- No exception overhead
- Easy to check success/failure

**Example**:
```cpp
struct OperationResult {
    bool success;
    int32_t errorCode;
    std::string message;
};
```

## Thread Safety

**Current State**: Library is NOT thread-safe.

**Recommendations**:
- Create separate instances per thread
- Synchronize access to shared instances
- COM operations require apartment threading

**Future Work**: Add thread-safe variants if needed.

## Memory Management

1. **Vector-based storage**: All buffers use `std::vector<uint8_t>`
2. **Smart pointers**: PIMPL uses `std::unique_ptr`
3. **No raw new/delete**: RAII throughout
4. **Windows handles**: Explicitly freed in destructors

## API Design Principles

1. **Explicit is better than implicit**
   - All operations have clear return types
   - No hidden state changes

2. **Fail-fast with clear errors**
   - Operations return OperationResult
   - Error messages include context

3. **Const correctness**
   - Getter methods are const
   - Input parameters are const references

4. **Namespace organization**
   - Main API: `KrbRelay` namespace
   - Helpers: `KrbRelay::RelayHelpers` namespace

## Porting Decisions

### From C# to C++

**What was ported**:
- Core security buffer management
- Windows API wrappers (SSPI, COM)
- OBJREF handling
- Ticket validation logic
- Helper utilities

**What was NOT ported** (intentionally):
- Server implementations (LLMNR, HTTP, SMB)
- Attack-specific code (RBCD, shadow credentials)
- Client protocol implementations
- Console UI and argument parsing

**Why**: The goal was a reusable library, not a complete tool.

### Placeholder Implementations

Some functions have placeholder implementations:

```cpp
OperationResult HookAcceptSecurityContext() {
    // Full implementation requires:
    // 1. Getting function table
    // 2. Modifying function pointer
    // 3. Storing original for unhook
    return OperationResult(true, 0, "Hook placeholder");
}
```

**Rationale**: 
- Core infrastructure is in place
- Users can implement based on their needs
- Keeps library focused and maintainable

## Extension Points

Users can extend the library by:

1. **Implementing callbacks**: Custom ticket processing
2. **Deriving from base functionality**: Add protocol-specific code
3. **Using as foundation**: Build complete relay tools
4. **Contributing**: Add full implementations of placeholders

## Performance Considerations

1. **PIMPL overhead**: Minimal (one pointer indirection)
2. **Vector copying**: Use move semantics where possible
3. **String operations**: Minimal in hot paths
4. **Windows API calls**: Dominant performance factor

## Testing Strategy

**Current State**: No automated tests (out of scope for initial port)

**Recommendations**:
1. Unit tests for helper functions
2. Integration tests on Windows test machines
3. Mock Windows APIs for cross-platform testing
4. Fuzz testing for OBJREF parsing

## Future Enhancements

1. **Complete hook implementation**: Full AcceptSecurityContext hooking
2. **Protocol helpers**: Add LDAP/HTTP/SMB utilities
3. **Async operations**: Non-blocking relay operations
4. **Cross-platform abstractions**: Linux Kerberos support
5. **Python bindings**: PyBind11 wrapper for scripting

## References

- Original C# KrbRelay: Based on the C# implementation in KrbRelay/
- Windows SSPI: https://docs.microsoft.com/en-us/windows/win32/secauthn/sspi
- COM Marshalling: https://docs.microsoft.com/en-us/windows/win32/com/marshaling
- Kerberos Protocol: RFC 4120

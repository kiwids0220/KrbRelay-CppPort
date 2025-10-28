# Build Instructions for KrbRelay C++ Library

## Prerequisites

### Windows
- **Operating System**: Windows 10/11 or Windows Server 2016+
- **Compiler**: Visual Studio 2017 or later with C++17 support
- **CMake**: Version 3.15 or higher
- **Windows SDK**: Windows 10 SDK or later

### Linux (for development/testing CMake configuration only)
- CMake 3.15+
- GCC/Clang with C++17 support
- Note: Will not build/run on Linux (Windows-specific APIs)

## Building on Windows

### Option 1: Visual Studio IDE

1. Open CMake GUI or use Visual Studio's CMake support
2. Set source directory to `KrbRelayLib/`
3. Set build directory to `KrbRelayLib/build`
4. Configure and generate Visual Studio solution
5. Open solution in Visual Studio and build

### Option 2: Command Line (Visual Studio Developer Command Prompt)

#### Build as DLL (Shared Library)

```cmd
cd KrbRelayLib
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64 -DBUILD_SHARED_LIBS=ON
cmake --build . --config Release
```

Output files:
- `build/Release/KrbRelay.dll` - Shared library
- `build/Release/KrbRelay.lib` - Import library
- `build/bin/Release/basic_example.exe` - Example program

#### Build as Static Library

```cmd
cd KrbRelayLib
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64 -DBUILD_SHARED_LIBS=OFF
cmake --build . --config Release
```

Output files:
- `build/Release/KrbRelay_static.lib` - Static library
- `build/bin/Release/basic_example.exe` - Example program

### Option 3: MSBuild

```cmd
cd KrbRelayLib\build
msbuild KrbRelayLib.sln /p:Configuration=Release /p:Platform=x64
```

## Build Options

| Option | Values | Default | Description |
|--------|--------|---------|-------------|
| `BUILD_SHARED_LIBS` | ON/OFF | ON | Build as DLL (ON) or static library (OFF) |
| `BUILD_EXAMPLES` | ON/OFF | ON | Build example programs |
| `CMAKE_INSTALL_PREFIX` | path | varies | Installation directory |

### Example with Options

```cmd
cmake .. -DBUILD_SHARED_LIBS=OFF -DBUILD_EXAMPLES=ON
```

## Installation

After building, install to a specific directory:

```cmd
cmake --install . --prefix C:\KrbRelay --config Release
```

This creates:
```
C:\KrbRelay\
├── include\KrbRelay\    (headers)
├── lib\                 (libraries)
├── bin\                 (DLLs if built as shared)
└── lib\cmake\KrbRelay\  (CMake config files)
```

## Running Examples

### If built as DLL:

```cmd
cd build\bin\Release
basic_example.exe
```

The DLL must be in the same directory or in PATH.

### If built as static library:

```cmd
cd build\bin\Release
basic_example.exe
```

No additional files needed.

## Troubleshooting

### CMake Generator Not Found

**Error**: "Could not find a valid CMake generator"

**Solution**: Run from Visual Studio Developer Command Prompt, or specify generator:
```cmd
cmake .. -G "Visual Studio 16 2019"
```

Available generators:
- Visual Studio 17 2022
- Visual Studio 16 2019
- Visual Studio 15 2017
- NMake Makefiles
- Ninja

### Missing Windows SDK

**Error**: "Windows SDK not found"

**Solution**: 
1. Install Windows SDK via Visual Studio Installer
2. Or download from: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/

### Link Errors

**Error**: "unresolved external symbol"

**Solution for DLL build**:
- Ensure `KRBRELAY_EXPORTS` is defined when building
- Check that `KRBRELAY_SHARED` is defined when using

**Solution for static build**:
- Define `KRBRELAY_STATIC` in your project
- Link all required system libraries

### Cannot Open DLL

**Error**: "The code execution cannot proceed because KrbRelay.dll was not found"

**Solution**:
1. Copy `KrbRelay.dll` to your executable directory
2. Or add build directory to PATH
3. Or use CMake's `install` target

## Build Configurations

### Debug Build

```cmd
cmake --build . --config Debug
```

Outputs debug symbols for debugging.

### Release Build

```cmd
cmake --build . --config Release
```

Optimized build for production.

### RelWithDebInfo Build

```cmd
cmake --build . --config RelWithDebInfo
```

Optimized with debug information.

## Clean Build

```cmd
cmake --build . --target clean
```

Or simply delete the build directory:
```cmd
rmdir /s /q build
```

## Advanced Build Options

### Custom Compiler Flags

```cmd
cmake .. -DCMAKE_CXX_FLAGS="/W4 /WX"
```

### Specify C++ Standard

```cmd
cmake .. -DCMAKE_CXX_STANDARD=20
```

### Verbose Build

```cmd
cmake --build . --config Release --verbose
```

### Parallel Build

```cmd
cmake --build . --config Release --parallel 8
```

## Integration Testing

After building, test integration with a simple app:

1. Create `test.cpp`:
```cpp
#include <KrbRelay/KerberosRelay.h>
#include <iostream>

int main() {
    KrbRelay::KerberosRelay relay;
    std::cout << "KrbRelay library loaded successfully!" << std::endl;
    return 0;
}
```

2. Build:
```cmake
find_package(KrbRelay REQUIRED)
add_executable(test test.cpp)
target_link_libraries(test PRIVATE KrbRelay::KrbRelay)
```

## Continuous Integration

Example GitHub Actions workflow:

```yaml
name: Build

on: [push, pull_request]

jobs:
  build:
    runs-on: windows-latest
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Configure CMake
      run: cmake -B build -S KrbRelayLib -DBUILD_SHARED_LIBS=ON
    
    - name: Build
      run: cmake --build build --config Release
    
    - name: Test
      run: build\bin\Release\basic_example.exe
```

## Platform-Specific Notes

### Windows 11 / Server 2022
- No special considerations
- All features supported

### Windows 10
- Requires version 1809 or later
- Some COM features may require specific updates

### Windows Server 2016/2019
- Fully supported
- May require Windows updates for full COM support

## Support

For build issues:
1. Check CMake output for specific errors
2. Verify Visual Studio installation
3. Ensure Windows SDK is installed
4. Check CMakeLists.txt for required versions

For library usage issues:
- See [INTEGRATION.md](INTEGRATION.md)
- See [QUICKREF.md](QUICKREF.md)
- See examples in `examples/`

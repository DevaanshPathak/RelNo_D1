# Building RelNo_D1

This document provides detailed instructions for building the RelNo_D1 library on Windows.

## Prerequisites

- **CMake** (version 3.10 or higher)
- **C++17 compatible compiler**:
  - Visual Studio 2017 or newer (MSVC)
  - Or GCC/Clang with C++17 support
- **Git** (for cloning the repository)

---

## Quick Build Guide

### 1. Clone the Repository

```powershell
git clone https://github.com/DevaanshPathak/RelNo_D1.git
cd RelNo_D1
```

### 2. Create Build Directory

```powershell
mkdir build
cd build
```

### 3. Configure CMake

**For Visual Studio (default):**
```powershell
cmake ..
```

**For a specific generator:**
```powershell
cmake -G "Visual Studio 17 2022" ..
```

**Available generators:**
- `"Visual Studio 17 2022"` - VS 2022
- `"Visual Studio 16 2019"` - VS 2019
- `"Visual Studio 15 2017"` - VS 2017
- `"Ninja"` - Ninja build system
- `"Unix Makefiles"` - Make build system

### 4. Build the Project

**Build Release configuration:**
```powershell
cmake --build . --config Release
```

**Build Debug configuration:**
```powershell
cmake --build . --config Debug
```

**Build with multiple cores (faster):**
```powershell
cmake --build . --config Release --parallel
```

---

## Build Options

### Disable Examples

If you only want to build the library without examples:

```powershell
cmake .. -DBUILD_EXAMPLES=OFF
cmake --build . --config Release
```

### Clean Build

To rebuild from scratch (after changes):

```powershell
cmake --build . --config Release --clean-first
```

### Complete Clean (remove all build files)

```powershell
cd ..
Remove-Item -Recurse -Force build
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

---

## Running Examples

After building, executables are located in:
- `build/Release/` (for Release builds)
- `build/Debug/` (for Debug builds)

### Available Examples

1. **Main Noise Example:**
   ```powershell
   .\Release\RelNoD_NoiseExample.exe
   ```

2. **Sampling Test:**
   ```powershell
   .\Release\RelNoD_SamplingTest.exe
   ```

3. **Terrain Test:**
   ```powershell
   .\Release\RelNoD_TerrainTest.exe
   ```

4. **Chunk Test:**
   ```powershell
   .\Release\RelNoD_ChunkTest.exe
   ```

5. **Cave Test:**
   ```powershell
   .\Release\RelNoD_CaveTest.exe
   ```

6. **Tilemap Test:**
   ```powershell
   .\Release\RelNoD_TilemapTest.exe
   ```

7. **Post Processing Test:**
   ```powershell
   .\Release\RelNoD_PostProcessingTest.exe
   ```

8. **Domain Warp Test:**
   ```powershell
   .\Release\RelNoD_DomainWarpTest.exe
   ```

---

## Building with Visual Studio IDE

### Method 1: Using CMake-generated Solution

1. Generate Visual Studio solution:
   ```powershell
   mkdir build
   cd build
   cmake ..
   ```

2. Open `RelNo_D1.sln` in Visual Studio:
   ```powershell
   start RelNo_D1.sln
   ```

3. In Visual Studio:
   - Select **Release** or **Debug** configuration
   - Right-click on the solution → **Build Solution** (or press `Ctrl+Shift+B`)

### Method 2: Open Folder in Visual Studio

1. Open Visual Studio
2. **File** → **Open** → **Folder**
3. Select the `RelNo_D1` directory
4. Visual Studio will automatically detect CMakeLists.txt and configure the project

---

## Installation

To install the library headers and binaries:

```powershell
# From the build directory
cmake --install . --prefix ../install --config Release
```

This will create an `install` directory with:
- `install/include/Noise/` - Header files
- `install/lib/` - Static libraries

### Using in Your Project

After installation, you can use the library in your CMake projects:

```cmake
# In your CMakeLists.txt
list(APPEND CMAKE_PREFIX_PATH "path/to/RelNo_D1/install")
find_package(RelNo_D1 REQUIRED)

add_executable(MyApp main.cpp)
target_link_libraries(MyApp PRIVATE 
    RelNo_D1::WhiteNoise
    RelNo_D1::PerlinNoise
    RelNo_D1::SimplexNoise
    RelNo_D1::TerrainNoise
    RelNo_D1::CaveNoise
    RelNo_D1::TilemapExport
    RelNo_D1::PostProcessing
    RelNo_D1::DomainWarp
)
```

---

## Output Directories

Generated files are saved to:
- **Images**: `ImageOutput/` directory
- **Tilemaps**: `TilemapOutput/` directory
- **Executables**: `build/Release/` or `build/Debug/`

---

## Troubleshooting

### CMake Not Found
```powershell
# Add CMake to PATH or use full path
"C:\Program Files\CMake\bin\cmake.exe" ..
```

### Compiler Not Found
Ensure Visual Studio or another C++ compiler is installed:
```powershell
# Check MSVC
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
```

### Build Errors
1. Try cleaning and rebuilding:
   ```powershell
   cmake --build . --clean-first --config Release
   ```

2. Check C++17 support:
   - Visual Studio 2017 or newer required
   - Update your compiler if needed

### Permission Errors
Run PowerShell as Administrator if you encounter permission issues.

---

## Build Configurations

### Configuration Types

- **Release**: Optimized for speed, no debug symbols
  ```powershell
  cmake --build . --config Release
  ```

- **Debug**: Includes debug symbols, no optimization
  ```powershell
  cmake --build . --config Debug
  ```

- **RelWithDebInfo**: Optimized with debug symbols
  ```powershell
  cmake --build . --config RelWithDebInfo
  ```

- **MinSizeRel**: Optimized for size
  ```powershell
  cmake --build . --config MinSizeRel
  ```

---

## Advanced Options

### Verbose Build Output

```powershell
cmake --build . --config Release --verbose
```

### Specify Number of Parallel Jobs

```powershell
cmake --build . --config Release --parallel 8
```

### Build Specific Target

```powershell
cmake --build . --config Release --target RelNoD_NoiseExample
```

---

## Summary of Common Commands

```powershell
# Clone and build (first time)
git clone https://github.com/DevaanshPathak/RelNo_D1.git
cd RelNo_D1
mkdir build
cd build
cmake ..
cmake --build . --config Release

# Run main example
.\Release\RelNoD_NoiseExample.exe

# Rebuild from scratch
cmake --build . --config Release --clean-first

# Install library
cmake --install . --prefix ../install --config Release
```

---

## Need Help?

- Check the main [README.md](README.md) for usage examples
- Review [LICENSE.txt](LICENSE.txt) for licensing information
- Open an issue on GitHub for build problems

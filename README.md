# Status
![Build Windows](https://github.com/Azenris/font_maker/actions/workflows/build-windows.yml/badge.svg)
![Build Ubuntu](https://github.com/Azenris/font_maker/actions/workflows/build-ubuntu.yml/badge.svg)

# What does it do?
Turn a PNG Font into an SDF PNG Font.

# What is this for?
I use it for `personal` projects.

# Possible Package Manager
I use vcpkg to install the dependencies.
```
cd c:\
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg
```
After running the bootstrap an exe should have been created.
```
add to enviroment path for easy access to vcpkg
```

# Dependencies
```
vcpkg install zlib:x64-windows-static-md
```
Packages will be downloaded into the vcpkg locations, in the packages & installed folders. Other possibilities include:
```
vcpkg install zlib:x64-windows
vcpkg install zlib:x64-windows-static
vcpkg install zlib:x64-windows-static-md
vcpkg install zlib:x64-linux-release
vcpkg install zlib:x64-linux-dynamic
```

# How to build the build system using Cmake
```
BUILD_STATIC              build deps as static (else shared) (ON/OFF)
BUILD_CRT_STATIC          link CRT as static (else shared) (ON/OFF)
LINK_ZLIB                 zlib location
LINK_ZLIB_INCLUDE         include folder
LINK_ZLIB_LIB             release lib
LINK_ZLIB_LIB_DEBUG       debug lib
```
```
cmake -S . -B build ^
    -DBUILD_STATIC=ON
    -DBUILD_CRT_STATIC=ON
    -DLINK_ZLIB="C:/vcpkg/packages/zlib_x64-windows-static"
    -DLINK_ZLIB_INCLUDE="include/"
    -DLINK_ZLIB_LIB="lib/zlib.lib"
    -DLINK_ZLIB_LIB_DEBUG="debug/lib/zlibd.lib"
```
You can then build using:
```
cmake --build build --config=Release
cmake --build build --config=Debug
```

# What does it do?
Turn a PNG Font into an SDF PNG Font.

# What is this for?
I use it for `personal` projects.

# How to build the build system using Cmake
```
cmake -S . -B build
cmake --build build --config=Release         for release
cmake --build build --config=Debug           for debug
```
If zlib cannot be found, manually specify the locations.
```
cmake -S . -B build -DZLIB_LIBRARY_RELEASE="C:/zlib/release/64/zlib.lib"  -DZLIB_LIBRARY_DEBUG="C:/zlib/debug/64/zlib.lib" -DZLIB_INCLUDE_DIR="C:/zlib/include"
```
```
ZLIB_LIBRARY_RELEASE      should point to your release zlib library
ZLIB_LIBRARY_DEBUG        should point to your debug zlib library
ZLIB_INCLUDE_DIR          should point to your zlib include directory
```
```
You can check the build.bat as an example.
It will also show how the zlib is compiled using vcpkg
```
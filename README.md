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
cmake -S . -B build -DZLIB_LIBRARY="C:/libs/zlib/debug/64/zlib.lib" -DZLIB_INCLUDE_DIR="C:/libs/zlib/include"
ZLIB_LIBRARY      should point to your zlib library
ZLIB_INCLUDE_DIR  should point to your zlib include directory
```

# build.bat
You can check the build.bat as an example.
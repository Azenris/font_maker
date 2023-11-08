@echo OFF
cls

:: VCPKG
:: goto (for example) C:/
:: git clone https://github.com/microsoft/vcpkg.git
:: run the bootstrap-vcpkg (a vcpkg.exe will be created)
:: add this to global enviroment paths

:: Then run:
:: vcpkg install zlib:x64-windows-static-md

:: If you want to use the shared lib. then instead of the 3 defines just setup this one
:: Set CMAKE_TOOLCHAIN_FILE to the <vcpkg-path>/scripts/buildsystems/vcpkg.cmake
:: (for example) -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

cmake -S . -B build ^
	-DZLIB_INCLUDE_DIR=C:/vcpkg/packages/zlib_x64-windows-static-md/include/ ^
	-DZLIB_LIBRARY_RELEASE=C:/vcpkg/packages/zlib_x64-windows-static-md/lib/zlib.lib ^
	-DZLIB_LIBRARY_DEBUG=C:/vcpkg/packages/zlib_x64-windows-static-md/debug/lib/zlibd.lib

cmake --build build --config=Release
@echo OFF
cls

:: VCPKG
:: goto (for example) C:/
:: git clone https://github.com/microsoft/vcpkg.git
:: run the bootstrap-vcpkg (a vcpkg.exe will be created)
:: add this to global enviroment paths

:: vcpkg install zlib:x64-windows

:: Set CMAKE_TOOLCHAIN_FILE to the <vcpkg-path>/scripts/buildsystems/vcpkg.cmake
:: (for example) -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

:: ZLIB (zlib doesn't allow selection of static/dynamic lib. Manually setting them will allow choosing)

if not exist build (
	cmake -S . -B build ^
		-DZLIB_INCLUDE_DIR=C:/vcpkg/packages/zlib_x64-windows/include/ ^
		-DZLIB_LIBRARY_RELEASE=C:/vcpkg/packages/zlib_x64-windows/lib/zlib.lib ^
		-DZLIB_LIBRARY_DEBUG=C:/vcpkg/packages/zlib_x64-windows/debug/lib/zlibd.lib
)

cmake --build build --config=Release
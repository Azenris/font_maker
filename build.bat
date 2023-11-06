@echo OFF
cls

:: Change -DZLIB_LIBRARY= to point to your zlib library
	:: You can optionaly specify DZLIB_LIBRARY_RELEASE & DZLIB_LIBRARY_DEBUG separately
:: Change -DZLIB_INCLUDE_DIR= to point to your zlib include directory

if not exist build (
	cmake -S . -B build ^
		-DZLIB_LIBRARY_RELEASE="C:/libs/zlib/release/64/zlib.lib" ^
		-DZLIB_LIBRARY_DEBUG="C:/libs/zlib/debug/64/zlib.lib" ^
		-DZLIB_INCLUDE_DIR="C:/libs/zlib/include/"
)

cmake --build build --config=Debug
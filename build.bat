@echo OFF
cls

cmake -S . -B build/static-mt ^
	-DBUILD_STATIC=ON ^
	-DBUILD_CRT_STATIC=ON ^
	-DLINK_ZLIB="C:/vcpkg/packages/zlib_x64-windows-static" ^
	-DLINK_ZLIB_INCLUDE="include/" ^
	-DLINK_ZLIB_LIB="lib/zlib.lib" ^
	-DLINK_ZLIB_LIB_DEBUG="debug/lib/zlibd.lib"

cmake --build build/static-mt --config=Release
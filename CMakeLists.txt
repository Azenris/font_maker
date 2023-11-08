
cmake_minimum_required( VERSION 3.26.3 )

project(
	font_maker
	VERSION 1.0
	LANGUAGES CXX
)

add_executable( font_maker src/main.cpp )

target_include_directories( font_maker PRIVATE "./third_party/" )

target_compile_definitions( font_maker PRIVATE C_PLUS_PLUS )
target_compile_definitions( font_maker PRIVATE "$<$<CONFIG:Debug>:DEBUG>" )
target_compile_definitions( font_maker PRIVATE "$<$<CONFIG:Release>:NDEBUG>" )

target_compile_features( font_maker PRIVATE cxx_std_20 )

find_package( ZLIB REQUIRED )
target_link_libraries( font_maker PRIVATE ZLIB::ZLIB )

if ( MSVC )
target_compile_definitions( font_maker PRIVATE _CRT_SECURE_NO_WARNINGS )
	target_compile_options( font_maker PRIVATE -WX -W4 -wd4100 -wd4201 -wd4706 -Zc:preprocessor -Zc:strictStrings -GR- )
	target_compile_options( font_maker PRIVATE $<$<CONFIG:Debug>:-Z7 -FC -MDd> )
	target_compile_options( font_maker PRIVATE $<$<CONFIG:Release>:-MD -O2 -Ot -GF> )
endif()

if ( CMAKE_COMPILER_IS_GNUCC )
	target_compile_options( font_maker PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-uninitialized -Wno-non-virtual-dtor -fno-rtti )
	target_compile_options( font_maker PRIVATE $<$<CONFIG:Debug>:-OO -g> )
	target_compile_options( font_maker PRIVATE $<$<CONFIG:Release>:-O2> )
endif()

if ( CMAKE_SYSTEM_NAME STREQUAL "Windows" )
	target_compile_definitions( font_maker PRIVATE "PLATFORM_WINDOWS" )
endif()
if ( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
	target_compile_definitions( font_maker PRIVATE "PLATFORM_LINUX" )
endif()
if ( CMAKE_SYSTEM_NAME STREQUAL "Darwin" )
	target_compile_definitions( font_maker PRIVATE "PLATFORM_MAC" )
endif()
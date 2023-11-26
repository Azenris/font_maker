
cmake_minimum_required( VERSION 3.26.3 )

project(
	font_maker
	VERSION 1.0
	LANGUAGES CXX
)

add_executable( font_maker src/main.cpp )

target_compile_definitions( font_maker PRIVATE C_PLUS_PLUS )
target_compile_definitions( font_maker PRIVATE "$<$<CONFIG:Debug>:DEBUG>" )
target_compile_definitions( font_maker PRIVATE "$<$<CONFIG:Release>:NDEBUG>" )

target_compile_features( font_maker PRIVATE cxx_std_20 )

target_include_directories( font_maker PRIVATE "third_party/" )

if ( NOT LINK_ZLIB_INCLUDE )
	message( FATAL_ERROR "LINK_ZLIB_INCLUDE path missing" )
endif()
if ( NOT LINK_ZLIB_LIB )
	message( FATAL_ERROR "LINK_ZLIB_LIB path missing" )
endif()
if ( NOT LINK_ZLIB_LIB_DEBUG )
	message( FATAL_ERROR "LINK_ZLIB_LIB_DEBUG path missing" )
endif()

function( connect_zlib target static crtStatic )
	set( ZLIB_INCLUDE_DIR     ${LINK_ZLIB}/${LINK_ZLIB_INCLUDE}   )
	set( ZLIB_LIBRARY_RELEASE ${LINK_ZLIB}/${LINK_ZLIB_LIB}       )
	set( ZLIB_LIBRARY_DEBUG   ${LINK_ZLIB}/${LINK_ZLIB_LIB_DEBUG} )

	find_package( ZLIB REQUIRED )

	if ( ZLIB_FOUND )
		message( STATUS "Found zlib, linking to ${target}" )
		target_link_libraries( ${target} PRIVATE ZLIB::ZLIB )
	else()
		message( FATAL_ERROR "Failed to find zlib." )
	endif()
endfunction()

connect_zlib( font_maker BUILD_STATIC BUILD_CRT_STATIC )

if ( MSVC )
	target_compile_definitions( font_maker PRIVATE _CRT_SECURE_NO_WARNINGS )
	target_compile_options( font_maker PRIVATE -WX -W4 -wd4100 -wd4201 -wd4706 -Zc:preprocessor -Zc:strictStrings -GR- )
	target_compile_options( font_maker PRIVATE $<$<CONFIG:Debug>:-Z7 -FC> )
	target_compile_options( font_maker PRIVATE $<$<CONFIG:Release>:-O2 -Ot -GF> )
	target_compile_options( font_maker PRIVATE $<$<CONFIG:Release>:-MT>$<$<CONFIG:Debug>:-MTd> )
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

option( BUILD_STATIC "Build static" ON )
option( BUILD_CRT_STATIC "CRT static link." ON )

if ( BUILD_STATIC AND BUILD_CRT_STATIC )
	set_target_properties(
		font_maker
		PROPERTIES
		OUTPUT_NAME                      "font_maker"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/../builds/bin-static-mt-debug"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/../builds/bin-static-mt-release"
	)
endif()

if ( BUILD_STATIC AND NOT BUILD_CRT_STATIC )
	set_target_properties(
		font_maker
		PROPERTIES
		OUTPUT_NAME                      "font_maker"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/../builds/bin-static-md-debug"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/../builds/bin-static-md-release"
	)
endif()

if ( NOT BUILD_STATIC AND BUILD_CRT_STATIC )
	set_target_properties(
		font_maker
		PROPERTIES
		OUTPUT_NAME                      "font_maker"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/../builds/bin-shared-mt-debug"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/../builds/bin-shared-mt-release"
	)
endif()

if ( NOT BUILD_STATIC AND NOT BUILD_CRT_STATIC )
	set_target_properties(
		font_maker
		PROPERTIES
		OUTPUT_NAME                      "font_maker"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/../builds/bin-shared-md-debug"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/../builds/bin-shared-md-release"
	)
endif()
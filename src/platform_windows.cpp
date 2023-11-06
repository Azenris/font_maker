
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#undef min
#undef max

void show_debug_function( WORD colour, const char *prefix, const char *message )
{
	HANDLE consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

	SetConsoleTextAttribute( consoleHandle, colour );

	#ifdef DEBUG
		OutputDebugStringA( prefix );
		OutputDebugStringA( message );
		OutputDebugStringA( "\n" );
	#endif

	WriteConsoleA( consoleHandle, prefix, (DWORD)strlen( prefix ), 0, 0 );
	WriteConsoleA( consoleHandle, message, (DWORD)strlen( message ), 0, 0 );
	WriteConsoleA( consoleHandle, "\n", 1, 0, 0 );
}

// Messages
void show_debug_message_function( const char *message, ... )
{
	char buffer[ 2048 ];
	va_list args;
	va_start( args, message );
	vsnprintf( buffer, array_length( buffer ), message, args );
	show_debug_function( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, "[  MSG]: ", buffer );
	va_end( args );
}

void show_debug_message_function_ext( const char *message, va_list args )
{
	char buffer[ 2048 ];
	vsnprintf( buffer, array_length( buffer ), message, args );
	show_debug_function( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, "[  MSG]: ", buffer );
}

void show_debug_information_function( const char *message, ... )
{
	char buffer[ 2048 ];
	va_list args;
	va_start( args, message );
	vsnprintf( buffer, array_length( buffer ), message, args );
	show_debug_function( FOREGROUND_GREEN | FOREGROUND_BLUE, "[ INFO]: ", buffer );
	va_end( args );
}

void show_debug_information_function_ext( const char *message, va_list args )
{
	char buffer[ 2048 ];
	vsnprintf( buffer, array_length( buffer ), message, args );
	show_debug_function( FOREGROUND_GREEN | FOREGROUND_BLUE, "[ INFO]: ", buffer );
}

void show_debug_warning_function( const char *message, ... )
{
	char buffer[ 2048 ];
	va_list args;
	va_start( args, message );
	vsnprintf( buffer, array_length( buffer ), message, args );
	show_debug_function( FOREGROUND_RED | FOREGROUND_GREEN, "[ WARN]: ", buffer );
	va_end( args );
}

void show_debug_warning_function_ext( const char *message, va_list args )
{
	char buffer[ 2048 ];
	vsnprintf( buffer, array_length( buffer ), message, args );
	show_debug_function( FOREGROUND_RED | FOREGROUND_GREEN, "[ WARN]: ", buffer );
}

void show_debug_error_function( const char *message, ... )
{
	char buffer[ 2048 ];
	va_list args;
	va_start( args, message );
	vsnprintf( buffer, array_length( buffer ), message, args );
	show_debug_function( FOREGROUND_RED, "[ERROR]: ", buffer );
	va_end( args );
}

void show_debug_error_function_ext( const char *message, va_list args )
{
	char buffer[ 2048 ];
	vsnprintf( buffer, array_length( buffer ), message, args );
	show_debug_function( FOREGROUND_RED, "[ERROR]: ", buffer );
}

// Timing
/// @desc Returns how many tick counts happen per second. Cannot change after system boot. Only need once and cache.
[[nodiscard]] inline u64 platform_get_tick_frequency()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency( &freq );
	return freq.QuadPart;
}

[[nodiscard]] inline u64 platform_get_tick_counter()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter( &counter );
	return counter.QuadPart;
}

inline void platform_sleep( i32 msWait )
{
	Sleep( msWait );
}
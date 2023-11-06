
#include <unistd.h>

// Messages
void show_debug_message_function( const char *message, ... )
{
	char buffer[ 2048 ];
	va_list args;
	va_start( args, message );
	vsnprintf( buffer, array_length( buffer ), message, args );
	printf( "\x1b[37m[  MSG]:\x1b[0m %s\n", buffer );
	va_end( args );
}

void show_debug_message_function_ext( const char *message, va_list args )
{
	char buffer[ 2048 ];
	vsnprintf( buffer, array_length( buffer ), message, args );
	printf( "\x1b[37m[  MSG]:\x1b[0m %s\n", buffer );
}

void show_debug_information_function( const char *message, ... )
{
	char buffer[ 2048 ];
	va_list args;
	va_start( args, message );
	vsnprintf( buffer, array_length( buffer ), message, args );
	printf( "\x1b[36m[ INFO]:\x1b[0m %s\n", buffer );
	va_end( args );
}

void show_debug_information_function_ext( const char *message, va_list args )
{
	char buffer[ 2048 ];
	vsnprintf( buffer, array_length( buffer ), message, args );
	printf( "\x1b[36m[ INFO]:\x1b[0m %s\n", buffer );
}

void show_debug_warning_function( const char *message, ... )
{
	char buffer[ 2048 ];
	va_list args;
	va_start( args, message );
	vsnprintf( buffer, array_length( buffer ), message, args );
	printf( "\x1b[33m[ WARN]:\x1b[0m %s\n", buffer );
	va_end( args );
}

void show_debug_warning_function_ext( const char *message, va_list args )
{
	char buffer[ 2048 ];
	vsnprintf( buffer, array_length( buffer ), message, args );
	printf( "\x1b[33m[ WARN]:\x1b[0m %s\n", buffer );
}

void show_debug_error_function( const char *message, ... )
{
	char buffer[ 2048 ];
	va_list args;
	va_start( args, message );
	vsnprintf( buffer, array_length( buffer ), message, args );
	printf( "\x1b[31m[ERROR]:\x1b[0m %s\n", buffer );
	va_end( args );
}

void show_debug_error_function_ext( const char *message, va_list args )
{
	char buffer[ 2048 ];
	vsnprintf( buffer, array_length( buffer ), message, args );
	printf( "\x1b[31m[ERROR]:\x1b[0m %s\n", buffer );
}

void platform_sleep( i32 msWait )
{
	usleep( msWait * 1000 );
}
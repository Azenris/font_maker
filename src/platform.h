
#pragma once

void show_debug_message_function( const char *message, ... );
void show_debug_message_function_ext( const char *message, va_list args );
void show_debug_information_function( const char *message, ... );
void show_debug_information_function_ext( const char *message, va_list args );
void show_debug_warning_function( const char *message, ... );
void show_debug_warning_function_ext( const char *message, va_list args );
void show_debug_error_function( const char *message, ... );
void show_debug_error_function_ext( const char *message, va_list args );

void platform_sleep( i32 msWait );
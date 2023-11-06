
#pragma once

#define max( a, b )		( ( a ) >= ( b ) ? ( a ) : ( b ) )
#define min( a, b )		( ( a ) <= ( b ) ? ( a ) : ( b ) )

inline void write_data( u8 *&dst, const void *src, u64 size, u8 *dstBarrier )
{
	massert( dst + size <= dstBarrier );
	memcpy( dst, src, size );
	dst += size;
}

[[nodiscard]] inline f32 increment_real( f32 value )
{
	int exponent;
	f32 mantissa = frexp( value, &exponent );
	if ( mantissa == 0 )
		return FLT_MIN;
	mantissa += FLT_EPSILON / 2.0f;
	value = ldexp( mantissa, exponent );
	return value;
}

[[nodiscard]] inline f32 decrement_real( f32 value )
{
	int exponent;
	f32 mantissa = frexp( value, &exponent );
	if ( mantissa == 0 )
		return FLT_MIN;
	mantissa -= FLT_EPSILON / 2.0f;
	value = ldexp( mantissa, exponent );
	return value;
}

[[nodiscard]] inline f64 increment_real( f64 value )
{
	int exponent;
	f64 mantissa = frexp( value, &exponent );
	if ( mantissa == 0 )
		return DBL_MIN;
	mantissa += DBL_EPSILON / 2.0;
	value = ldexp( mantissa, exponent );
	return value;
}

[[nodiscard]] inline f64 decrement_real( f64 value )
{
	int exponent;
	f64 mantissa = frexp( value, &exponent );
	if ( mantissa == 0 )
		return DBL_MIN;
	mantissa -= DBL_EPSILON / 2.0;
	value = ldexp( mantissa, exponent );
	return value;
}

inline void set_magic_value( char( &destination )[ 4 ], const char magic[ 4 ] )
{
	destination[ 0 ] = magic[ 0 ];
	destination[ 1 ] = magic[ 1 ];
	destination[ 2 ] = magic[ 2 ];
	destination[ 3 ] = magic[ 3 ];
}

[[nodiscard]] inline bool has_magic_value( const char value[ 4 ], const char magic[ 4 ] )
{
	return value[ 0 ] == magic[ 0 ] &&
		value[ 1 ] == magic[ 1 ] &&
		value[ 2 ] == magic[ 2 ] &&
		value[ 3 ] == magic[ 3 ];
}

int convert_to_int( const char *value )
{
	return atoi( value );
}

namespace base64
{
	static const char *base64LastError = "";

	[[nodiscard]] inline const char *get_last_error()
	{
		return base64LastError;
	}

	bool encode( u8 *dest, u64 *destSize, const u8 *source, u64 sourceSize )
	{
		massert( dest );
		massert( destSize );
		massert( source );
		massert( sourceSize );

		static constexpr const char *toBase64 =
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
				"0123456789+/";

		u64 missing = 0;
		u64 reqSize = sourceSize;

		while ( ( reqSize % 3 ) != 0 )
		{
			++reqSize;
			++missing;
		}

		reqSize = 4 * reqSize / 3;

		// make sure there is enough room in the dest buffer
		if ( *destSize < reqSize )
		{
			base64LastError = "Required memory size is greater than the supplied buffer size.";
			return false;
		}

		*destSize = reqSize;

		for ( u64 i = 0, count = ( reqSize / 4 ) * 3; i < count; i += 3 )
		{
			u8 byte0 = ( i + 0 < sourceSize ) ? source[ i + 0 ] : 0;
			u8 byte1 = ( i + 1 < sourceSize ) ? source[ i + 1 ] : 0;
			u8 byte2 = ( i + 2 < sourceSize ) ? source[ i + 2 ] : 0;

			*dest++ = toBase64[ ( ( byte0 & 0xfc ) >> 2 ) ];
			*dest++ = toBase64[ ( ( byte0 & 0x03 ) << 4 ) + ( ( byte1 & 0xf0 ) >> 4 ) ];
			*dest++ = toBase64[ ( ( byte1 & 0x0f ) << 2 ) + ( ( byte2 & 0xc0 ) >> 6 ) ];
			*dest++ = toBase64[ ( ( byte2 & 0x3f ) << 0 ) ];
		}

		for ( u64 i = 0; i < missing; ++i )
			*dest++ = '=';

		return true;
	}

	bool decode( u8 *dest, u64 *destSize, const u8 *source, u64 sourceSize )
	{
		massert( dest );
		massert( destSize );
		massert( source );
		massert( sourceSize );

		static constexpr const u8 fromBase64[] =
		{
			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255,  62, 255,  63,
			 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,   0, 255, 255, 255,
			255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
			 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255,  63,
			255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
			 41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255
		};

		massert( sourceSize % 4 == 0 );

		// make sure there is enough room in the dest buffer
		if ( *destSize < ( 3 * sourceSize / 4 ) )
		{
			base64LastError = "Required memory size is greater than the supplied buffer size.";
			return false;
		}

		u8 *start = dest;

		for ( u64 i = 0; i < sourceSize; i += 4 )
		{
			u8 byte0 = ( source[ i + 0 ] <= 'z' ) ? fromBase64[ source[ i + 0 ] ] : 0xff;
			u8 byte1 = ( source[ i + 1 ] <= 'z' ) ? fromBase64[ source[ i + 1 ] ] : 0xff;
			u8 byte2 = ( source[ i + 2 ] <= 'z' ) ? fromBase64[ source[ i + 2 ] ] : 0xff;
			u8 byte3 = ( source[ i + 3 ] <= 'z' ) ? fromBase64[ source[ i + 3 ] ] : 0xff;

			if ( byte1 != 0xff )
				*dest++ = static_cast<u8>( ( ( byte0 & 0x3f ) << 2 ) + ( ( byte1 & 0x30 ) >> 4 ) );

			if ( byte2 != 0xff )
				*dest++ = static_cast<u8>( ( ( byte1 & 0x0f ) << 4 ) + ( ( byte2 & 0x3c ) >> 2 ) );

			if ( byte3 != 0xff )
				*dest++ = static_cast<u8>( ( ( byte2 & 0x03 ) << 6 ) + ( ( byte3 & 0x3f ) >> 0 ) );
		}

		*destSize = ( dest - start );

		return true;
	}

	[[nodiscard]] u64 encode_bound( u64 size )
	{
		u64 reqSize = size;
		while ( ( reqSize % 3 ) != 0 )
			++reqSize;
		return 4 * reqSize / 3;
	}
}

namespace zlib
{
	static const char *zlibLastError = "";

	[[nodiscard]] inline const char *get_last_error()
	{
		return zlibLastError;
	}

	int compress( u8 *dest, u64 *destSize, const u8 *source, u64 sourceSize )
	{
		massert( dest, "zlib::compress : dest is nullptr" );
		massert( destSize, "zlib::compress : destSize is nullptr" );
		massert( source, "zlib::compress : source is nullptr" );

		uLongf _destSize = static_cast<uLong>( *destSize );

		int result = ::compress( dest, &_destSize, source, static_cast<uLong>( sourceSize ) );

		*destSize = _destSize;

		switch ( result )
		{
		case Z_OK:             zlibLastError = "";	break;
		case Z_STREAM_END:     zlibLastError = "";	break;
		case Z_NEED_DICT:      zlibLastError = "";	break;

			// errors
		case Z_STREAM_ERROR:   zlibLastError = "zlib::compress : [ stream error]";	break;
		case Z_DATA_ERROR:     zlibLastError = "zlib::compress : [   data error]";	break;
		case Z_MEM_ERROR:      zlibLastError = "zlib::compress : [ memory error]";	break;
		case Z_BUF_ERROR:      zlibLastError = "zlib::compress : [ buffer error]";	break;
		case Z_VERSION_ERROR:  zlibLastError = "zlib::compress : [version error]";	break;
		default:               zlibLastError = "zlib::compress : [unknown error]";	break;
		}

		return result;
	}

	int uncompress( u8 *dest, u64 *destSize, const u8 *source, u64 sourceSize )
	{
		massert( dest, "zlib::uncompress : dest is nullptr" );
		massert( destSize, "zlib::uncompress : destSize is nullptr" );
		massert( source, "zlib::uncompress : source is nullptr" );

		uLongf _destSize = static_cast<uLong>( *destSize );

		int result = ::uncompress( dest, &_destSize, source, static_cast<uLong>( sourceSize ) );

		*destSize = _destSize;

		switch ( result )
		{
		case Z_OK:             zlibLastError = "";	break;
		case Z_STREAM_END:     zlibLastError = "";	break;
		case Z_NEED_DICT:      zlibLastError = "";	break;

			// errors
		case Z_STREAM_ERROR:   zlibLastError = "zlib::uncompress : [ stream error]";	break;
		case Z_DATA_ERROR:     zlibLastError = "zlib::uncompress : [   data error]";	break;
		case Z_MEM_ERROR:      zlibLastError = "zlib::uncompress : [ memory error]";	break;
		case Z_BUF_ERROR:      zlibLastError = "zlib::uncompress : [ buffer error]";	break;
		case Z_VERSION_ERROR:  zlibLastError = "zlib::uncompress : [version error]";	break;
		default:               zlibLastError = "zlib::uncompress : [unknown error]";	break;
		}

		return result;
	}

	[[nodiscard]] u32 compress_bound( u64 size )
	{
		return static_cast<u32>( ::compressBound( static_cast<uLong>( size ) ) );
	}
}
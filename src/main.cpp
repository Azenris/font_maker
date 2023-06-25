
// System includes
#include <stdarg.h>
#include <cmath>

// Includes
#include "defines.h"
#include "logging.h"
#include "platform.h"
#include "zlib\zlib.h"
#include "array.h"
#include "strings.h"
#include "map.h"
#include "memory_arena.h"
#include "utility.h"
#include "error_codes.h"
#include "font_info.h"
#include "data_parser.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "platform_windows.cpp"

MemoryArena arena;

#include "image.h"

struct Options
{
	u64 memory = MB( 64 );
	const char *programName = "font_maker.exe";
	const char *workingDirectory = nullptr;
	bool verbose = false;
	char inputFile[ 4096 ];
	char outputFile[ 4096 ];
	char outputDataFile[ 4096 ];
	bool inject = false;
	int injectX = 0;
	int injectY = 0;
	int sdfBufferSize = 4;
	int maxW = 256;
	bool base64 = false;
	bool compress = false;

} options;

static int usage_message( RESULT_CODE code )
{
	show_log_warning( "\nERROR_CODE: %s\n", error_code_string( code ) );
	show_log_info( ":: USAGE ::" );
	show_log_message( "Expects %s <input file> <options>", options.programName );
	show_log_message( "Eg. %s %s\n", options.programName, "test\\main_font -o test\\main_font_sdf.png" );
	show_log_info( "OPTIONAL COMMANDS" );
	show_log_message( "[-v]                         EG. -v                                     (enable verbose outputs)" );
	show_log_message( "[-ra]                        EG. -ra                                    (outputs received arguments)" );
	show_log_message( "[-wd] <path>                 EG. -wd TEMP\\                              (override the default working directory)" );
	show_log_message( "[-o] <file>                  EG. -o assets\\fonts\\main_font.png          (override the default output file)" );
	show_log_message( "[-inject] <file> <int> <int> EG. -inject assets\\texture_atlas.png 32 15 (output image is placed inside another one)" );
	show_log_message( "[-d] <file>                  EG. -d assets\\fonts\\main_font.bin          (override the default output data file)" );
	show_log_message( "[-maxW] <int>                EG. -maxW 256                              (max pixel width of output image)" );
	show_log_message( "[-sdf] <int>                 EG. -sdf 8                                 (pixels added for sdf room)" );
	show_log_message( "[-base64]                    EG. -base64                                (enable base64 encoding)" );
	show_log_message( "[-compress]                  EG. -compress                              (enable zlib compressing)" );
	show_log_message( "[-memory] <bytes>            EG. -memory 1024                           (specify memory allocation)" );

#ifdef DEBUG
	system( "pause" );
#endif

	return code;
}

#ifdef DEBUG
int main( int _argc, const char *_argv[] )
{
	const char *argv[] = { "font_maker.exe", "debug\\test\\font_main.png", "-v", "-ra"
		, "-wd", "C:\\ProgrammingCDrive\\FontMaker", "-maxW", "512", "-sdf", "4"
		//, "-o", "debug\\test\\output_main_font.png"
		, "-inject", "debug\\test\\TEXTURE_ATLAS_01.png", "512", "0"
		, "-d", "debug\\test\\output_main_font.bin" };
	int argc = array_length( argv );
#else
int main( int argc, const char *argv[] )
{
#endif

	platform_initialise();

	options.inputFile[ 0 ] = '\0';
	options.outputFile[ 0 ] = '\0';
	options.outputDataFile[ 0 ] = '\0';

	Map<const char *, RESULT_CODE(*)( int &, int, const char ** ), 256> commands;

	commands.insert( "-v", [] ( int &index, int argc, const char *argv[] )
		{
			options.verbose = true;

			return RESULT_CODE_SUCCESS;
		} );

	commands.insert( "-ra", [] ( int &index, int argc, const char *argv[] )
		{
			show_log_message( "Arguments received [#%d]", argc );
			for ( int i = 0; i < argc; ++i )
				show_log_info( " [%d] = %s", i, argv[ i ] );

			return RESULT_CODE_SUCCESS;
		} );

	commands.insert( "-wd", [] ( int &index, int argc, const char *argv[] )
		{
			options.workingDirectory = argv[ ++index ];

			return RESULT_CODE_SUCCESS;
		} );

	commands.insert( "-o", [] ( int &index, int argc, const char *argv[] )
		{
			options.inject = false;

			string_utf8_copy( options.outputFile, argv[ ++index ] );

			return RESULT_CODE_SUCCESS;
		} );

	commands.insert( "-inject", [] ( int &index, int argc, const char *argv[] )
		{
			options.inject = true;

			string_utf8_copy( options.outputFile, argv[ ++index ] );
			options.injectX = convert_to_int( argv[ ++index ] );
			options.injectY = convert_to_int( argv[ ++index ] );

			return RESULT_CODE_SUCCESS;
		} );

	commands.insert( "-d", [] ( int &index, int argc, const char *argv[] )
		{
			string_utf8_copy( options.outputDataFile, argv[ ++index ] );

			return RESULT_CODE_SUCCESS;
		} );

	commands.insert( "-maxW", [] ( int &index, int argc, const char *argv[] )
		{
			options.maxW = convert_to_int( argv[ ++index ] );

			return RESULT_CODE_SUCCESS;
		} );

	commands.insert( "-sdf", [] ( int &index, int argc, const char *argv[] )
		{
			options.sdfBufferSize = convert_to_int( argv[ ++index ] );

			return RESULT_CODE_SUCCESS;
		} );

	commands.insert( "-base64", [] ( int &index, int argc, const char *argv[] )
		{
			options.base64 = true;

			return RESULT_CODE_SUCCESS;
		} );

	commands.insert( "-compress", [] ( int &index, int argc, const char *argv[] )
		{
			options.compress = true;

			return RESULT_CODE_SUCCESS;
		} );

	commands.insert( "-memory", [] ( int &index, int argc, const char *argv[] )
		{
			options.memory = convert_to_int( argv[ ++index ] );

			return RESULT_CODE_SUCCESS;
		} );

	// Process the option commands
	for ( int i = 2; i < argc; ++i )
	{
		auto f = commands.find( argv[ i ] );

		if ( f )
		{
			RESULT_CODE code = f->value( i, argc, &argv[ 0 ] );
			if ( code != RESULT_CODE_SUCCESS )
				return usage_message( code );
		}
		else
		{
			show_log_warning( "Unknown command: %s", argv[ i ] );
			return usage_message( RESULT_CODE_UNKNOWN_OPTIONAL_COMMAND );
		}
	}

	if ( !memory_arena_initialise( &arena, 0, options.memory ) )
	{
		show_log_error( "Failed to initialise memory arena" );
		return usage_message( RESULT_CODE_FAILED_MEMORY_ARENA_INITIALISATION );
	}

	// Set working directory
	if ( options.workingDirectory )
	{
		platform_set_current_directory( options.workingDirectory );

		if ( options.verbose )
			show_log_info( "Working directory changed to: %s", options.workingDirectory );
	}

	// Get the path without the extension
	char name[ 4096 ];
	string_utf8_copy( name, argv[ 1 ] );
	string_utf8_trim_ext( name );

	string_utf8_copy( options.inputFile, argv[ 1 ] );

	if ( options.verbose )
		show_log_info( "Input file: %s", options.inputFile );

	// Create an output filename if one was not provided
	if ( options.outputFile[ 0 ] == '\0' )
	{
		string_utf8_copy( options.outputFile, name );
		string_utf8_append( options.outputFile, "_output.png" );
	}

	// Create an output data filename if one was not provided
	if ( options.outputDataFile[ 0 ] == '\0' )
	{
		string_utf8_copy( options.outputDataFile, name );
		string_utf8_append( options.outputDataFile, "_output.bin" );
	}

	char inputDataFilename[ 4096 ];
	string_utf8_copy( inputDataFilename, name );
	string_utf8_append( inputDataFilename, ".txt" );

	// Open input data file
	u32 inputDataFile = platform_open_file( inputDataFilename, FILE_OPTION_READ );

	if ( inputDataFile == INVALID_INDEX_UINT_32 )
	{
		platform_close_file( inputDataFile );
		show_log_warning( "Failed to open data file: %s", inputDataFilename );
		return usage_message( RESULT_CODE_FAILED_TO_OPEN_INPUT_DATA_FILE );
	}
	else if ( options.verbose )
	{
		show_log_message( "File opened: %s", inputDataFilename );
	}

	// -----------------------------------------------------------------------------
	// Parse the data file for font information

	FontHeader header = {};
	FontChars chars;
	FontKernings kernings;

	string_utf8_copy( header.textureName, string_utf8_get_filename( options.outputFile ) );
	string_utf8_trim_ext( header.textureName );

	header.sdfBufferSize = static_cast<u16>( options.sdfBufferSize );

	if ( string_utf8_has_ext( options.inputFile, "ttf" ) )
	{
		parse_ttf_font_data_file( inputDataFile, &header, &chars, &kernings, &arena );
	}
	else if ( string_utf8_has_ext( options.inputFile, "png" ) )
	{
		parse_img_font_data_file( inputDataFile, &header, &chars, &kernings, &arena );
	}
	else
	{
		show_log_warning( "Unknown extension type for: %s", inputDataFile );
		return usage_message( RESULT_CODE_UNKNOWN_EXTENSION );
	}

	if ( options.verbose )
		show_log_message( "Data file parsed: %s", inputDataFilename );

	// -----------------------------------------------------------------------------
	// Create the sdf image

	u32 imgWidth;
	u32 imgHeight;
	u32 imgChannels = 1;

	u8 *image = platform_read_image( options.inputFile, &imgWidth, &imgHeight, &imgChannels );

	const u64 imageSize = imgWidth * imgHeight * imgChannels;

	if ( !image )
	{
		show_log_warning( "Failed to open file: %s", options.inputFile );
		return usage_message( RESULT_CODE_FAILED_TO_OPEN_INPUT_FILE );
	}
	if ( imageSize <= 0 )
	{
		show_log_warning( "Failed to open file: %s ( 0 bytes )", options.inputFile );
		return usage_message( RESULT_CODE_FAILED_TO_OPEN_INPUT_FILE );
	}
	else if ( options.verbose )
	{
		show_log_message( "Read %d bytes for file: %s", imageSize, options.inputFile );
	}

	const u32 charWidth = header.maxCharW + options.sdfBufferSize * 2;
	const u32 charHeight = header.maxCharH + options.sdfBufferSize * 2;
	const u32 charNumX = options.maxW / charWidth;
	const u32 charNumY = static_cast<u32>( ceil( static_cast<f32>( header.maxChars ) / charNumX ) );

	const u32 outWidth = charNumX * charWidth;
	const u32 outHeight = charNumY * charHeight;
	const u32 outChannels = 4;
	const u32 outImageSize = outWidth * outHeight * outChannels;

	u8 *outImage = memory_arena_transient_allocate( &arena, outImageSize );

	if ( !outImage )
	{
		show_log_warning( "Failed to allocate &d bytes.", outImageSize );
		return usage_message( RESULT_CODE_FAILED_TO_ALLOCATE_MEMORY_FOR_OUTPUT_IMAGE );
	}

	platform_memzero( outImage, outImageSize );

	const int charsPerLine = imgWidth / header.gridW;
	constexpr const f32 startBlend = 0.5f;
	constexpr const f32 endBlend = 0.05f;
	constexpr const f32 diffBlend = ( startBlend - endBlend );

	for ( int i = 0, maxChars = header.maxChars; i < maxChars; ++i )
	{
		FontChar &charData = chars[ i ];

		u32 outGridPosX = ( i % charNumX );
		u32 outGridPosY = ( i / charNumX );
		i32 startX = ( outGridPosX * charWidth ) + ( charWidth / 2 ) - ( charData.bearingX + charData.width / 2 );
		i32 startY = ( outGridPosY * charHeight ) + ( charHeight / 2 ) - ( header.originLine - charData.bearingY ) - charData.height / 2;

		massert( startX >= 0 );
		massert( startY >= 0 );

		bool blankChar = false;

		// Draw the character
		int px = ( i % charsPerLine ) * header.gridW;
		int py = ( i / charsPerLine ) * header.gridH;

		for ( int y = 0; y < header.gridH; ++y )
		{
			for ( int x = 0; x < header.gridW; ++x )
			{

				u32 idx = ( px + x + ( py + y ) * imgWidth ) * imgChannels;
				u32 drawOffset = ( startX + x + ( startY + y ) * outWidth ) * outChannels;

				massert( idx < imageSize );
				massert( drawOffset + 3 < outImageSize );

				u8 r = image[ idx ];

				outImage[ drawOffset ] = r;
				outImage[ drawOffset + 1 ] = r;
				outImage[ drawOffset + 2 ] = r;
				outImage[ drawOffset + 3 ] = r;

				blankChar = ( blankChar && r == 0 );
			}
		}

		// If it was a blank character, there is no sdf
		if ( blankChar )
			continue;

		// If the sdf size if 0, then there is no sdf
		if ( options.sdfBufferSize == 0 )
			continue;

		i32 sdfBufferSize = static_cast<i32>( options.sdfBufferSize );
		f32 sdfBufferSizef = static_cast<f32>( sdfBufferSize );

		startX = ( outGridPosX * charWidth );
		startY = ( outGridPosY * charHeight );

		// sdf
		for ( u32 y = 0; y < charHeight; ++y )
		{
			for ( u32 x = 0; x < charWidth; ++x )
			{
				u32 offset = ( startX + x + ( startY + y ) * outWidth ) * outChannels;

				massert( offset + 3 < outImageSize );

				//u8 r = outImage[ offset ];
				//u8 g = outImage[ offset + 1 ];
				//u8 b = outImage[ offset + 2 ];
				u8 a = outImage[ offset + 3 ];

				if ( a != 255 )
					continue;

				int sectionXEnd = startX + x + charWidth - 1;
				int sectionYEnd = startY + y + charHeight - 1;

				for ( int col = -sdfBufferSize; col <= sdfBufferSize; ++col )
				{
					for ( int row = -sdfBufferSize; row <= sdfBufferSize; ++row )
					{
						int xx = startX + x + col;
						int yy = startY + y + row;

						// Check its sampling from this characters section
						if ( xx < 0 || xx > sectionXEnd || yy < 0 || yy > sectionYEnd )
							continue;

						i32 idx = ( xx + yy * outWidth ) * outChannels;

						massert( idx + 3 < static_cast<i32>( outImageSize ) );

						f32 factor = sqrtf( static_cast<f32>( col * col + row * row ) ) / sdfBufferSizef;

						if ( factor <= 1.0f )
						{
							u8 oldA = outImage[ idx + 3 ];

							a = static_cast<u8>( ( startBlend - factor * diffBlend ) * 255 );

							if ( a > oldA )
							{
								outImage[ idx ] = a;
								outImage[ idx + 1 ] = a;
								outImage[ idx + 2 ] = a;
								outImage[ idx + 3 ] = a;
							}
						}
					}
				}
			}
		}
	}

	if ( options.verbose )
		show_log_message( "Finished creating sdf image. Preparing to save to disk." );

	header.textureOffsetX = options.injectX;
	header.textureOffsetY = options.injectY;
	header.textureWidth = outWidth;
	header.textureHeight = outHeight;

	// -----------------------------------------------------------------------------
	// Write the binary data file out
	{
		FileHeader dataHeader;
		dataHeader.version = 0;
		set_magic_value( dataHeader.id, FONT_MAGIC_VALUE );
		dataHeader.flags = FILE_HEADER_FLAG_COMPRESSED;

		u64 dataBufferSize = sizeof( FontHeader ) + chars.bytes() + kernings.bytes();
		u8 *dataBuffer = memory_arena_transient_allocate( &arena, dataBufferSize );

		if ( !dataBuffer )
		{
			show_log_warning( "Failed to allocate %d bytes.", dataBufferSize );
			return usage_message( RESULT_CODE_FAILED_TO_CREATE_OUTPUT_DATA_FILE );
		}

		// Adjust the bearing relative to the new image size
		for ( int i = 0, maxChars = header.maxChars; i < maxChars; ++i )
		{
			FontChar &charData = chars[ i ];
			charData.bearingX -= static_cast<i16>( options.sdfBufferSize );
			charData.bearingY += static_cast<i16>( options.sdfBufferSize );
		}

		u8 *dataBufferPtr = dataBuffer;
		write_data( dataBufferPtr, &header, sizeof( header ) );
		write_data( dataBufferPtr, chars.data, chars.bytes() );
		write_data( dataBufferPtr, kernings.data, kernings.bytes() );

		if ( !save_file( options.outputDataFile, dataBuffer, dataBufferSize, &dataHeader, &arena ) )
		{
			show_log_warning( "Failed to create output data file: %s", options.outputDataFile );
			return usage_message( RESULT_CODE_FAILED_TO_CREATE_OUTPUT_DATA_FILE );
		}
		else if ( options.verbose )
			show_log_message( "Output Datafile written to disk: %s", options.outputDataFile );

		memory_arena_transient_free( &arena, dataBuffer );
	}

	// -----------------------------------------------------------------------------
	// Write the sdf image out

	// Check if it's injected into another image or creates its own file
	if ( options.inject )
	{
		u32 width;
		u32 height;
		u32 channels = 4;

		u8 *injectImage = platform_read_image( options.outputFile, &width, &height, &channels );

		const u64 injectImageSize = width * height * channels;

		// Attempt to read in the image that will be written to
		if ( !injectImage || injectImageSize <= 0 )
		{
			show_log_warning( "Failed to open output image: %s", options.outputFile );
			return usage_message( RESULT_CODE_FAILED_TO_OPEN_OUTPUT_FILE );
		}

		u32 startX = options.injectX;
		u32 startY = options.injectY;

		if ( ( startX + outWidth ) > width || ( startY + outHeight ) > height )
		{
			show_log_warning( "Failed to inject image with dimensions [ %d x %d ] at [ %d x %d ]: %s", outWidth, outHeight, startX, startY, options.outputFile );
			return usage_message( RESULT_CODE_FAILED_TO_INJECT_IMAGE );
		}

		for ( u32 y = 0; y < outHeight; ++y )
		{
			for ( u32 x = 0; x < outWidth; ++x )
			{
				u32 idx = ( startX + x + ( startY + y ) * width ) * channels;
				u32 outIdx = ( x + y * outWidth ) * channels;

				massert( idx < injectImageSize );
				massert( outIdx < outImageSize );

				injectImage[ idx ] = outImage[ outIdx ];
				injectImage[ idx + 1 ] = outImage[ outIdx + 1 ];
				injectImage[ idx + 2 ] = outImage[ outIdx + 2 ];
				injectImage[ idx + 3 ] = outImage[ outIdx + 3 ];
			}
		}

		if ( !stbi_write_png( options.outputFile, width, height, channels, injectImage, width * channels ) )
		{
			show_log_warning( "Failed to inject output image: %s", options.outputFile );
			return usage_message( RESULT_CODE_FAILED_TO_CREATE_OUTPUT_FILE );
		}
		else if ( options.verbose )
			show_log_message( "Successfully injected output image[ %d x %d ]: %s", outWidth, outHeight, options.outputFile );
	}
	else
	{
		platform_create_directory( options.outputFile );

		if ( !stbi_write_png( options.outputFile, outWidth, outHeight, outChannels, outImage, outWidth * outChannels ) )
		{
			show_log_warning( "Failed to create output image: %s", options.outputFile );
			return usage_message( RESULT_CODE_FAILED_TO_CREATE_OUTPUT_FILE );
		}
		else if ( options.verbose )
			show_log_message( "Successfully created output image[ %d x %d ]: %s", outWidth, outHeight, options.outputFile );
	}

	// -----------------------------------------------------------------------------

	platform_close_file( inputDataFile );

#ifdef DEBUG
	system( "pause" );
#endif

	return 0;
}
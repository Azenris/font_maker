
#pragma once

constexpr const u64 INVALID_FILE_INDEX = INVALID_INDEX_UINT_64;
constexpr const u64 MAX_FIND_FILES = 4096;

using FilePermissions = u8;
enum FILE_PERMISSION : FilePermissions
{
	FILE_PERMISSION_READ	= BIT( 0 ),
	FILE_PERMISSION_WRITE	= BIT( 1 ),
};

using FileOptions = u32;
enum FILE_OPTION : FileOptions
{
	FILE_OPTION_READ					= BIT( 0 ),			// open the file for reading
	FILE_OPTION_WRITE					= BIT( 1 ),			// open the file for writing
	FILE_OPTION_CREATE					= BIT( 2 ),			// create the file if it doesnt exist
	FILE_OPTION_APPEND					= BIT( 3 ),			// start at the end of the file
	FILE_OPTION_CLEAR					= BIT( 4 ),			// the file is cleared
};

using FileSeek = u32;
enum FILE_SEEK : FileSeek
{
	FILE_SEEK_START,
	FILE_SEEK_CURRENT,
	FILE_SEEK_END,
};

using FileMove = u32;
enum FILE_MOVE : FileMove
{
	FILE_MOVE_REPLACE					= BIT( 0 ),			// if the target file already exists replace it
	FILE_MOVE_ERROR_LOG					= BIT( 1 ),			// if an error occurs it logs it
};

using FileCopy = u32;
enum FILE_COPY : FileCopy
{
	FILE_COPY_ERROR_LOG					= BIT( 1 ),			// if an error occurs it logs it
};

enum FILE_HEADER_FLAG : u32
{
	FILE_HEADER_FLAG_BASE64_ENCODED		= BIT( 0 ),
	FILE_HEADER_FLAG_COMPRESSED			= BIT( 1 ),
};
using FileHeaderFlags = u32;

struct FileHeader
{
	u32 version;
	char id[ 4 ];
	u64 decodedSize;
	u64 uncompressedSize;
	FileHeaderFlags flags;
};

// Directories & Paths
bool make_directory( const char *directory );
bool delete_directory( const char *directory );
bool change_directory( const char *directory );
[[nodiscard]] char *get_directory();
[[nodiscard]] char *get_relative_filename( char *path );
[[nodiscard]] const char *get_relative_filename( const char *path );
bool get_files_in_directory( const char *path, Array<const char *, MAX_FIND_FILES> *files, bool recursive, Allocator *allocator );

// Files
[[nodiscard]] bool file_exists( const char *path );
[[nodiscard]] u8 *read_file( const char *path, u64 *fileSize, bool addNullTerminator, Allocator *allocator );
u64 write_file( const char *path, const u8 *buffer, u64 size, bool append );
bool delete_file( const char *path );
[[nodiscard]] bool file_permissions( const char *path, FilePermissions permissions );
u64 open_file( const char *path, FileOptions options );
void close_file( u64 fileID );
u64 get_file_size( u64 fileID );
bool seek_in_file( u64 fileID, FileSeek seek, u64 offset );
u8 *read_whole_file( u64 fileID, Allocator *allocator, bool addNullTerminator );
u8 *read_image( const char *path, u32 *width, u32 *height, u32 *channels );
u64 read_from_file( u64 fileID, void *buffer, u64 size );
u64 write_to_file( u64 fileID, void *buffer, u64 size );
[[nodiscard]] u64 file_last_edit_timestamp( const char *path );
bool move_file( const char *from, const char *to, FileMove move = FILE_MOVE_ERROR_LOG );
bool move_file_retry( const char *from, const char *to, FileMove move = FILE_MOVE_ERROR_LOG, i32 attempts = 3, i32 msWaitPerAttempt = 0 );
bool copy_file( const char *from, const char *to, FileCopy copy = FILE_COPY_ERROR_LOG );
bool copy_file_retry( const char *from, const char *to, FileCopy copy = FILE_COPY_ERROR_LOG, i32 attempts = 3, i32 msWaitPerAttempt = 0 );

// Saving/Loading
bool save_file( const char *path, void *data, u64 size, FileHeader *header, Allocator *allocator );
[[nodiscard]] u8 *load_file( const char *path, u64 *fileSize, FileHeader *header, Allocator *allocator );
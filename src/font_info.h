
#pragma once

#pragma pack(push, 1)

static constexpr const u64 MAX_FONT_CODEPOINTS = 2048;
static constexpr const u64 MAX_FONT_KERNINGS = 512;
static constexpr const u64 MAX_FONT_TEXTURE_NAME = 256;
static constexpr const char *FONT_MAGIC_VALUE = "RMFF";

struct FontHeader
{
	u16 sdfBufferSize;							// size of the sdf area around the glyph
	u16 maxCharW;								// biggest character width
	u16 maxCharH;								// biggest character height
	u16 gridW;									// original input grid sizing
	u16 gridH;									// original input grid sizing
	u16 originLine;
	u32 maxChars;
	u32 maxKernings;
	char textureName[ MAX_FONT_TEXTURE_NAME ];	// name of the texture image
	u32 textureOffsetX;							// x position within the texture it starts
	u32 textureOffsetY;							// y position within the texture it starts
	u32 textureWidth;							// width of the generated image
	u32 textureHeight;							// height of the generated image
};

struct FontChar
{
	u32 codepoint;								// UTF-8 codepoint
	i16 bearingX;
	i16 bearingY;
	u16 offsetY;								// offset the y during rendering
	u16 width;
	u16 height;
	u16 advanceX;
};

struct FontKerning
{
	u32 first;									// first character codepoint
	u32 second;									// second character codepoint
	i32 offsetX;								// x axis adjustment
	u8 reserved[ 4 ];							// unused
};

using FontChars = Array<FontChar, MAX_FONT_CODEPOINTS>;
using FontKernings = Array<FontKerning, MAX_FONT_KERNINGS>;

// ----------------------------------------

static_assert( sizeof( FontChar ) % sizeof( u64 ) == 0 );
static_assert( sizeof( FontKerning ) % sizeof( u64 ) == 0 );

#pragma pack(pop)
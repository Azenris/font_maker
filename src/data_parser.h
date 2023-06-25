
static RESULT_CODE parse_img_font_data_file( u32 fileID, FontHeader *header, FontChars *pChars, FontKernings *pKernings, MemoryArena *arena )
{
	constexpr const int maxTokenslength = 32;
	constexpr const int maxTokensPerLine = 32;
	using TokenArray = Array<Array<char, maxTokenslength>, maxTokensPerLine>;
	using func = RESULT_CODE(*)( const TokenArray &, FontHeader *, FontChars *, FontKernings * );

	Map<const char *, func, 16> inputCommand;

	inputCommand.insert( "info", [] ( const TokenArray &tokens, FontHeader *header, FontChars *pChars, FontKernings *pKernings )
		{
			int index = 0;

			header->gridW = static_cast<u16>( convert_to_int( tokens[ index++ ].data ) );
			header->gridH = static_cast<u16>( convert_to_int( tokens[ index++ ].data ) );
			header->originLine = static_cast<u16>( convert_to_int( tokens[ index++ ].data ) );

			return RESULT_CODE_SUCCESS;
		} );

	inputCommand.insert( "char", [] ( const TokenArray &tokens, FontHeader *header, FontChars *pChars, FontKernings *pKernings )
		{
			int index = 0;

			FontChar data = {};

			data.codepoint = convert_to_int( tokens[ index++ ].data );
			data.bearingX = static_cast<i16>( convert_to_int( tokens[ index++ ].data ) );
			data.bearingY = static_cast<i16>( convert_to_int( tokens[ index++ ].data ) );
			data.offsetY = static_cast<i16>( convert_to_int( tokens[ index++ ].data ) );
			data.width = static_cast<u16>( convert_to_int( tokens[ index++ ].data ) );
			data.height = static_cast<u16>( convert_to_int( tokens[ index++ ].data ) );
			data.advanceX = static_cast<u16>( convert_to_int( tokens[ index++ ].data ) );

			++header->maxChars;

			header->maxCharW = max( header->maxCharW, data.width );
			header->maxCharH = max( header->maxCharH, data.height );

			pChars->add( data );

			return RESULT_CODE_SUCCESS;
		} );

	inputCommand.insert( "kerning", [] ( const TokenArray &tokens, FontHeader *header, FontChars *pChars, FontKernings *pKernings )
		{
			int index = 0;

			FontKerning kerning = {};

			kerning.first = convert_to_int( tokens[ index++ ].data );
			kerning.second = convert_to_int( tokens[ index++ ].data );
			kerning.offsetX = convert_to_int( tokens[ index++ ].data );

			++header->maxKernings;

			pKernings->add( kerning );

			return RESULT_CODE_SUCCESS;
		} );

	// -------------------------------------------------------

	u8 *buffer = platform_read_whole_file( fileID, arena, true );

	if ( !buffer )
	{
		show_log_error( error_code_string( RESULT_CODE_PARSE_DATA_FILE_FAILED_TO_READ_FILE ) );
		return RESULT_CODE_PARSE_DATA_FILE_FAILED_TO_READ_FILE;
	}

	const char *token = nullptr;
	char *tok = string_utf8_tokenise( (char *)buffer, " \t\n", &token );
	TokenArray lineTokens;

	while ( token )
	{
		// Ignore blank lines
		if ( *token != '\0' && *token != '\n' )
		{
			// Check if a comment
			if ( *token == ';' )
			{
				// Find end of line, then get the next token (Don't combine, its one after the other, not either)
				tok = string_utf8_tokenise( string_utf8_tokenise( tok, "\n", &token ), " \t", &token );
				continue;
			}

			tok = string_utf8_tokenise_line( tok, " \t\n", lineTokens );

			// Check some tokens were read in
			if ( lineTokens.count == 0 )
				continue;

			auto f = inputCommand.find( token );

			if ( f )
			{
				RESULT_CODE errorCode = f->value( lineTokens, header, pChars, pKernings );

				if ( errorCode != RESULT_CODE_SUCCESS )
				{
					show_log_error( "Function \"parse_img_font_data_file\" failed. Unable to process input: %s", token );
					show_log_error( error_code_string( errorCode ) );
					return RESULT_CODE_PARSE_DATA_FILE_FAILED;
				}
			}
			else
			{
				show_log_error( "Function \"parse_img_font_data_file\" failed. Unknown input: %s", token );
				show_log_error( error_code_string( RESULT_CODE_PARSE_DATA_FILE_FAILED ) );
				return RESULT_CODE_PARSE_DATA_FILE_FAILED;
			}
		}

		tok = string_utf8_tokenise( tok, " \t\n", &token );
	}

	memory_arena_transient_free( arena, buffer );

	return RESULT_CODE_SUCCESS;
}

RESULT_CODE parse_ttf_font_data_file( u32 fileID, FontHeader *header, FontChars *pChars, FontKernings *pKernings, MemoryArena *arena )
{
	// TODO parse_ttf_font_data_file
	show_log_error( error_code_string( RESULT_CODE_PARSE_DATA_FILE_UNSUPPORTED ) );
	return RESULT_CODE_PARSE_DATA_FILE_UNSUPPORTED;
}
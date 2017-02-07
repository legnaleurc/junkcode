#include <tchar.h>
#include <Windows.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

std::wstring fromUTF8( const std::string & utf8 ) {
	wchar_t unicode[_MAX_PATH];
	int ret = MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		utf8.c_str(),
		-1,
		unicode,
		sizeof( unicode ) / sizeof( *unicode )
	);
	if( ret == 0 ) {
		return L"";
	}
	return unicode;
}

std::string toLocal8Bits( const std::wstring & unicode ) {
	char local8Bits[_MAX_PATH];
	int ret = WideCharToMultiByte(
		CP_OEMCP,
		0,
		unicode.c_str(),
		-1,
		local8Bits,
		sizeof( local8Bits ) / sizeof( *local8Bits ),
		NULL,
		NULL
	);
	if( ret == 0 ) {
		return "";
	}
	return local8Bits;
}

std::string toUTF8( const std::wstring & unicode ) {
	char urf8[_MAX_PATH];
	int ret = WideCharToMultiByte(
		CP_UTF8,
		0,
		unicode.c_str(),
		-1,
		urf8,
		sizeof( urf8 ) / sizeof( *urf8 ),
		NULL,
		NULL
	);
	if( ret == 0 ) {
		return "";
	}
	return urf8;
}

std::wstring getFileName( const std::wstring & path ) {
	wchar_t drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	errno_t ret = _wsplitpath_s( path.c_str(), drive, dir, fname, ext );
	if( ret != 0 ) {
		return L"";
	}
	return fname;
}

int wmain( int argc, wchar_t * argv[] ) {
	typedef std::wstring String;
	typedef std::vector< String > StringList;

	StringList args;
	std::copy( argv, argv + argc, back_inserter( args ) );
	const String appName( getFileName( args[0] ) );

	if( args.size() != 4 ) {
		std::cerr << "Usage: " << toLocal8Bits( appName ) << " file.list oldQtDir newQtDir" << std::endl;
		return EXIT_FAILURE;
	}

	const String fileList = args[1];
	const String qtDirPath = args[2];
	const String newQtPath = args[3];
	const std::string lQtDirPath( toLocal8Bits( qtDirPath ) );
	const std::string lNewQtPath( toLocal8Bits( newQtPath ) );

	if( lQtDirPath.size() < lNewQtPath.size() ) {
		std::cerr
			<< toLocal8Bits( appName )
			<< ": error: newQtDir needs to be less than "
			<< lQtDirPath.size() << " characters."
			<< std::endl
		;
		return EXIT_FAILURE;
	}

	std::ifstream fn( fileList );
	if ( !fn.is_open() ) {
		std::cerr << toLocal8Bits( appName ) << ": error: file not found" << std::endl;
		return EXIT_FAILURE;
	}

	StringList binaryFiles, textFiles;
	bool readingTextFilesToPatch = false;

	// read the input file
	std::string line;
	while( std::getline( fn, line ) ) {
		if( line.empty() ) {
			continue;
		}

		if( line.compare( 0, 2, "%%" ) == 0 ) {
			readingTextFilesToPatch = true;
		} else if( readingTextFilesToPatch ) {
			textFiles.push_back( fromUTF8( line ) );
		} else {
			binaryFiles.push_back( fromUTF8( line ) );
		}
	}

	std::for_each( binaryFiles.begin(), binaryFiles.end(), [ &appName, &qtDirPath, &newQtPath, &lQtDirPath, &lNewQtPath ]( const String & fileName ) {
		typedef std::wstring String;
		typedef std::vector< char > ByteArray;

		String prefix = newQtPath;

		if( prefix.back() != L'\\' ) {
			prefix += L'\\';
		}
		String sFileName( fileName );
		for( std::size_t i = sFileName.find( L"/", 0 ); i != String::npos; i = sFileName.find( L"/", i ) ) {
			sFileName.replace( i, 1, L"\\" );
		}

		sFileName = prefix + sFileName;

		std::ifstream fin( fileName, std::ios::binary );
		if( !fin.is_open() ) {
			std::cerr << toLocal8Bits( appName ) << ": warning: file `" << toLocal8Bits( fileName ) << "' not found" << std::endl;
			return;
		}

		ByteArray source;
		std::copy( std::istreambuf_iterator< char >( fin ), std::istreambuf_iterator< char >(), std::back_inserter( source ) );
		fin.close();

		std::ofstream fout( fileName, std::ios::binary );
		if( !fout.is_open() ) {
			std::cerr << toLocal8Bits( appName ) << ": error: file `" << toLocal8Bits( fileName ) << "' not writable" << std::endl;
			return;
		}
		
		ByteArray::const_iterator srcIndex( source.begin() );
		ByteArray::const_iterator srcEnd( source.end() );
		for(;;) {
			ByteArray::const_iterator token = std::search( srcIndex, srcEnd, lQtDirPath.begin(), lQtDirPath.end() );
			if( token == source.end() ) {
				std::cout << "`" << toLocal8Bits( fileName ) << "' need not to change" << std::endl;
				break;
			}
			std::cout << "`" << toLocal8Bits( fileName ) << "' found" << std::endl;

			ByteArray::const_iterator endOfToken = token;
			std::advance( endOfToken, lQtDirPath.size() );
			endOfToken = std::find( endOfToken, srcEnd, '\0' );
			if( endOfToken == srcEnd ) {
				std::cerr << toLocal8Bits( appName ) << ": error: file `" << toLocal8Bits( fileName ) << "' is invalid" << std::endl;
				// FIXME exception
				break;
			}
			// reserve a null char
			++endOfToken;

			if( srcIndex != token ) {
				// if there are bytes between srcIndex and token, fill them
				fout.write( static_cast< const char * >( &*srcIndex ), std::distance( srcIndex, token ) );
			}

			// get the original string length
			std::size_t length = std::distance( token, endOfToken );
			// new path prefix
			std::vector< char > s( lNewQtPath.begin(), lNewQtPath.end() );

			ByteArray::const_iterator b( token );
			std::advance( b, lQtDirPath.size() );
			// concate last part
			s.insert( s.end(), b, endOfToken );
			// fill zero until it equals to original block
			s.insert( s.end(), length - s.size(), '\0' );

#ifdef _DEBUG
			std::cerr << "replace string: " << &*token << " with: " << &s[0] << std::endl;
#endif
			fout.write( static_cast< const char * >( &s[0] ), s.size() );

			srcIndex = endOfToken;
		}

		if( srcIndex != source.end() ) {
			fout.write( static_cast< const char * >( &*srcIndex ), std::distance( srcIndex, srcEnd ) );
		}
	} );

	std::for_each( textFiles.begin(), textFiles.end(), [ &appName, &qtDirPath, &newQtPath ]( const String & fileName ) {
		typedef std::wstring String;
		String prefix = newQtPath;

		if( prefix.back() != L'\\' ) {
			prefix += L'\\';
		}
		String sFileName( fileName );
		for( std::size_t i = fileName.find( L"/", 0 ); i != String::npos; i = fileName.find( L"/", i ) ) {
			sFileName.replace( i, 1, L"\\" );
		}

		sFileName = prefix + sFileName;

		std::ifstream fin( fileName );
		if ( !fin.is_open() ) {
			std::cerr << toLocal8Bits( appName ) << L": warning: file `" << toLocal8Bits( fileName ) << L"' not found" << std::endl;
			return;
		}
		std::vector< std::string > text;
		std::string line;
		while( std::getline( fin, line ) != 0 ) {
			String tmp( fromUTF8( line ) );
			for( std::size_t i = tmp.find( qtDirPath, 0 ); i != String::npos; i = tmp.find( qtDirPath, i ) ) {
				tmp.replace( i, qtDirPath.size(), newQtPath );
#ifdef _DEBUG
				std::cerr << "replace string: " << toLocal8Bits( qtDirPath ) << " with: " << toLocal8Bits( newQtPath ) << std::endl;
#endif
			}
			text.push_back( toUTF8( tmp ) );
		}
		fin.close();

		std::ofstream fout( fileName );
		if( !fout.is_open() ) {
			std::cerr << toLocal8Bits( appName ) << L": error: file `" << toLocal8Bits( fileName ) << L"' not writable" << std::endl;
			return;
		}
		std::for_each( text.begin(), text.end(), [&fout]( const std::string & line ) {
			fout << line << std::endl;
		} );
	} );

	return 0;
}

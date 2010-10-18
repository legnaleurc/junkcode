#include <tchar.h>
#include <Windows.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

template< typename CharType >
struct StreamTrait {
	typedef std::basic_string< CharType > String;
	typedef std::basic_ifstream< CharType, std::char_traits< CharType > > IFStream;
	typedef std::basic_ofstream< CharType, std::char_traits< CharType > > OFStream;

	static std::basic_istream< CharType, std::char_traits< CharType > > & in;
	static std::basic_ostream< CharType, std::char_traits< CharType > > & out;
	static std::basic_ostream< CharType, std::char_traits< CharType > > & err;
	
	static String fromUTF8( const std::string & utf8 );
	static std::string toLocal8Bits( const String & unicode );
	static std::string toUTF8( const String & unicode );
};

std::basic_istream< char, std::char_traits< char > > & StreamTrait< char >::in = std::cin;
std::basic_ostream< char, std::char_traits< char > > & StreamTrait< char >::out = std::cout;
std::basic_ostream< char, std::char_traits< char > > & StreamTrait< char >::err = std::cerr;
template<>
StreamTrait< char >::String StreamTrait< char >::fromUTF8( const std::string & utf8 ) {
	return utf8;
}
template<>
std::string StreamTrait< char >::toLocal8Bits( const StreamTrait< char >::String & unicode ) {
	return unicode;
}
template<>
std::string StreamTrait< char >::toUTF8( const StreamTrait< char >::String & unicode ) {
	return unicode;
}

std::basic_istream< wchar_t, std::char_traits< wchar_t > > & StreamTrait< wchar_t >::in = std::wcin;
std::basic_ostream< wchar_t, std::char_traits< wchar_t > > & StreamTrait< wchar_t >::out = std::wcout;
std::basic_ostream< wchar_t, std::char_traits< wchar_t > > & StreamTrait< wchar_t >::err = std::wcerr;
template<>
StreamTrait< wchar_t >::String StreamTrait< wchar_t >::fromUTF8( const std::string & utf8 ) {
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
template<>
std::string StreamTrait< wchar_t >::toLocal8Bits( const String & unicode ) {
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
template<>
std::string StreamTrait< wchar_t >::toUTF8( const String & unicode ) {
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

StreamTrait< _TCHAR >::String getFileName( const StreamTrait< _TCHAR >::String & path ) {
	_TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	errno_t ret = _tsplitpath_s( path.c_str(), drive, dir, fname, ext );
	if( ret != 0 ) {
		return _T( "" );
	}
	return fname;
}

int _tmain( int argc, _TCHAR * argv[] ) {
	using std::copy;
	using std::back_inserter;
	using std::getline;
	using std::for_each;
	using std::endl;
	using std::ifstream;
	using std::ofstream;
	using std::string;
	typedef StreamTrait< _TCHAR > Stream;
	typedef std::vector< Stream::String > StringList;

	StringList args;
	copy( argv, argv + argc, back_inserter( args ) );
	const Stream::String appName( getFileName( args[0] ) );

	if( args.size() != 4 ) {
		Stream::err << _T( "Usage: " ) << appName << _T( " file.list oldQtDir newQtDir" ) << endl;
		return EXIT_FAILURE;
	}

	const Stream::String files = args[1];
	const Stream::String qtDirPath = args[2];
	const Stream::String newQtPath = args[3];
	const std::string lQtDirPath( Stream::toLocal8Bits( qtDirPath ) );
	const std::string lNewQtPath( Stream::toLocal8Bits( newQtPath ) );

	if( lQtDirPath.size() < lNewQtPath.size() ) {
		Stream::err
			<< appName
			<< _T( ": error: newQtDir needs to be less than " )
			<< lQtDirPath.size() << _T( " characters." )
			<< endl
		;
		return EXIT_FAILURE;
	}

	ifstream fn( files );
	if ( !fn.is_open() ) {
		Stream::err << appName << ": error: file not found" << endl;
		return EXIT_FAILURE;
	}

	StringList filesToPatch, textFilesToPatch;
	bool readingTextFilesToPatch = false;

	// read the input file
	string line;
	while( getline( fn, line ) ) {
		if( line.empty() ) {
			continue;
		}

		if( line.compare( 0, 2, "%%" ) == 0 ) {
			readingTextFilesToPatch = true;
		} else if( readingTextFilesToPatch ) {
			textFilesToPatch.push_back( Stream::fromUTF8( line ) );
		} else {
			filesToPatch.push_back( Stream::fromUTF8( line ) );
		}
	}

	for_each( filesToPatch.begin(), filesToPatch.end(), [ &appName, &qtDirPath, &newQtPath, &lQtDirPath, &lNewQtPath ]( Stream::String fileName ) {
		typedef StreamTrait< _TCHAR > Stream;
		typedef std::vector< char > ByteArray;

		Stream::String prefix = newQtPath;

		if( prefix.back() != _T( '\\' ) ) {
			prefix += _T( '\\' );
		}
		for( std::size_t i = fileName.find( _T( "/" ), 0 ); i != Stream::String::npos; i = fileName.find( _T( "/" ), i ) ) {
			fileName.replace( i, 1, _T( "\\" ) );
		}

		fileName = prefix + fileName;

		ifstream fin( fileName, std::ios::binary );
		if( !fin.is_open() ) {
			Stream::err << appName << _T( ": warning: file `" ) << fileName << _T( "' not found" ) << std::endl;
			return;
		}

		ByteArray source;
		using std::istreambuf_iterator;
		std::copy( istreambuf_iterator< char >( fin ), istreambuf_iterator< char >(), back_inserter( source ) );
		fin.close();

		ofstream fout( fileName, std::ios::binary );
		if( !fout.is_open() ) {
			Stream::err << appName << _T( ": error: file `" ) << fileName << _T( "' not writable" ) << std::endl;
			return;
		}
		
		ByteArray::iterator index( source.begin() );
		for(;;) {
			ByteArray::iterator start = std::search( index, source.end(), lQtDirPath.begin(), lQtDirPath.end() );
			if( start == source.end() ) {
				break;
			}

			ByteArray::iterator endOfString = start;
			std::advance( endOfString, lQtDirPath.size() );
			for( ; endOfString != source.end(); ++endOfString ) {
				if( *endOfString == '\0' ) {
					break;
				}
			}
			++endOfString;

			if( index != start )	{
				fout.write( static_cast< const char * >( &*index ), std::distance( index, start ) );
			}

			std::size_t length = std::distance( start, endOfString );
			std::vector< char > s;

			std::copy( lNewQtPath.begin(), lNewQtPath.end(), back_inserter( s ) );

			ByteArray::iterator b( start );
			for( std::advance( b, lQtDirPath.size() ); b != endOfString; ++b ) {
				s.push_back( *b );
			}

			for( std::size_t i = s.size(); i < length; ++i ) {
				s.push_back( '\0' );
			}

#if 0
			std::cout << "replace string: " << source.mid(start, endOfString - start).constData()
			<< " with: " << s.constData() << std::endl;
#endif
			fout.write( static_cast< const char * >( &s[0] ), s.size() );

			index = endOfString;
		}

		if( index != source.end() ) {
			fout.write( static_cast< const char * >( &*index ), std::distance( index, source.end() ) );
		}
	} );

	for_each( textFilesToPatch.begin(), textFilesToPatch.end(), [ &appName, &qtDirPath, &newQtPath ]( Stream::String fileName ) {
		typedef StreamTrait< _TCHAR > Stream;

		Stream::String prefix = newQtPath;

		if( prefix.back() != _T( '\\' ) ) {
			prefix += _T( '\\' );
		}
		for( std::size_t i = fileName.find( _T( "/" ), 0 ); i != Stream::String::npos; i = fileName.find( _T( "/" ), i ) ) {
			fileName.replace( i, 1, _T( "\\" ) );
		}

		fileName = prefix + fileName;

		ifstream fin( fileName );
		if ( !fin.is_open() ) {
			Stream::err << appName << _T( ": warning: file `" ) << fileName << _T( "' not found" ) << std::endl;
			return;
		}
		std::vector< std::string > text;
		std::string line;
		while( std::getline( fin, line ) != 0 ) {
			Stream::String tmp( Stream::fromUTF8( line ) );
			for( std::size_t i = tmp.find( qtDirPath, 0 ); i != Stream::String::npos; i = tmp.find( qtDirPath, i ) ) {
				tmp.replace( i, qtDirPath.size(), newQtPath );
			}
			text.push_back( Stream::toUTF8( tmp ) );
		}
		fin.close();

		ofstream fout( fileName );
		if( !fout.is_open() ) {
			Stream::err << appName << _T( ": error: file `" ) << fileName << _T( "' not writable" ) << std::endl;
			return;
		}
		std::for_each( text.begin(), text.end(), [&fout]( const std::string & line ) {
			fout << line << std::endl;
		} );
	} );

	return 0;
}

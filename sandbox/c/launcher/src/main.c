#include "os.h"

#include <windows.h>

#include <stdio.h>

// string buffer size
#define BUF_SIZE 1024
// cache file name
static const TCHAR * const CACHE = TEXT( ".cache" );

/**
 * @brief Load cache file
 * @param [out] buf string buffer
 * @param [in] len buffer size
 * @return 0 if succeed, otherwise returns error code.
 */
static errno_t load( TCHAR * buf, size_t len ) {
	FILE * fin = NULL;
	errno_t err = _tfopen_s( &fin, CACHE, TEXT( "r" ) );
	if( err != 0 ) {
		return err;
	}

	_fgetts( buf, len, fin );

	return fclose( fin );
}

int _tmain( int argc, TCHAR * argv[] ) {
	Context app;

	FILE * fout = NULL;
	TCHAR buffer[BUF_SIZE], tmp[BUF_SIZE];
	errno_t err = 0;

	getPaths( &app, argv[0] );
	cd( app.dirName );

	// loading cache
	show( CACHE );
	err = load( buffer, BUF_SIZE );
	if( err != 0 ) {
		// no cache file, send a warning
		_tcserror_s( tmp, BUF_SIZE, err );
		_stprintf_s( buffer, BUF_SIZE, TEXT( "Warning on loading %s: %s\n" ), CACHE, tmp );
		_ftprintf_s( stderr, buffer );

		err = findFile( buffer, BUF_SIZE, app.baseName, argv[0] );
		if( err != 0 ) {
			// error occured, about operation
			_tcserror_s( tmp, BUF_SIZE, err );
			_stprintf_s( buffer, BUF_SIZE, TEXT( "Error on finding %s: %s\n" ), app.baseName, tmp );
			_ftprintf_s( stderr, buffer );
			return err;
		}
	}

	// buffer is the target program path
	cd( buffer );
	_stprintf_s( tmp, BUF_SIZE, TEXT( "%s &" ), buffer );
	_tsystem( tmp );
	cd( app.dirName );

	// saving cache
	err = _tfopen_s( &fout, CACHE, TEXT( "w" ) );
	if( err != 0 ) {
		_tcserror_s( tmp, BUF_SIZE, err );
		_stprintf_s( buffer, BUF_SIZE, TEXT( "Error on saving %s: %s\n" ), CACHE, tmp );
		_ftprintf_s( stderr, buffer );
		return err;
	}
	_ftprintf_s( fout, TEXT( "%s" ), buffer );
	fclose( fout );
	hide( CACHE );

	return 0;
}

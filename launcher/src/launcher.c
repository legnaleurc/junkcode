#include "os.h"

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1024

int _tmain( int argc, TCHAR * argv[] ) {
	TCHAR pwd[_MAX_PATH], target[_MAX_PATH], config[_MAX_PATH];
	FILE * fin = NULL, * fout = NULL;

	TCHAR buffer[BUF_SIZE];

	getPaths( pwd, _MAX_PATH, target, _MAX_PATH, config, _MAX_PATH, argv[0] );

	if( _taccess_s( config, 2 ) == 0 ) {
		_tfopen_s( &fin, config, TEXT( "r" ) );
		if( fin == NULL ) {
			_ftprintf_s( stderr, TEXT( "Read file error.\n" ) );
			return 1;
		} else {
			_fgetts( buffer, BUF_SIZE, fin );
			fclose( fin );
			show( config );
			if( _taccess_s( buffer, 0 ) != 0 ) {
				findFile( buffer, BUF_SIZE, target, pwd, argv[0] );
			}
		}
	} else {
		findFile( buffer, BUF_SIZE, target, pwd, argv[0] );
	}

	cd( buffer );
	execute( buffer, argv );

	_tfopen_s( &fout, config, TEXT( "w" ) );
	if( fout == NULL ) {
		_ftprintf_s( stderr, TEXT( "Write file error.\n" ) );
		return 1;
	} else {
		_fputts( buffer, fout );
		fclose( fout );
	}

	hide( config );

	return 0;
}

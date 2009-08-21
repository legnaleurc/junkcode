#include "os.h"

#include <windows.h>

#include <stdio.h>
#include <errno.h>

void cd( const TCHAR * path ) {
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	TCHAR d[_MAX_PATH];

	_tsplitpath_s( path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT );
	_tmakepath_s( d, _MAX_PATH, drive, dir, NULL, NULL );

	_tchdir( d );
}

int hide( const TCHAR * path ) {
	return SetFileAttributes( path, GetFileAttributes( path ) | FILE_ATTRIBUTE_HIDDEN );
}

int show( const TCHAR * path ) {
	return SetFileAttributes( path, GetFileAttributes( path ) ^ FILE_ATTRIBUTE_HIDDEN );
}

int findFile( TCHAR * result, size_t len, const TCHAR * target, const TCHAR * from, const TCHAR * exclude ) {
	FILE * pin = NULL;
	errno_t err = ENOENT;

	_stprintf_s( result, len, TEXT( "DIR /B /S %s" ), target );
	pin = _tpopen( result, TEXT( "r" ) );

	while( _fgetts( result, len, pin ) != NULL ) {
		result[_tcslen(result)-1] = '\0';
		if( _tcscmp( result, exclude ) != 0 ) {
			err = 0;
			break;
		}
	}

	_pclose( pin );
	return err;
}

void getPaths( Context * app, const TCHAR * path ) {
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];

	_tsplitpath_s( path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT );
	_tmakepath_s( app->dirName, _MAX_PATH, drive, dir, NULL, NULL );
	_tmakepath_s( app->baseName, _MAX_PATH, NULL, NULL, fname, ext );
}

#include "os.h"
#include <direct.h>
#include <process.h>
#include <windows.h>
#include "util.h"
#include "queue.h"

void cd( const TCHAR * path ) {
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	TCHAR d[_MAX_PATH];

	_tsplitpath_s( path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT );
	_tmakepath_s( d, _MAX_PATH, drive, dir, NULL, NULL );

	_tchdir( d );
}

void hide( const TCHAR * path ) {
	SetFileAttributes( path, GetFileAttributes( path ) | FILE_ATTRIBUTE_HIDDEN );
}

void show( const TCHAR * path ) {
	SetFileAttributes( path, GetFileAttributes( path ) ^ FILE_ATTRIBUTE_HIDDEN );
}

int findFile( TCHAR * result, size_t len, const TCHAR * target, const TCHAR * from, const TCHAR * exclude ) {
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	DWORD dwAttrs;

	TCHAR fname[_MAX_PATH], tmp[_MAX_PATH];
	PathQueue paths = newPathQueue();
	int ret = 1;

	// Find first file in path\\* .
	hFind = FindFirstFile( concat( tmp, _MAX_PATH, from, TEXT( "*" ) ), &FindFileData );
	do {
		_tcscpy_s( fname, _MAX_PATH, FindFileData.cFileName );
		dwAttrs = GetFileAttributes( concat( tmp, _MAX_PATH, from, fname ) );

		// Don't walk directory "." and ".." .
		if( _tcscmp( fname, TEXT( "." ) ) != 0 && _tcscmp( fname, TEXT( ".." ) ) != 0 ) {
			// If this file is directory:
			if( ( dwAttrs & FILE_ATTRIBUTE_DIRECTORY ) ) {
				//findFile( result, len, target, tmp, exclude );
				_tcscat_s( tmp, _MAX_PATH, TEXT( "\\" ) );
				PQ_push( &paths, tmp );
			} else if( _tcsicmp( fname, target ) == 0 ) {
				if( _tcscmp( tmp, exclude ) != 0 ) {
					freePathQueue( &paths );
					_tcscpy_s( result, len, tmp );
					return 0;
				}
			}
		}
	} while( FindNextFile( hFind, &FindFileData ) );
	FindClose( hFind );

	while( ret != 0 && !PQ_empty( paths ) ) {
		ret = findFile( result, len, target, paths.front->path, exclude );
		PQ_pop( &paths );
	}
	freePathQueue( &paths );
	return ret;
}

intptr_t execute( const TCHAR * path, const TCHAR * const * args ) {
	return _tspawnv( _P_NOWAIT, path, args );
}

void getPaths( TCHAR * pwd, size_t pwd_size, TCHAR * target, size_t tgt_size, TCHAR * config, size_t conf_size, const TCHAR * arg0 ) {
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];

	_tsplitpath_s( arg0, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT );

	_tmakepath_s( pwd, pwd_size, drive, dir, NULL, NULL );

	_tmakepath_s( target, tgt_size, NULL, NULL, fname, ext );

	_tmakepath_s( config, conf_size, drive, dir, TEXT( "cache" ), TEXT( "ini" ) );
}

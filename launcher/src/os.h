#ifndef LEGNALEURC_OS_H
#define LEGNALEURC_OS_H

#include <tchar.h>

#include <stdlib.h>

typedef struct Context {
	TCHAR baseName[_MAX_PATH];
	TCHAR dirName[_MAX_PATH];
} Context;

void cd( const TCHAR * );

int hide( const TCHAR * path );

int show( const TCHAR * path );

int findFile( TCHAR * result, size_t len, const TCHAR * target, const TCHAR * from, const TCHAR * exclude );

void getPaths( Context * app, const TCHAR * path );

#endif

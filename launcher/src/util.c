#include "util.h"

TCHAR * concat( TCHAR * dst, size_t size, const TCHAR * left, const TCHAR * right ) {
	_tcscpy_s( dst, size, left );
	_tcscat_s( dst, size, right );
	return dst;
}

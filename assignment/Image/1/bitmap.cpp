#include "bitmap.hpp"

#include <stdexcept>
#include <cstring>

namespace {

inline int fromWord( char * w ) {
	int i;
	std::memcpy( &i, w, 2 );
	return i;
}

}

namespace pwc94u {

Bitmap::Bitmap():
offset_( 0 ),
headerSize_( 0 ),
planes_( 0 ),
nColorPalette_( 0 ) {
}
Bitmap::~Bitmap() {}

void Bitmap::parseHeader( std::istream & in ) {
	char header[54];
	in.read( header, 54 );
	char * tmp = header;

	if( tmp[0] != 'B' || tmp[1] != 'M' ) {
		throw std::runtime_error( "magic error" );
	}
	tmp += 2;

	this->setFileSize( fromDWord( tmp ) );
	tmp += 4;

	// ignore reserved bits
	tmp += 4;

	this->offset_ = fromDWord( tmp );
	tmp += 4;

	this->headerSize_ = fromDWord( tmp );
	tmp += 4;

	this->setWidth( fromDWord( tmp ) );
	tmp += 4;
	this->setHeight( fromDWord( tmp ) );
	tmp += 4;

	this->planes_ = fromWord( tmp );
	tmp += 2;

	this->setBitsPerPixel( fromWord( tmp ) );
	tmp += 2;

	int compressMethod = fromDWord( tmp );
	tmp += 4;

	this->setImageSize( fromDWord( tmp ) );
	tmp += 4;

	this->setHResolution( fromDWord( tmp ) );
	tmp += 4;
	this->setVResolution( fromDWord( tmp ) );
	tmp += 4;

	this->nColorPalette_ = fromDWord( tmp );
	tmp += 4;

	// ignore important color
	tmp += 4;

	// jump offset
	in.seekg( this->offset_, std::ios::beg );
}

void Bitmap::writeHeader( std::ostream & out ) {
	out.write( "BM", 2 );
	char tmp[4];
	fromInt32( tmp, 4, this->getFileSize() );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, 0 );	// ignore reserve bits
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->offset_ );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->headerSize_ );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->getWidth() );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->getHeight() );
	out.write( tmp, 4 );
	fromInt32( tmp, 2, this->planes_ );
	out.write( tmp, 2 );
	fromInt32( tmp, 2, this->getBitsPerPixel() );
	out.write( tmp, 2 );
	fromInt32( tmp, 4, 0 );	// no compress method
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->getImageSize() );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->getHResolution() );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->getVResolution() );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->nColorPalette_ );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, 0 );	// no important color
	out.write( tmp, 4 );

	// write color palette
	for( int i = 0; i < 256; ++i ) {
		fromInt32( tmp, 4, i << 16 | i << 8 | i );
		out.write( tmp, 4 );
	}
}

}

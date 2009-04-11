#include "bmp.hpp"

#include <stdexcept>
#include <map>
#include <algorithm>
#include <cmath>
#include <cstring>

namespace {

typedef std::vector< int > vi;

inline int fromWord( char * w ) {
	int i;
	std::memcpy( &i, w, 2 );
	return i;
}

inline int fromDWord( char * dw ) {
	int i;
	std::memcpy( &i, dw, 4 );
	return i;
}

inline void fromInt32( char * b, std::size_t n, int i32 ) {
	std::memcpy( b, &i32, n );
}

struct eq {
	eq( int cdfMin, int bpp, int size ):
	min_( cdfMin ),
	sum_( 0 ),
	colors_( std::pow( 2, bpp ) - 1 ),
	down_( size - this->min_ ) {}

	void operator ()( std::pair< const int, int > & it ) {
		// get cdf
		this->sum_ += it.second;
		// equalize
		it.second = std::floor( ( this->sum_ - this->min_ ) / this->down_ * this->colors_ );
	}
private:
	int min_, sum_;
	double colors_, down_;
};

}

namespace pwc94u {

Image::Image():
fileSize_( 0 ) {
}
Image::~Image() {}

void Image::load( std::istream & in ) {
	this->parseHeader( in );

	this->pixmap_ = Pixmap( this->height_, ::vi( this->width_ ) );
	int bytes = this->bitsPerPixel_ / 8;
	char pixel[4] = { 0 };
	int paddingSize = this->width_ * bytes % 4;
	for( Pixmap::reverse_iterator it = this->pixmap_.rbegin(); it != this->pixmap_.rend(); ++it ) {
		for( int col = 0; col < this->width_; ++col ) {
			in.read( pixel, bytes );
			(*it)[col] = fromDWord( pixel );
		}
		in.read( pixel, paddingSize );
	}
}

void Image::save( std::ostream & out ) {
	this->writeHeader( out );

	int bytes = this->bitsPerPixel_ / 8;
	char pixel[4] = { 0 };
	int paddingSize = this->width_ * bytes % 4;
	for( Pixmap::reverse_iterator it = this->pixmap_.rbegin(); it != this->pixmap_.rend(); ++it ) {
		for( int col = 0; col < this->width_; ++col ) {
			fromInt32( pixel, 4, (*it)[col] );
			out.write( pixel, bytes );
		}
		out.write( "\0\0\0\0", paddingSize );
	}
}

void Image::equalize() {
	using std::map;
	using std::make_pair;
	map< int, int > table;

	// get color count
	for( int row = 0; row < this->width_; ++row ) {
		for( int col = 0; col < this->height_; ++col ) {
			map< int, int >::iterator it = table.find( this->pixmap_[row][col] );
			if( it == table.end() ) {
				table.insert( make_pair( this->pixmap_[row][col], 1 ) );
			} else {
				++it->second;
			}
		}
	}

	std::for_each( table.begin(), table.end(), eq( table.begin()->second, this->bitsPerPixel_, this->imageSize_ ) );

	for( int row = 0; row < this->width_; ++row ) {
		for( int col = 0; col < this->height_; ++col ) {
			this->pixmap_[row][col] = table.find( this->pixmap_[row][col] )->second;
		}
	}
}

void Image::parseHeader( std::istream & in ) {
	char header[54];
	in.read( header, 54 );
	char * tmp = header;

	if( tmp[0] != 'B' || tmp[1] != 'M' ) {
		throw std::runtime_error( "magic error" );
	}
	tmp += 2;

	this->fileSize_ = fromDWord( tmp );
	tmp += 4;

	// ignore reserved bits
	tmp += 4;

	this->offset_ = fromDWord( tmp );
	tmp += 4;

	this->headerSize_ = fromDWord( tmp );
	tmp += 4;

	this->width_ = fromDWord( tmp );
	tmp += 4;
	this->height_ = fromDWord( tmp );
	tmp += 4;

	this->planes_ = fromWord( tmp );
	tmp += 2;

	this->bitsPerPixel_ = fromWord( tmp );
	tmp += 2;

	int compressMethod = fromDWord( tmp );
	tmp += 4;

	this->imageSize_ = fromDWord( tmp );
	tmp += 4;

	this->hRes_ = fromDWord( tmp );
	tmp += 4;
	this->vRes_ = fromDWord( tmp );
	tmp += 4;

	this->nColorPalette_ = fromDWord( tmp );
	tmp += 4;

	// ignore important color
	tmp += 4;
}

void Image::writeHeader( std::ostream & out ) {
	out.write( "BM", 2 );
	char tmp[4];
	fromInt32( tmp, 4, this->fileSize_ );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, 0 );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->offset_ );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->headerSize_ );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->width_ );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->height_ );
	out.write( tmp, 4 );
	fromInt32( tmp, 2, this->planes_ );
	out.write( tmp, 2 );
	fromInt32( tmp, 2, this->bitsPerPixel_ );
	out.write( tmp, 2 );
	fromInt32( tmp, 4, 0 );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->imageSize_ );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->hRes_ );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->vRes_ );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, this->nColorPalette_ );
	out.write( tmp, 4 );
	fromInt32( tmp, 4, 0 );
	out.write( tmp, 4 );
}

}

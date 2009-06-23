#include "image.hpp"

#include <map>
#include <algorithm>
#include <cmath>
#include <cstring>

namespace {

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

const double PI = std::acos( -1 );

}

namespace pwc94u {

Image::Image():
fileSize_( 0 ),
width_( 0 ),
height_( 0 ),
bitsPerPixel_( 0 ),
imageSize_( 0 ),
hRes_( 0 ),
vRes_( 0 ),
pixmap_() {
}
Image::~Image() {}

void Image::load( std::istream & in ) {
	this->parseHeader( in );

	this->pixmap_ = Pixmap( this->height_, std::vector< int >( this->width_ ) );
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

	std::for_each( table.begin(), table.end(), eq( table.begin()->second, this->bitsPerPixel_, this->width_ * this->height_ ) );

	for( int row = 0; row < this->width_; ++row ) {
		for( int col = 0; col < this->height_; ++col ) {
			this->pixmap_[row][col] = table.find( this->pixmap_[row][col] )->second;
		}
	}
}

struct HSI {
	double h, s, i;
	HSI() : h(0), s(0), i(0) {}
	HSI( int rgb ) {
		int red = ( rgb >> 8 ) & 0xff;
		int green = ( rgb >> 16 ) & 0xff;
		int black = ( rgb >> 24 ) & 0xff;

		double theta = std::acos( 0.5 * ( 2*red - green - black ) / std::sqrt( ( red - green ) * ( red - green ) + ( red - black ) * ( green - black ) ) );
		h = ( black <= green ) ? ( theta ) : ( 2 * PI - theta );

		s = 1 - 3.0 * std::min( std::min( red, green ), black ) / ( red + green + black );

		i = ( red + green + black ) / 3.0;
	}

	int toRGB() const {
		int black = 0, red = 0, green = 0;
		if( 0 <= h && h < PI * 2 / 3 ) {
			black = i * ( 1 - s );
			red = i * ( 1 + ( s * std::cos( h ) ) / ( std::cos( PI / 3 - h ) ) );
			green = 3 * i - ( red + black );
		} else if( PI * 2 / 3 <= h && h < PI * 4 / 3 ) {
			double h_ = h - PI * 2 / 3;
			red = i * ( 1 - s );
			green = i * ( 1 + ( s * std::cos( h_ ) ) / ( std::cos( PI / 3 - h_ ) ) );
			black = 3 * i - ( red + black );
		} else if( PI * 4 / 3 <= h && h <= 2 * PI ) {
			double h_ = h - PI * 4 / 3;
			green = i * ( 1 - s );
			black = i * ( 1 + ( s * std::cos( h_ ) ) / ( std::cos( PI / 3 - h_ ) ) );
			red = 3 * i - ( red + black );
		}
		return ( black << 24 ) | ( green << 16 ) | ( red << 8 );
	}
};

struct eq2 {
	eq2( int cdfMin, int bpp, int size ):
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

void Image::equalize2() {
	using std::map;
	map< int, int > table;
	using std::vector;
	vector< vector< HSI > > backup( this->height_, vector< HSI >( this->width_ ) );
	using std::make_pair;

	for( int row = 0; row < this->width_; ++row ) {
		for( int col = 0; col < this->height_; ++col ) {
			backup[row][col] = HSI( this->pixmap_[row][col] );
			int i = static_cast< int >( backup[row][col].i );
			map< int, int >::iterator it = table.find( i );
			if( it == table.end() ) {
				table.insert( make_pair( i, 1 ) );
			} else {
				++it->second;
			}
		}
	}

	std::for_each( table.begin(), table.end(), eq2( table.begin()->second, 8, this->width_ * this->height_ ) );

	for( int row = 0; row < this->width_; ++row ) {
		for( int col = 0; col < this->height_; ++col ) {
			int i = static_cast< int >( backup[row][col].i );
			backup[row][col].i = table.find( i )->second;
			this->pixmap_[row][col] = backup[row][col].toRGB();
		}
	}
}

struct Filter {
	Filter( int dim, const Image::Pixmap & image, int width, int height ) : image_( image ), dim_( dim ), width_( width ), height_( height ) {}

	int operator()( int row, int col ) const {
		return std::floor( 0.5 * ( this->max_( row, col ) + this->min_( row, col ) ) );
	}
private:
	const Image::Pixmap & image_;
	const int dim_;
	const int width_;
	const int height_;

	int max_( int row, int col ) const {
		const int offset = this->dim_ / 2;
		const int BeginRow = ( row - offset < 0 ) ? 0 : ( row - offset );
		const int EndRow = ( row + offset >= this->height_ ) ? this->height_ : ( row + offset );
		const int BeginCol = ( col - offset < 0 ) ? 0 : ( col - offset );
		const int EndCol = ( col + offset >= this->width_ ) ? this->width_ : ( col + offset );

		int maxi = this->image_[row][col];
		for( row = BeginRow; row < EndRow; ++row ) {
			for( col = BeginCol; col < EndCol; ++col ) {
				if( maxi < this->image_[row][col] ) {
					maxi = this->image_[row][col];
				}
			}
		}

		return maxi;
	}
	int min_( int row, int col ) const {
		const int offset = this->dim_ / 2;
		const int BeginRow = ( row - offset < 0 ) ? 0 : ( row - offset );
		const int EndRow = ( row + offset >= this->height_ ) ? this->height_ : ( row + offset );
		const int BeginCol = ( col - offset < 0 ) ? 0 : ( col - offset );
		const int EndCol = ( col + offset >= this->width_ ) ? this->width_ : ( col + offset );

		int mini = this->image_[row][col];
		for( row = BeginRow; row < EndRow; ++row ) {
			for( col = BeginCol; col < EndCol; ++col ) {
				if( mini > this->image_[row][col] ) {
					mini = this->image_[row][col];
				}
			}
		}

		return mini;
	}
};

void Image::reduceNoise() {
	Pixmap backup( this->pixmap_ );
	Filter functor( 3, this->pixmap_, this->width_, this->height_ );

	for( int row = 0; row < this->width_; ++row ) {
		for( int col = 0; col < this->height_; ++col ) {
			backup[row][col] = functor( row, col );
		}
	}

	this->pixmap_ = backup;
}

struct LaplacianFilter {
	LaplacianFilter( const Image::Pixmap & image, int width, int height ) : image_( image ), width_( width ), height_( height ) {}

	int operator()( int row, int col ) const {
		return 4 * this->get_( row, col ) - ( this->get_( row - 1, col ) + this->get_( row, col - 1 ) + this->get_( row, col + 1 ) + this->get_( row + 1, col ) );
// 		return 8 * this->get_( row, col ) - ( this->get_( row - 1, col - 1 ) + this->get_( row - 1, col ) + this->get_( row - 1, col + 1 ) + this->get_( row, col - 1 ) + this->get_( row, col + 1 ) + this->get_( row + 1, col - 1 ) + this->get_( row + 1, col ) + this->get_( row + 1, col + 1 ) );
	}
private:
	const Image::Pixmap & image_;
	int width_, height_;

	int get_( int row, int col ) const {
		if( row < 0 || row >= this->height_ || col < 0 || col >= this->width_ ) {
			return 0;
		} else {
			return this->image_[row][col];
		}
	}
};

void Image::laplacian() {
	Pixmap backup( this->pixmap_ );
	LaplacianFilter functor( this->pixmap_, this->width_, this->height_ );

	for( int row = 0; row < this->width_; ++row ) {
		for( int col = 0; col < this->height_; ++col ) {
			backup[row][col] = functor( row, col );
		}
	}

	this->pixmap_ = backup;
}

void filter44( Image::Pixmap & pixmap, int y, int x ) {
}

int fromDWord( char * dw ) {
	int i;
	std::memcpy( &i, dw, 4 );
	return i;
}

void fromInt32( char * b, std::size_t n, int i32 ) {
	std::memcpy( b, &i32, n );
}

}

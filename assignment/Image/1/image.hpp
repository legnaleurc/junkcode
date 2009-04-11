#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <iostream>
#include <vector>

namespace pwc94u {

class Image {
public:
	typedef std::vector< std::vector< int > > Pixmap;

	Image();
	virtual ~Image();

	void load( std::istream & in );
	void save( std::ostream & out );

	void equalize();

private:
	virtual void parseHeader( std::istream & in ) = 0;
	virtual void writeHeader( std::ostream & out ) = 0;

	int fileSize_;
	int offset_;
	int headerSize_;
	int width_;
	int height_;
	short int planes_;
	short int bitsPerPixel_;
	int imageSize_;
	int hRes_;
	int vRes_;
	int nColorPalette_;
	Pixmap pixmap_;
};

}

#endif

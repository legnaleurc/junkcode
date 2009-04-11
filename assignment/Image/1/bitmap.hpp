#ifndef BITMAP_HPP
#define BITMAP_HPP

#include "image.hpp"

#include <iostream>
#include <vector>

namespace pwc94u {

class Bitmap : public Image {
public:
	Bitmap();
	virtual ~Bitmap();

private:
	virtual void parseHeader( std::istream & in );
	virtual void writeHeader( std::ostream & out );

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

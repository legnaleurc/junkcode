#ifndef BITMAP_HPP
#define BITMAP_HPP

#include "image.hpp"

namespace pwc94u {

class Bitmap : public Image {
public:
	Bitmap();
	virtual ~Bitmap();

private:
	virtual void parseHeader( std::istream & in );
	virtual void writeHeader( std::ostream & out );

	int offset_;
	int headerSize_;
	short int planes_;
	int nColorPalette_;
};

}

#endif

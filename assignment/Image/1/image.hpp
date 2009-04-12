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

	int getFileSize() const {
		return this->fileSize_;
	}
	void setFileSize( int fileSize ) {
		this->fileSize_ = fileSize;
	}
	int getWidth() const {
		return this->width_;
	}
	void setWidth( int width ) {
		this->width_ = width;
	}
	int getHeight() const {
		return this->height_;
	}
	void setHeight( int height ) {
		this->height_ = height;
	}
	short int getBitsPerPixel() const {
		return this->bitsPerPixel_;
	}
	void setBitsPerPixel( short int bpp ) {
		this->bitsPerPixel_ = bpp;
	}
	int getImageSize() const {
		return this->imageSize_;
	}
	void setImageSize( int imageSize ) {
		this->imageSize_ = imageSize;
	}
	int getHResolution() const {
		return this->hRes_;
	}
	void setHResolution( int hRes ) {
		this->hRes_ = hRes;
	}
	int getVResolution() const {
		return this->vRes_;
	}
	void setVResolution( int vRes ) {
		this->vRes_ = vRes;
	}

protected:
	Pixmap & pixmap() {
		return this->pixmap_;
	}

private:
	virtual void parseHeader( std::istream & in ) = 0;
	virtual void writeHeader( std::ostream & out ) = 0;

	int fileSize_;
	int width_;
	int height_;
	short int bitsPerPixel_;
	int imageSize_;
	int hRes_;
	int vRes_;
	Pixmap pixmap_;
};

int fromDWord( char * dw );

void fromInt32( char * b, std::size_t n, int i32 );

}

#endif

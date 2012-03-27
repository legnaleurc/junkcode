#ifndef NSIZE_HPP
#define NSIZE_HPP

#include <memory>

class NSize {
public:
	NSize();
	NSize( int rows, int cols );
	NSize( const NSize & that );

	NSize & operator =( const NSize & that );

	int & cols();
	int cols() const;
	int & rows();
	int rows() const;

private:
	class Private;
	std::shared_ptr< Private > p_;
};

#endif

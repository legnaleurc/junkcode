#ifndef NWIDGET_HPP
#define NWIDGET_HPP

#include <memory>

class NPoint;

class NWidget {
public:
	explicit NWidget( NWidget * parent = 0 );
	virtual ~NWidget();

	NWidget * parent() const;
	const NPoint & pos() const;
	void update();

private:
	NWidget( const NWidget & );
	NWidget & operator =( const NWidget & );
	class Private;
	std::shared_ptr< Private > p_;
};

#endif

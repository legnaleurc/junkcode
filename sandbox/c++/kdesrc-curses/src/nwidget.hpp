#ifndef NWIDGET_HPP
#define NWIDGET_HPP

#include <boost/signal.hpp>

#include <memory>

class NApplication;
class NPoint;

class NWidget {
public:
	explicit NWidget( NWidget * parent = 0 );
	virtual ~NWidget();

	NWidget * parent() const;
	const NPoint & pos() const;
	void update();

	boost::signal< void ( int ) > & keyPressed();

protected:
	virtual void inputEvent( int keyCode );

private:
	NWidget( const NWidget & );
	NWidget & operator =( const NWidget & );
	friend class NApplication;
	class Private;
	std::shared_ptr< Private > p_;
};

#endif

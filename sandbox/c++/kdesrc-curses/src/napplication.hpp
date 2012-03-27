#ifndef NAPPLICATION_HPP
#define NAPPLICATION_HPP

#include <memory>

class NWidget;

class NApplication {
public:
	static NApplication & instance();

	int exec();
	void quit();

private:
	NApplication();
	NApplication( const NApplication & );
	NApplication & operator =( const NApplication & );
	~NApplication();
	friend class NWidget;
	class Private;
	friend class Private;
	std::shared_ptr< Private > p_;
};

#endif

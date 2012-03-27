#ifndef NAPPLICATION_P_HPP
#define NAPPLICATION_P_HPP

#include "napplication.hpp"
#include "nwidget.hpp"

#include <ncurses.h>

#include <vector>

class NApplication::Private {
public:
	static std::shared_ptr< NApplication > self;
	static void destroy( NApplication * );
	static void onWindowChanged( int sig );

	Private();
	~Private();

	void addWidget( NWidget * widget );
	void removeWidget( NWidget * widget );
	void resize( int rows, int cols );

	NWidget * focusWidget;
	std::shared_ptr< WINDOW > screen;
	std::vector< NWidget * > widgets;
};

#endif

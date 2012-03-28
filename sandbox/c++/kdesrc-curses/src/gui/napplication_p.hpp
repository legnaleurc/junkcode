#ifndef NAPPLICATION_P_HPP
#define NAPPLICATION_P_HPP

#include "gui/napplication.hpp"
#include "core/nsize.hpp"
#include "gui/nwidget.hpp"

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
	NSize size;
	NWidget * topLevelWidget;
	std::vector< NWidget * > widgets;
};

#endif

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

	Private();
	~Private();

	WINDOW * screen;
	std::vector< NWidget * > widgets;
};

#endif

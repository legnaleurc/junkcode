#ifndef NWIDGET_P_HPP
#define NWIDGET_P_HPP

#include "nwidget.hpp"
#include "npoint.hpp"

#include <ncurses.h>

#include <vector>

class NWidget::Private {
public:
	Private( NWidget * parent );
	~Private();

	std::vector< NWidget * > children;
	NWidget * parent;
	NPoint pos;
	WINDOW * window;
};

#endif

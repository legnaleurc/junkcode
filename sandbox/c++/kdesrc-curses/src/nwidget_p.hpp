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
	boost::signal< void ( int ) > keyPressed;
	NWidget * parent;
	NPoint pos;
	std::shared_ptr< WINDOW > window;
};

#endif

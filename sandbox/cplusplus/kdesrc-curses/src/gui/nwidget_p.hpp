#ifndef NWIDGET_P_HPP
#define NWIDGET_P_HPP

#include "gui/nwidget.hpp"
#include "core/npoint.hpp"
#include "core/nsize.hpp"

#include <vector>

#include <ncurses.h>


class NWidget::Private {
public:
    Private (NWidget * parent);
    ~Private ();

    std::vector<NWidget *> children;
    boost::signals2::signal<void (int)> keyPressed;
    NWidget * parent;
    NPoint pos;
    NSize size;
    std::shared_ptr<WINDOW> window;
};

#endif

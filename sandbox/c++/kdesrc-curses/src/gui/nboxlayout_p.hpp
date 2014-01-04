#ifndef NBOXLAYOUT_P_HPP
#define NBOXLAYOUT_P_HPP

#include "gui/nboxlayout.hpp"

class NBoxLayout::Private {
public:
    Private( NWidget * parent );

    NWidget * parentWidget;
};

#endif

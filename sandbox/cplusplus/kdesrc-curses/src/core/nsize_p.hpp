#ifndef NSIZE_P_HPP
#define NSIZE_P_HPP

#include "core/nsize.hpp"

class NSize::Private {
public:
    Private( int rows, int cols );

    int cols;
    int rows;
};

#endif

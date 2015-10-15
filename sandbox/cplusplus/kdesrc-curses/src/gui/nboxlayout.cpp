#include "gui/nboxlayout_p.hpp"

NBoxLayout::Private::Private( NWidget * parent ):
parentWidget( parent ) {
}

NBoxLayout::NBoxLayout( NWidget * parent ):
p_( new Private( parent ) ) {
}

NBoxLayout::~NBoxLayout() {
}

void NBoxLayout::addLayout( NBoxLayout * layout ) {
}

void NBoxLayout::addWidget( NWidget * widget ) {
}

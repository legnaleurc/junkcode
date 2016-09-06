#ifndef RUST_INTERNAL_H
#define RUST_INTERNAL_H

#include <QtGui/QWidget>

#include "rust_qobject.h"

QWidget * rust_to_qwidget(RustQWidget object);

#endif

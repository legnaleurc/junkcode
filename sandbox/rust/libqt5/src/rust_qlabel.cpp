#include "rust_qlabel.h"

#include <QtGui/QLabel>

#include "rust_internal.h"


struct qlabel {
    QLabel * p;
};


qlabel * qlabel_new(qwidget * parent) {
    qlabel * self = new qlabel;
    self->p = new QLabel(unwrap(parent));
    return self;
}


void qlabel_delete(qlabel * * self) {
    if (!self) {
        return;
    }
    if (*self) {
        (*self)->p->deleteLater();
        delete *self;
    }
    *self = nullptr;
}


void qlabel_resize(qlabel * self, int width, int height) {
    self->p->resize(width, height);
}


void qlabel_show(qlabel * self) {
    return self->p->show();
}

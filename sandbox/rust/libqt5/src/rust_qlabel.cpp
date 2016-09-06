#include <QtGui/QLabel>

#include "rust_qlabel.h"
#include "rust_internal.h"


struct RustQLabel_ {
    QLabel * p;
};


RustQLabel qlabel_new(RustQWidget parent) {
    RustQLabel self = new RustQLabel_;
    self->p = new QLabel(rust_to_qwidget(parent));
    return self;
}


void qlabel_delete(RustQLabel * self) {
    if (self) {
        if (*self) {
            (*self)->p->deleteLater();
            delete *self;
        }
        *self = nullptr;
    }
}


void qlabel_resize(RustQLabel self, int width, int height) {
    self->p->resize(width, height);
}


void qlabel_show(RustQLabel self) {
    return self->p->show();
}

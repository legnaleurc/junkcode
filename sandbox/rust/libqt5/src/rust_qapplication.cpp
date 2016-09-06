#include <QtGui/QApplication>

#include "rust_qapplication.h"


struct RustQApplication_ {
    QApplication * p;
};


RustQApplication qapplication_new(int argc, char ** argv) {
    RustQApplication self = new RustQApplication_;
    self->p = new QApplication(argc, argv);
    return self;
}


void qapplication_delete(RustQApplication * self) {
    if (self) {
        if (*self) {
            (*self)->p->deleteLater();
            delete *self;
        }
        *self = nullptr;
    }
}


int qapplication_exec(RustQApplication self) {
    return self->p->exec();
}

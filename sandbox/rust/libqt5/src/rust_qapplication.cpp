#include <QtGui/QApplication>

#include "rust_qapplication.h"


struct qapplication {
    QApplication * p;
};


qapplication * qapplication_new(int argc, char ** argv) {
    qapplication * self = new qapplication;
    self->p = new QApplication(argc, argv);
    return self;
}


void qapplication_delete(qapplication * * self) {
    if (!self) {
        return;
    }
    if (*self) {
        (*self)->p->deleteLater();
        delete *self;
    }
    *self = nullptr;
}


int qapplication_exec(qapplication * self) {
    return self->p->exec();
}

#include "rust_internal.h"


struct RustQWidget_ {
    QWidget * p;
};


QWidget * rust_to_qwidget(RustQWidget object) {
    if (object && object->p) {
        return object->p;
    }
    return nullptr;
}

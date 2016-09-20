#ifndef RUST_INTERNAL_H
#define RUST_INTERNAL_H

// #include <QtGui/QWidget>

// #include "rust_qobject.h"

// QWidget * rust_to_qwidget(RustQWidget object);

// template<typename T, typename U>
// T * unwrap(U * wrapped_object) {
//     if (wrapped_object && wrapped_object->p) {
//         return wrapped_object->p;
//     }
//     return nullptr;
// }

template<typename T>
auto unwrap(T * wrapped_object) -> decltype(wrapped_object->p) {
    if (wrapped_object && wrapped_object->p) {
        return wrapped_object->p;
    }
    return nullptr;
}

#endif

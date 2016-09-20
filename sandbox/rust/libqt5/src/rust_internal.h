#ifndef RUST_INTERNAL_H
#define RUST_INTERNAL_H


class QWidget;
class QLabel;


struct qwidget {
    QWidget * p;
};


struct qlabel {
    QLabel * p;
};


template<typename T>
auto unwrap(T * wrapped_object) -> decltype(wrapped_object->p) {
    if (wrapped_object && wrapped_object->p) {
        return wrapped_object->p;
    }
    return nullptr;
}

#endif

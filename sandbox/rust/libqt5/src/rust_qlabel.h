#ifndef RUST_QLABEL_H
#define RUST_QLABEL_H

#include "rust_qtgui.h"

#ifdef __cplusplus
extern "C" {
#endif

qlabel * qlabel_new(qwidget * parent);
void qlabel_delete(qlabel * * self);
void qlabel_resize(qlabel * self, int width, int height);
void qlabel_show(qlabel * self);

#ifdef __cplusplus
}
#endif

#endif

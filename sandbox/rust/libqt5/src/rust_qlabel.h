#ifndef RUST_QLABEL_H
#define RUST_QLABEL_H

#include "rust_qobject.h"


typedef struct RustQLabel_ * RustQLabel;

#ifdef __cplusplus
extern "C" {
#endif

RustQLabel qlabel_new(RustQWidget parent);
void qlabel_delete(RustQLabel * self);
void qlabel_resize(RustQLabel self, int width, int height);
void qlabel_show(RustQLabel self);

#ifdef __cplusplus
}
#endif

#endif

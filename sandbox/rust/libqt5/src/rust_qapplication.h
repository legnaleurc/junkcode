#ifndef RUST_QAPPLICATION_H
#define RUST_QAPPLICATION_H

#include "rust_qtgui.h"

#ifdef __cplusplus
extern "C" {
#endif

qapplication * qapplication_new(int argc, char ** argv);
void qapplication_delete(qapplication * * self);

int qapplication_exec(qapplication * self);

#ifdef __cplusplus
}
#endif

#endif

#ifndef RUST_QAPPLICATION_H
#define RUST_QAPPLICATION_H

typedef struct RustQApplication_ * RustQApplication;

#ifdef __cplusplus
extern "C" {
#endif

RustQApplication qapplication_new(int argc, char ** argv);
void qapplication_delete(RustQApplication * self);
int qapplication_exec(RustQApplication self);

#ifdef __cplusplus
}
#endif

#endif

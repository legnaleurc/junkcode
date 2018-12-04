import pathlib as pl

import cffi


ffi_builder = cffi.FFI()
ffi_builder.cdef('''
    typedef struct ScreenshotContext ScreenshotContext;

    typedef int(*ReadFunction)(void *, uint8_t *, int);
    typedef int64_t(*SeekFunction)(void *, int64_t, int);

    ScreenshotContext * screenshot_new(void * opaque, ReadFunction reader,
                                       SeekFunction seeker);
    void screenshot_delete(ScreenshotContext * handle);
    int screenshot_has_next(ScreenshotContext * handle);
    int screenshot_slice(ScreenshotContext * handle, const char * path);

    extern "Python" {
        int drive_read(void *, uint8_t *, int);
        int64_t drive_seek(void *, int64_t, int);
    }
''')


if __name__ == '__main__':
    ROOT_PATH = pl.Path(__file__).parent.parent
    BUILD_PATH = ROOT_PATH / 'build'
    ffi_builder.set_source('build._screenshot', '''
        #include "../ext/screenshot.h"
    ''', libraries=['screenshot'], extra_link_args=[
        f'-L{BUILD_PATH}',
        f'-Wl,-rpath,{BUILD_PATH}',
    ])
    ffi_builder.compile()

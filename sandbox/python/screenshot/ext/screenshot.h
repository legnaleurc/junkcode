#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include "types.h"


typedef struct ScreenshotContext ScreenshotContext;


ScreenshotContext * screenshot_new(void * opaque, ReadFunction reader,
                                   SeekFunction seeker);
void screenshot_delete(ScreenshotContext * handle);
int screenshot_has_next(ScreenshotContext * handle);
int screenshot_slice(ScreenshotContext * handle, const char * path);


#endif

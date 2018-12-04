#include "screenshot.h"

#include <stdlib.h>

#include "input.h"
#include "output.h"
#include "frame.h"


static const int START_SECOND = 300;
static const int INTERVAL_SECOND = 300;


struct ScreenshotContext {
    InputContext * input_context;
    int64_t next_second;
    int counter;
};


ScreenshotContext * screenshot_new(void * opaque, ReadFunction reader,
                                   SeekFunction seeker) {
    ScreenshotContext * handle = malloc(sizeof(ScreenshotContext));
    if (!handle) {
        return NULL;
    }

    handle->input_context = input_context_new(opaque, reader, seeker);
    if (!handle->input_context) {
        goto fail;
    }
    handle->next_second = START_SECOND;
    handle->counter = 0;
    return handle;

fail:
    free(handle);
    return NULL;
}


void screenshot_delete(ScreenshotContext * handle) {
    if (!handle) {
        return;
    }
    input_context_delete(&handle->input_context);
    free(handle);
}


int screenshot_has_next(ScreenshotContext * handle) {
    if (!handle) {
        return 0;
    }
    return handle->next_second < handle->input_context->duration;
}


int screenshot_slice(ScreenshotContext * handle, const char * path) {
    if (!handle) {
        return 1;
    }

    int ok = EXIT_SUCCESS;

    ok = input_context_seek(handle->input_context, handle->next_second);
    if (ok != 0) {
        ok = EXIT_FAILURE;
        goto next;
    }

    FrameContext * frame_context = frame_context_new(handle->input_context);
    if (!frame_context) {
        ok = EXIT_FAILURE;
        goto next;
    }

    char filename[4096] = "";
    snprintf(filename, sizeof(filename), path, handle->counter);
    OutputContext * output_context =
        output_context_new(filename,
                           frame_context->input_frame->width,
                           frame_context->input_frame->height);
    if (!output_context) {
        ok = EXIT_FAILURE;
        goto close_frame_context;
    }
    frame_context->output_context = output_context;

    // convert snapshot to output format
    ok = frame_context_convert(frame_context);
    if (ok != 0) {
        ok = EXIT_FAILURE;
        goto close_frame_context;
    }

    // write the snapshot
    ok = output_context_write_frame(output_context,
                                    frame_context->output_frame);
    if (ok != 0) {
        ok = EXIT_FAILURE;
        goto close_output_context;
    }

    ok = EXIT_SUCCESS;

close_output_context:
    output_context_delete(&output_context);
close_frame_context:
    frame_context_delete(&frame_context);
next:
    handle->next_second += INTERVAL_SECOND;
    handle->counter += 1;
    return ok;
}

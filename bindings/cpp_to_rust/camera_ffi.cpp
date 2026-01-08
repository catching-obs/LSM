#include "camera_ffi.h"
#include "../../core/video/CameraSource.h"
#include <memory>
#include <string>

struct CameraContext {
    std::unique_ptr<CameraSource> source;
    // Cache last frame dimensions to avoid checking every time if needed
    int width = 0;
    int height = 0;
};

CameraContext* camera_create(int device_index) {
    try {
        auto ctx = new CameraContext();
        // Convert integer index to string ID expected by CameraSource
        ctx->source = std::make_unique<CameraSource>(std::to_string(device_index));
        return ctx;
    } catch (...) {
        return nullptr;
    }
}

void camera_destroy(CameraContext* ctx) {
    if (ctx) {
        delete ctx;
    }
}

bool camera_start(CameraContext* ctx) {
    if (!ctx || !ctx->source) return false;
    return ctx->source->start();
}

void camera_stop(CameraContext* ctx) {
    if (ctx && ctx->source) {
        ctx->source->stop();
    }
}

bool camera_get_frame(CameraContext* ctx, uint8_t* buffer, int* width, int* height, uint64_t* timestamp) {
    if (!ctx || !ctx->source) return false;

    VideoFrame frame;
    if (ctx->source->getFrame(frame)) {
        if (buffer) {
            // Safety check: caller must ensure buffer is large enough.
            // In a production FFI, we should pass buffer size to verify.
            std::memcpy(buffer, frame.data.data(), frame.data.size());
        }
        
        if (width) *width = frame.width;
        if (height) *height = frame.height;
        if (timestamp) *timestamp = frame.timestamp;
        
        return true;
    }
    return false;
}

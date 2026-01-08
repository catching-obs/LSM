#ifndef BINDINGS_VIDEO_FFI_H
#define BINDINGS_VIDEO_FFI_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque pointer to Rust EncoderPipeline
typedef struct EncoderPipeline EncoderPipeline;

// Encoder configuration
typedef struct {
    int width;
    int height;
    int fps;
    int bitrate;
} CEncoderConfig;

// Create a new encoder pipeline
EncoderPipeline* encoder_create(CEncoderConfig config);

// Destroy an encoder pipeline
void encoder_destroy(EncoderPipeline* encoder);

// Submit a frame for encoding
bool encoder_submit_frame(
    EncoderPipeline* encoder,
    const void* frame_data,
    size_t frame_size,
    int64_t timestamp
);

// Start the encoding pipeline
bool encoder_start(EncoderPipeline* encoder);

// Stop the encoding pipeline
bool encoder_stop(EncoderPipeline* encoder);

// Get number of encoded frames
uint64_t encoder_get_encoded_frames(const EncoderPipeline* encoder);

#ifdef __cplusplus
}
#endif

#endif // BINDINGS_VIDEO_FFI_H

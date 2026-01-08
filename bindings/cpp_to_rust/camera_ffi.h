#ifndef CAMERA_FFI_H
#define CAMERA_FFI_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque handle for the camera source
typedef struct CameraContext CameraContext;

/**
 * Create a new camera source instance.
 * @param device_index The index of the camera (e.g., 0 for default).
 * @return Pointer to CameraContext, or NULL on failure.
 */
CameraContext* camera_create(int device_index);

/**
 * Destroy the camera source and free resources.
 * @param ctx The camera context.
 */
void camera_destroy(CameraContext* ctx);

/**
 * Start video capture.
 * @param ctx The camera context.
 * @return true if started successfully, false otherwise.
 */
bool camera_start(CameraContext* ctx);

/**
 * Stop video capture.
 * @param ctx The camera context.
 */
void camera_stop(CameraContext* ctx);

/**
 * Retrieve the latest video frame.
 * @param ctx The camera context.
 * @param buffer Pointer to the destination buffer (must be large enough: w * h * 4 for RGBA).
 * @param width Output pointer for frame width.
 * @param height Output pointer for frame height.
 * @param timestamp Output pointer for timestamp (microseconds).
 * @return true if a new frame was copied, false if no new frame is available.
 */
bool camera_get_frame(CameraContext* ctx, uint8_t* buffer, int* width, int* height, uint64_t* timestamp);

#ifdef __cplusplus
}
#endif

#endif // CAMERA_FFI_H

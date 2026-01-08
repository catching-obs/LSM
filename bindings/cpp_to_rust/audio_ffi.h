#ifndef AUDIO_FFI_H
#define AUDIO_FFI_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AudioContext AudioContext;

/**
 * Create a new audio source instance.
 * @param device_index Index of the audio device (currently unused/mocked).
 * @return Pointer to AudioContext or NULL.
 */
AudioContext* audio_create(int device_index);

/**
 * Destroy audio source.
 */
void audio_destroy(AudioContext* ctx);

/**
 * Start audio capture.
 */
bool audio_start(AudioContext* ctx);

/**
 * Stop audio capture.
 */
void audio_stop(AudioContext* ctx);

/**
 * Retrieve the latest audio frame.
 * @param ctx Audio context.
 * @param buffer Pointer to float buffer.
 * @param buffer_len Size of the buffer in floats (must be enough to hold frame).
 * @param channels Output: number of channels.
 * @param sample_rate Output: sample rate.
 * @param timestamp Output: timestamp.
 * @return Number of samples actually copied (total samples, not frames). 0 if no new frame.
 */
int audio_get_frame(AudioContext* ctx, float* buffer, int buffer_len, int* channels, int* sample_rate, uint64_t* timestamp);

// --- Audio Mixer API (Restored) ---
typedef struct AudioMixer AudioMixer;

AudioMixer* audio_mixer_create(int channels);
void audio_mixer_destroy(AudioMixer* mixer);
bool audio_mixer_mix(AudioMixer* mixer, const float** inputs, float* output, size_t samples);
void audio_mixer_set_channel_volume(AudioMixer* mixer, int channel, float volume);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_FFI_H

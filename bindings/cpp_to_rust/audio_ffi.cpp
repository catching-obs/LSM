#include "audio_ffi.h"
#include "../../core/audio/MicrophoneSource.h"
#include <memory>
#include <string>
#include <algorithm>

struct AudioContext {
    std::unique_ptr<MicrophoneSource> source;
};

AudioContext* audio_create(int device_index) {
    try {
        auto ctx = new AudioContext();
        ctx->source = std::make_unique<MicrophoneSource>(std::to_string(device_index));
        return ctx;
    } catch (...) {
        return nullptr;
    }
}

void audio_destroy(AudioContext* ctx) {
    if (ctx) delete ctx;
}

bool audio_start(AudioContext* ctx) {
    if (!ctx || !ctx->source) return false;
    return ctx->source->start();
}

void audio_stop(AudioContext* ctx) {
    if (ctx && ctx->source) ctx->source->stop();
}

int audio_get_frame(AudioContext* ctx, float* buffer, int buffer_len, int* channels, int* sample_rate, uint64_t* timestamp) {
    if (!ctx || !ctx->source) return 0;

    AudioFrame frame;
    if (ctx->source->getFrame(frame)) {
        if (buffer) {
            int copy_count = std::min((int)frame.data.size(), buffer_len);
            std::memcpy(buffer, frame.data.data(), copy_count * sizeof(float));
            
            if (channels) *channels = frame.channels;
            if (sample_rate) *sample_rate = frame.sampleRate;
            if (timestamp) *timestamp = frame.timestamp;
            
            return copy_count;
        }
    }
    return 0;
}

// --- Audio Mixer Implementation (Dummy/Mock) ---
struct AudioMixer {
    int channels;
    std::vector<float> volumes;
};

AudioMixer* audio_mixer_create(int channels) {
    auto m = new AudioMixer();
    m->channels = channels;
    m->volumes.resize(channels, 1.0f);
    return m;
}

void audio_mixer_destroy(AudioMixer* mixer) {
    if (mixer) delete mixer;
}

bool audio_mixer_mix(AudioMixer* mixer, const float** inputs, float* output, size_t samples) {
    if (!mixer || !inputs || !output) return false;
    
    // Simple sum with volume
    for (size_t i = 0; i < samples; ++i) {
        float sum = 0.0f;
        for (int c = 0; c < mixer->channels; ++c) {
             if (inputs[c]) {
                 sum += inputs[c][i] * mixer->volumes[c];
             }
        }
        output[i] = sum;
    }
    return true;
}

void audio_mixer_set_channel_volume(AudioMixer* mixer, int channel, float volume) {
    if (mixer && channel >= 0 && channel < mixer->channels) {
        mixer->volumes[channel] = volume;
    }
}

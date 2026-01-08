#ifndef AUDIO_FRAME_H
#define AUDIO_FRAME_H

#include <vector>
#include <cstdint>

struct AudioFrame {
    // Interleaved PCM data (e.g. L, R, L, R...)
    // Usually 32-bit float or 16-bit integer. We'll use float for modern pipeline.
    std::vector<float> data; 
    
    int channels;
    int sampleRate;
    int samplesPerChannel; // Number of samples per channel in this frame
    uint64_t timestamp;    // Microseconds

    AudioFrame(int ch = 2, int rate = 44100, int samples = 1024) 
        : channels(ch), sampleRate(rate), samplesPerChannel(samples), timestamp(0) {
        data.resize(channels * samplesPerChannel);
    }
};

#endif // AUDIO_FRAME_H

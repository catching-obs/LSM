#include "SpeakerSink.h"
#include <iostream>
#include <algorithm>

SpeakerSink::SpeakerSink() : deviceId_(0), running_(false) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Audio Init Failed: " << SDL_GetError() << std::endl;
    }
}

SpeakerSink::~SpeakerSink() {
    stop();
    // SDL_QuitSubSystem(SDL_INIT_AUDIO); // Careful if other modules use SDL
}

bool SpeakerSink::start() {
    if (running_) return true;

    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_F32; // Float 32-bit
    want.channels = 2;
    want.samples = 1024;
    want.callback = AudioCallback;
    want.userdata = this;

    deviceId_ = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (deviceId_ == 0) {
        std::cerr << "Failed to open playback device: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "Speaker opened: " << have.freq << "Hz " << (int)have.channels << "ch" << std::endl;

    running_ = true;
    SDL_PauseAudioDevice(deviceId_, 0); // Start playing
    return true;
}

void SpeakerSink::stop() {
    if (!running_) return;

    running_ = false;
    if (deviceId_ != 0) {
        SDL_CloseAudioDevice(deviceId_);
        deviceId_ = 0;
    }
}

void SpeakerSink::pushFrame(const AudioFrame& frame) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    // Limit queue size to avoid latency buildup (e.g., keep max 0.5 sec)
    if (audioQueue_.size() > 44100 * 2 * 0.5) { 
        // Drop oldest if too full (simple overflow handling)
        // In real app, we might resample or just clear
        audioQueue_.clear(); 
    }
    
    for (float sample : frame.data) {
        audioQueue_.push_back(sample);
    }
}

void SpeakerSink::AudioCallback(void* userdata, Uint8* stream, int len) {
    auto* sink = static_cast<SpeakerSink*>(userdata);
    sink->processAudio(stream, len);
}

void SpeakerSink::processAudio(Uint8* stream, int len) {
    int sampleCount = len / sizeof(float);
    float* out = (float*)stream;
    
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    for (int i = 0; i < sampleCount; ++i) {
        if (audioQueue_.empty()) {
            out[i] = 0.0f; // Silence
        } else {
            out[i] = audioQueue_.front();
            audioQueue_.pop_front();
        }
    }
}

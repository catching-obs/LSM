#include "MicrophoneSource.h"
#include <iostream>
#include <chrono>

MicrophoneSource::MicrophoneSource(const std::string& deviceId) 
    : deviceId_(deviceId), running_(false), captureDeviceId_(0) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Audio Init Failed: " << SDL_GetError() << std::endl;
    }
}

MicrophoneSource::~MicrophoneSource() {
    stop();
}

bool MicrophoneSource::start() {
    if (running_) return true;
    
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_F32;
    want.channels = 2;
    want.samples = 1024;
    want.callback = AudioCallback;
    want.userdata = this;

    // iscapture = 1
    // If deviceId_ is "default", pass NULL to let SDL pick default.
    // Otherwise pass the name.
    const char* devName = nullptr;
    if (deviceId_ != "default" && !deviceId_.empty()) {
        devName = deviceId_.c_str();
    }

    captureDeviceId_ = SDL_OpenAudioDevice(devName, 1, &want, &have, 0);
    if (captureDeviceId_ == 0) {
        std::cerr << "Failed to open capture device '" << (devName ? devName : "default") << "': " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "Microphone opened: " << have.freq << "Hz " << (int)have.channels << "ch" << std::endl;

    running_ = true;
    SDL_PauseAudioDevice(captureDeviceId_, 0); // Start recording
    return true;
}

void MicrophoneSource::stop() {
    if (!running_) return;

    running_ = false;
    if (captureDeviceId_ != 0) {
        SDL_CloseAudioDevice(captureDeviceId_);
        captureDeviceId_ = 0;
    }
    std::cout << "MicrophoneSource stopped." << std::endl;
}

bool MicrophoneSource::getFrame(AudioFrame& frame) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    int samplesNeeded = frame.data.size();
    if (captureQueue_.size() < samplesNeeded) {
        return false;
    }
    
    for (int i = 0; i < samplesNeeded; ++i) {
        frame.data[i] = captureQueue_.front();
        captureQueue_.pop_front();
    }
    
    frame.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        
    return true;
}

std::string MicrophoneSource::getName() const {
    return "Mic-" + deviceId_;
}

void MicrophoneSource::AudioCallback(void* userdata, Uint8* stream, int len) {
    auto* source = static_cast<MicrophoneSource*>(userdata);
    source->processCapturedAudio(stream, len);
}

void MicrophoneSource::processCapturedAudio(Uint8* stream, int len) {
    if (!running_) return;

    int sampleCount = len / sizeof(float);
    float* in = (float*)stream;
    
    std::lock_guard<std::mutex> lock(queueMutex_);
    for (int i = 0; i < sampleCount; ++i) {
        captureQueue_.push_back(in[i]);
    }
    
    // Prevent unlimited growth if not consumed
    size_t maxSize = 44100 * 2 * 2; // 2 seconds buffer
    if (captureQueue_.size() > maxSize) {
        // Drop oldest
        size_t toDrop = captureQueue_.size() - maxSize;
        auto it = captureQueue_.begin();
        std::advance(it, toDrop);
        captureQueue_.erase(captureQueue_.begin(), it);
    }
}

#ifndef MICROPHONE_SOURCE_H
#define MICROPHONE_SOURCE_H

#include "AudioSource.h"
#include <SDL.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <list>
#include <vector>

class MicrophoneSource : public AudioSource {
public:
    MicrophoneSource(const std::string& deviceId);
    ~MicrophoneSource() override;

    bool start() override;
    void stop() override;
    bool getFrame(AudioFrame& frame) override;
    std::string getName() const override;

private:
    static void AudioCallback(void* userdata, Uint8* stream, int len);
    void processCapturedAudio(Uint8* stream, int len);

    std::string deviceId_;
    std::atomic<bool> running_;
    
    SDL_AudioDeviceID captureDeviceId_;
    
    std::mutex queueMutex_;
    std::list<float> captureQueue_;
};

class AudioDeviceManager {
public:
    static std::vector<std::string> getInputDevices() {
        std::vector<std::string> devices;
        if (SDL_Init(SDL_INIT_AUDIO) < 0) return devices;

        int count = SDL_GetNumAudioDevices(1); // 1 for capture
        for (int i = 0; i < count; ++i) {
            const char* name = SDL_GetAudioDeviceName(i, 1);
            if (name) {
                devices.push_back(std::string(name));
            }
        }
        return devices;
    }
};

#endif // MICROPHONE_SOURCE_H

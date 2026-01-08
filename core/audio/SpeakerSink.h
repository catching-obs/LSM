#ifndef SPEAKER_SINK_H
#define SPEAKER_SINK_H

#include "AudioFrame.h"
#include <SDL.h>
#include <vector>
#include <mutex>
#include <list>
#include <atomic>

class SpeakerSink {
public:
    SpeakerSink();
    ~SpeakerSink();

    bool start();
    void stop();
    
    // Queue audio for playback
    void pushFrame(const AudioFrame& frame);

private:
    static void AudioCallback(void* userdata, Uint8* stream, int len);
    void processAudio(Uint8* stream, int len);

    SDL_AudioDeviceID deviceId_;
    std::atomic<bool> running_;
    
    std::mutex queueMutex_;
    std::list<float> audioQueue_; // Simple FIFO for float samples
};

#endif // SPEAKER_SINK_H

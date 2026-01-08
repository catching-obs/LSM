#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#include "AudioFrame.h"
#include <string>

class AudioSource {
public:
    virtual ~AudioSource() = default;

    virtual bool start() = 0;
    virtual void stop() = 0;
    
    // Get audio data. 
    // Returns true if frame is filled.
    virtual bool getFrame(AudioFrame& frame) = 0;
    
    virtual std::string getName() const = 0;
};

#endif // AUDIO_SOURCE_H

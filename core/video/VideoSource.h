#ifndef VIDEO_SOURCE_H
#define VIDEO_SOURCE_H

#include "VideoFrame.h"
#include <string>

class VideoSource {
public:
    virtual ~VideoSource() = default;

    // Start capturing/generating frames
    virtual bool start() = 0;

    // Stop capturing
    virtual void stop() = 0;

    // Retrieve the latest frame. Returns false if no new frame is available.
    virtual bool getFrame(VideoFrame& frame) = 0;

    // Get source identifier/name
    virtual std::string getName() const = 0;
};

#endif // VIDEO_SOURCE_H

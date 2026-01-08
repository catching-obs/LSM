#ifndef SCREEN_SOURCE_H
#define SCREEN_SOURCE_H

#include "VideoSource.h"
#include <thread>
#include <atomic>
#include <mutex>

class ScreenSource : public VideoSource {
public:
    ScreenSource(int screenIndex);
    ~ScreenSource() override;

    bool start() override;
    void stop() override;
    bool getFrame(VideoFrame& frame) override;
    std::string getName() const override;

private:
    void captureLoop();

    int screenIndex_;
    std::atomic<bool> running_;
    std::thread captureThread_;
    
    std::mutex frameMutex_;
    VideoFrame currentFrame_;
    bool newFrameAvailable_;
};

#endif // SCREEN_SOURCE_H

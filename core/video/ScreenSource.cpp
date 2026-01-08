#include "ScreenSource.h"
#include <iostream>
#include <chrono>

ScreenSource::ScreenSource(int screenIndex) 
    : screenIndex_(screenIndex), running_(false), newFrameAvailable_(false) {
}

ScreenSource::~ScreenSource() {
    stop();
}

bool ScreenSource::start() {
    if (running_) return true;
    
    running_ = true;
    captureThread_ = std::thread(&ScreenSource::captureLoop, this);
    std::cout << "ScreenSource started: Display " << screenIndex_ << std::endl;
    return true;
}

void ScreenSource::stop() {
    if (!running_) return;

    running_ = false;
    if (captureThread_.joinable()) {
        captureThread_.join();
    }
    std::cout << "ScreenSource stopped: Display " << screenIndex_ << std::endl;
}

bool ScreenSource::getFrame(VideoFrame& frame) {
    std::lock_guard<std::mutex> lock(frameMutex_);
    if (!newFrameAvailable_) {
        return false;
    }
    
    frame = currentFrame_;
    newFrameAvailable_ = false;
    return true;
}

std::string ScreenSource::getName() const {
    return "Screen-" + std::to_string(screenIndex_);
}

void ScreenSource::captureLoop() {
    // Mock capture loop: Generate frames at ~60 FPS for screen capture
    int width = 1920;
    int height = 1080;
    
    while (running_) {
        auto start = std::chrono::high_resolution_clock::now();

        // Simulate screen capture
        VideoFrame frame(width, height, VideoFrame::Format::RGBA);
        
        // Mock data
        if (!frame.data.empty()) frame.data[0] = 255; 
        
        frame.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        {
            std::lock_guard<std::mutex> lock(frameMutex_);
            currentFrame_ = std::move(frame);
            newFrameAvailable_ = true;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Sleep to maintain ~60 FPS (16ms)
        if (elapsed.count() < 16) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16 - elapsed.count()));
        }
    }
}

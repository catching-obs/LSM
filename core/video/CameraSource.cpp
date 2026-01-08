#include "CameraSource.h"
#include <iostream>
#include <chrono>

CameraSource::CameraSource(const std::string& deviceId) 
    : deviceId_(deviceId), running_(false), newFrameAvailable_(false) {
}

CameraSource::~CameraSource() {
    stop();
}

bool CameraSource::start() {
    if (running_) return true;

    // Parse device ID to integer, default to 0
    int camIndex = 0;
    try {
        // Find first digit in the string
        size_t digitPos = deviceId_.find_first_of("0123456789");
        if (digitPos != std::string::npos) {
            camIndex = std::stoi(deviceId_.substr(digitPos));
        }
    } catch (...) {
        camIndex = 0;
    }

    // Open Camera
    // CAP_ANY allows OpenCV to choose the best backend (V4L2, AVFoundation, DSHOW, etc.)
    if (!capture_.open(camIndex, cv::CAP_ANY)) {
        std::cerr << "Error: Could not open camera with index " << camIndex << std::endl;
        return false;
    }

    // Optional: Set resolution (can be parameterized later)
    capture_.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    capture_.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    
    running_ = true;
    captureThread_ = std::thread(&CameraSource::captureLoop, this);
    std::cout << "CameraSource started: " << deviceId_ << " (Index: " << camIndex << ")" << std::endl;
    return true;
}

void CameraSource::stop() {
    if (!running_) return;

    running_ = false;
    if (captureThread_.joinable()) {
        captureThread_.join();
    }
    
    if (capture_.isOpened()) {
        capture_.release();
    }
    std::cout << "CameraSource stopped: " << deviceId_ << std::endl;
}

bool CameraSource::getFrame(VideoFrame& frame) {
    std::lock_guard<std::mutex> lock(frameMutex_);
    if (!newFrameAvailable_) {
        return false;
    }
    
    frame = currentFrame_; // Copy frame data
    newFrameAvailable_ = false;
    return true;
}

std::string CameraSource::getName() const {
    return "Camera-" + deviceId_;
}

void CameraSource::captureLoop() {
    cv::Mat rawFrame;
    cv::Mat rgbaFrame;
    
    while (running_) {
        // 1. Read frame from OpenCV
        if (!capture_.read(rawFrame)) {
            // Failed to read (camera disconnected?), sleep briefly and retry
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        if (rawFrame.empty()) continue;

        // 2. Convert Color Space (BGR -> RGBA)
        // VideoFrame expects RGBA buffer
        cv::cvtColor(rawFrame, rgbaFrame, cv::COLOR_BGR2RGBA);

        // 3. Prepare VideoFrame
        VideoFrame frame(rgbaFrame.cols, rgbaFrame.rows, VideoFrame::Format::RGBA);
        
        // Copy data efficiently
        // Note: rgbaFrame.data is contiguous
        size_t dataSize = rgbaFrame.total() * rgbaFrame.elemSize();
        if (frame.data.size() != dataSize) {
            frame.data.resize(dataSize);
        }
        std::memcpy(frame.data.data(), rgbaFrame.data, dataSize);
        
        frame.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        // 4. Update shared state
        {
            std::lock_guard<std::mutex> lock(frameMutex_);
            currentFrame_ = std::move(frame);
            newFrameAvailable_ = true;
        }
        
        // No explicit sleep needed here as capture.read() blocks until next frame
    }
}

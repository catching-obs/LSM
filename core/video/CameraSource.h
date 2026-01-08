#ifndef CAMERA_SOURCE_H
#define CAMERA_SOURCE_H

#include "VideoSource.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>

class CameraSource : public VideoSource {
public:
    CameraSource(const std::string& deviceId);
    ~CameraSource() override;

    bool start() override;
    void stop() override;
    bool getFrame(VideoFrame& frame) override;
    std::string getName() const override;

private:
    void captureLoop();

    std::string deviceId_;
    std::atomic<bool> running_;
    std::thread captureThread_;
    
    // OpenCV Capture object
    cv::VideoCapture capture_;
    
    std::mutex frameMutex_;
    VideoFrame currentFrame_;
    bool newFrameAvailable_;
};

class CameraDeviceManager {
public:
    static std::vector<std::string> getDeviceList() {
        // Silence OpenCV logs during probing
        cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
        
        std::vector<std::string> devices;
        // Simple probing of indices 0-9
        for (int i = 0; i < 10; ++i) {
            cv::VideoCapture cap(i);
            if (cap.isOpened()) {
                devices.push_back("Camera " + std::to_string(i));
                cap.release();
            }
        }
        
        // Restore log level (optional, or keep silent)
        cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
        
        return devices;
    }
};

#endif // CAMERA_SOURCE_H

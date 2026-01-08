#include "../../core/video/CameraSource.h"
#include "../../core/video/VideoFrame.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <csignal>
#include <opencv2/opencv.hpp> // Include for imshow

// Signal handler for graceful shutdown
std::atomic<bool> g_running(true);
void signalHandler(int signum) {
    g_running = false;
}

int main() {
    // Register signal handler
    signal(SIGINT, signalHandler);

    std::cout << "=== Camera Integration Test ===" << std::endl;
    std::cout << "Attempting to open default camera (index 0)..." << std::endl;

    // 1. Create Camera Source
    CameraSource camera("0");

    // 2. Start Camera
    if (!camera.start()) {
        std::cerr << "Failed to start camera! Exiting." << std::endl;
        return 1;
    }
    std::cout << "Camera started successfully." << std::endl;
    std::cout << "Opening 'Camera Test' window..." << std::endl;

    // 3. Frame Loop
    std::cout << "Press ESC or Ctrl+C to stop..." << std::endl;
    
    int frameCount = 0;
    auto lastReport = std::chrono::steady_clock::now();
    VideoFrame frame;

    while (g_running) {
        if (camera.getFrame(frame)) {
            frameCount++;
            
            if (!frame.data.empty()) {
                // Wrap raw data in cv::Mat
                // Note: VideoFrame is RGBA, cv::imshow expects BGR usually
                cv::Mat rgbaFrame(frame.height, frame.width, CV_8UC4, frame.data.data());
                cv::Mat bgrFrame;
                cv::cvtColor(rgbaFrame, bgrFrame, cv::COLOR_RGBA2BGR);

                cv::imshow("Camera Test", bgrFrame);
                
                // Wait for 1ms and check for ESC key (27)
                int key = cv::waitKey(1);
                if (key == 27) {
                    g_running = false;
                    break;
                }
            }
        } else {
            // Small sleep to prevent busy waiting if no frame is ready
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        // Report FPS every second
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastReport).count() >= 1) {
            std::cout << "FPS: " << frameCount << " | Resolution: " 
                      << frame.width << "x" << frame.height << std::endl;
            frameCount = 0;
            lastReport = now;
        }
    }

    // 4. Cleanup
    std::cout << "Stopping camera..." << std::endl;
    camera.stop();
    cv::destroyAllWindows();
    std::cout << "Test finished." << std::endl;

    return 0;
}

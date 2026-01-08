#include "video/SourceManager.h"
#include "video/CameraSource.h"
#include "video/ScreenSource.h"
#include "streaming/StreamController.h"
#include <iostream>
#include <thread>
#include <chrono>

// Simple test driver for Kim Seung-woo's module
int main() {
    std::cout << "=== Kim Seung-woo Module Test ===\n" << std::endl;

    // 1. Initialize Source Manager
    auto& manager = SourceManager::getInstance();

    // 2. Create and Register Sources
    auto cam1 = std::make_shared<CameraSource>("webcam-01");
    auto screen1 = std::make_shared<ScreenSource>(0); // Primary display

    manager.addSource(cam1);
    manager.addSource(screen1);

    // 3. Start Sources
    std::cout << "\n[Step 1] Starting Sources..." << std::endl;
    cam1->start();
    screen1->start();

    // 4. Initialize Stream Controller
    StreamController streamer;
    std::cout << "\n[Step 2] Initializing Stream Controller..." << std::endl;
    streamer.startStreaming("rtmp://live.twitch.tv/app/stream_key");

    // 5. Simulation Loop (Get frames and process)
    std::cout << "\n[Step 3] Running processing loop for 3 seconds..." << std::endl;
    auto startTime = std::chrono::steady_clock::now();
    
    while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(3)) {
        VideoFrame frame;
        
        // Try to get frame from Camera
        if (cam1->getFrame(frame)) {
            // In a real engine, this would go to the renderer/encoder
            // std::cout << "Got Camera Frame: " << frame.width << "x" << frame.height 
            //           << " TS: " << frame.timestamp << std::endl;
        }

        // Try to get frame from Screen
        if (screen1->getFrame(frame)) {
            // std::cout << "Got Screen Frame: " << frame.width << "x" << frame.height << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // 6. Network Adaptation Test
    std::cout << "\n[Step 4] Testing Network Adaptation..." << std::endl;
    streamer.updateNetworkStats();

    // 7. Cleanup
    std::cout << "\n[Step 5] Shutting down..." << std::endl;
    streamer.stopStreaming();
    manager.removeSource("Camera-webcam-01"); // This calls stop() internally
    manager.removeSource("Screen-0");

    std::cout << "\n=== Test Complete ===" << std::endl;
    return 0;
}

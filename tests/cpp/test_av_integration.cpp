#include "../../core/video/CameraSource.h"
#include "../../core/video/VideoFrame.h"
#include "../../core/audio/MicrophoneSource.h"
#include "../../core/audio/AudioFrame.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <csignal>
#include <vector>

// Signal handler for graceful shutdown
std::atomic<bool> g_running(true);
void signalHandler(int signum) {
    g_running = false;
}

int main() {
    // Register signal handler
    signal(SIGINT, signalHandler);

    std::cout << "=== A/V Integration Test ===" << std::endl;

    // 1. Initialize Sources
    std::cout << "[Video] Attempting to open default camera (index 0)..." << std::endl;
    CameraSource camera("0");

    std::cout << "[Audio] Initializing Virtual Microphone..." << std::endl;
    MicrophoneSource mic("default-mic");

    // 2. Start Sources
    if (!camera.start()) {
        std::cerr << "Failed to start camera! Continuing with audio only..." << std::endl;
    } else {
        std::cout << "Camera started." << std::endl;
    }

    if (!mic.start()) {
        std::cerr << "Failed to start microphone!" << std::endl;
    } else {
        std::cout << "Microphone started." << std::endl;
    }

    // 3. Processing Loop
    std::cout << "Running capture loop. Press Ctrl+C to stop..." << std::endl;
    
    int videoFrames = 0;
    int audioFrames = 0;
    auto lastReport = std::chrono::steady_clock::now();
    
    VideoFrame vFrame;
    AudioFrame aFrame;

    while (g_running) {
        bool hasActivity = false;

        // Video
        if (camera.getFrame(vFrame)) {
            videoFrames++;
            hasActivity = true;
        }

        // Audio
        if (mic.getFrame(aFrame)) {
            audioFrames++;
            hasActivity = true;
            
            // Simple validation: Check if we have non-zero data (sine wave)
            if (!aFrame.data.empty() && aFrame.data[0] == 0.0f && aFrame.data[10] == 0.0f) {
                 // Should be silent if mocked with 0, but our mock uses sine wave
            }
        }

        if (!hasActivity) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        // Report Stats
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastReport).count() >= 1) {
            std::cout << "STATS | Video: " << videoFrames << " fps (" << vFrame.width << "x" << vFrame.height << ")"
                      << " | Audio: " << audioFrames << " chunks"
                      << " (" << (audioFrames * 1024.0 / 44100.0) << " sec)" << std::endl;
            
            videoFrames = 0;
            audioFrames = 0;
            lastReport = now;
        }
    }

    // 4. Cleanup
    std::cout << "Stopping sources..." << std::endl;
    camera.stop();
    mic.stop();
    std::cout << "Test finished." << std::endl;

    return 0;
}

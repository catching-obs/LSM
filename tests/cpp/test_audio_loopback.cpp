#include "../../core/audio/MicrophoneSource.h"
#include "../../core/audio/SpeakerSink.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <csignal>

std::atomic<bool> g_running(true);
void signalHandler(int signum) {
    g_running = false;
}

int main() {
    signal(SIGINT, signalHandler);

    std::cout << "=== Audio Loopback Test ===" << std::endl;
    std::cout << "Make noise! It should be played back." << std::endl;

    MicrophoneSource mic("default");
    SpeakerSink speaker;

    if (!mic.start()) {
        std::cerr << "Failed to start Mic" << std::endl;
        return 1;
    }
    if (!speaker.start()) {
        std::cerr << "Failed to start Speaker" << std::endl;
        return 1;
    }

    AudioFrame frame;
    // Main loop: Move data from Mic to Speaker
    while (g_running) {
        // Try to get data
        if (mic.getFrame(frame)) {
            // Push to speaker
            speaker.pushFrame(frame);
        } else {
            // Yield if no data
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    mic.stop();
    speaker.stop();
    return 0;
}

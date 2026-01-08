#include "../../core/video/CameraSource.h"
#include "../../core/audio/MicrophoneSource.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

void listVideoDevices() {
    std::cout << "\n--- Available Video Devices ---" << std::endl;
    auto devices = CameraDeviceManager::getDeviceList();
    if (devices.empty()) {
        std::cout << "No cameras found." << std::endl;
    } else {
        for (const auto& dev : devices) {
            std::cout << " - " << dev << std::endl;
        }
    }
}

void listAudioDevices() {
    std::cout << "\n--- Available Audio Devices ---" << std::endl;
    auto devices = AudioDeviceManager::getInputDevices();
    if (devices.empty()) {
        std::cout << "No audio input devices found." << std::endl;
    } else {
        for (const auto& dev : devices) {
            std::cout << " - " << dev << std::endl;
        }
    }
}

int main() {
    std::cout << "=== Device Enumeration & Switching Test ===" << std::endl;

    // 1. List Devices
    listVideoDevices();
    listAudioDevices();

    // 2. Camera Switching Demo
    std::cout << "\n[Test] Starting Camera 0..." << std::endl;
    {
        CameraSource cam1("Camera 0");
        if (cam1.start()) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::cout << "Stopping Camera 0..." << std::endl;
            cam1.stop();
        } else {
            std::cerr << "Failed to start Camera 0" << std::endl;
        }
    }

    // Try Camera 1 if exists
    std::cout << "\n[Test] Switching to Camera 1 (if available)..." << std::endl;
    {
        // Simple check if we have more than 1 device in the list
        auto devices = CameraDeviceManager::getDeviceList();
        std::string target = (devices.size() > 1) ? devices[1] : "Camera 0";
        
        std::cout << "Targeting: " << target << std::endl;
        CameraSource cam2(target);
        if (cam2.start()) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            cam2.stop();
        }
    }

    // 3. Audio Switching Demo
    std::cout << "\n[Test] Starting Default Microphone..." << std::endl;
    {
        MicrophoneSource mic1("default");
        if (mic1.start()) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::cout << "Stopping Default Mic..." << std::endl;
            mic1.stop();
        }
    }

    std::cout << "\n[Test] Switching to Specific Microphone..." << std::endl;
    {
        auto devices = AudioDeviceManager::getInputDevices();
        if (!devices.empty()) {
            std::string target = devices[0]; // Pick first specific device
            std::cout << "Targeting: " << target << std::endl;
            
            MicrophoneSource mic2(target);
            if (mic2.start()) {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                mic2.stop();
            } else {
                std::cerr << "Failed to open specific mic: " << target << std::endl;
            }
        } else {
            std::cout << "No specific audio devices found to switch to." << std::endl;
        }
    }

    std::cout << "\n=== Test Complete ===" << std::endl;
    return 0;
}

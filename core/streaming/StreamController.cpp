#include "StreamController.h"
#include <iostream>

StreamController::StreamController() : currentState_(State::IDLE) {
}

StreamController::~StreamController() {
    stopStreaming();
}

bool StreamController::startStreaming(const std::string& url) {
    if (currentState_ == State::STREAMING) {
        std::cout << "Already streaming." << std::endl;
        return false;
    }

    std::cout << "Initializing stream to " << url << "..." << std::endl;
    currentState_ = State::INITIALIZING;
    currentUrl_ = url;

    // Simulate connection logic
    // In real implementation, this would initialize WebRTC/RTMP streamers
    
    currentState_ = State::STREAMING;
    std::cout << "Streaming started." << std::endl;
    return true;
}

void StreamController::stopStreaming() {
    if (currentState_ == State::IDLE) return;

    std::cout << "Stopping stream..." << std::endl;
    // Teardown logic here
    
    currentState_ = State::IDLE;
    currentUrl_.clear();
    std::cout << "Stream stopped." << std::endl;
}

StreamController::State StreamController::getState() const {
    return currentState_;
}

void StreamController::updateNetworkStats() {
    // Logic to check bitrate, packet loss, etc.
    // If stats are bad -> trigger State::RECOVERING or adjust bitrate
    if (currentState_ == State::STREAMING) {
        // Mock check
        // std::cout << "Network stats updated." << std::endl;
    }
}

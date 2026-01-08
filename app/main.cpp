#include <iostream>
#include <vector>
#include <cstring>
#include <thread>
#include <chrono>

// Core utilities
#include "utils/logger.h"

// Rust FFI bindings
#include "audio_ffi.h"
#include "video_ffi.h"

void demonstrate_audio_mixing() {
    std::cout << "\n=== Audio Mixing Demo ===" << std::endl;

    // Create a 2-channel audio mixer
    AudioMixer* mixer = audio_mixer_create(2);
    if (!mixer) {
        std::cerr << "Failed to create audio mixer" << std::endl;
        return;
    }

    // Prepare test audio data
    const size_t frame_count = 100;
    std::vector<float> channel1(frame_count, 0.5f);  // 50% amplitude
    std::vector<float> channel2(frame_count, 0.3f);  // 30% amplitude
    std::vector<float> output(frame_count, 0.0f);

    const float* input_buffers[2] = {channel1.data(), channel2.data()};

    // Mix audio
    bool success = audio_mixer_mix(mixer, input_buffers, output.data(), frame_count);
    if (success) {
        std::cout << "Mixed audio successfully!" << std::endl;
        std::cout << "First output sample: " << output[0] << " (expected: 0.8)" << std::endl;
    } else {
        std::cerr << "Audio mixing failed" << std::endl;
    }

    // Adjust channel volumes
    audio_mixer_set_channel_volume(mixer, 0, 1.0f);  // Full volume
    audio_mixer_set_channel_volume(mixer, 1, 0.0f);  // Mute channel 2

    // Mix again with new volumes
    std::fill(output.begin(), output.end(), 0.0f);
    audio_mixer_mix(mixer, input_buffers, output.data(), frame_count);
    std::cout << "After volume adjustment: " << output[0] << " (expected: 0.5)" << std::endl;

    // Clean up
    audio_mixer_destroy(mixer);
}

void demonstrate_video_encoding() {
    std::cout << "\n=== Video Encoding Demo ===" << std::endl;

    // Configure encoder
    CEncoderConfig config;
    config.width = 1920;
    config.height = 1080;
    config.fps = 30;
    config.bitrate = 5000000;  // 5 Mbps

    // Create encoder
    EncoderPipeline* encoder = encoder_create(config);
    if (!encoder) {
        std::cerr << "Failed to create encoder pipeline" << std::endl;
        return;
    }

    // Start encoder
    if (!encoder_start(encoder)) {
        std::cerr << "Failed to start encoder" << std::endl;
        encoder_destroy(encoder);
        return;
    }

    std::cout << "Encoder started successfully" << std::endl;

    // Submit some test frames
    const size_t frame_size = 1920 * 1080 * 3;  // RGB24
    std::vector<uint8_t> frame_data(frame_size, 128);  // Gray frame

    for (int i = 0; i < 10; ++i) {
        bool success = encoder_submit_frame(
            encoder,
            frame_data.data(),
            frame_size,
            i * 33333  // ~30fps timestamp in microseconds
        );

        if (success) {
            std::cout << "Submitted frame " << i << std::endl;
        } else {
            std::cerr << "Failed to submit frame " << i << std::endl;
            break;
        }
    }

    // Wait a bit for encoding to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Get statistics
    uint64_t encoded = encoder_get_encoded_frames(encoder);
    std::cout << "Encoded frames: " << encoded << std::endl;

    // Stop and clean up
    encoder_stop(encoder);
    encoder_destroy(encoder);
}

int main() {
    // Initialize logging
    using namespace core::utils;
    Logger::getInstance().setLogLevel(LogLevel::INFO);
    Logger::getInstance().log(LogLevel::INFO, "Hybrid Media Engine - Demo Application");

    try {
        // Demonstrate audio mixing
        demonstrate_audio_mixing();

        // Demonstrate video encoding
        demonstrate_video_encoding();

        Logger::getInstance().log(LogLevel::INFO, "Demo completed successfully!");
        return 0;

    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::ERROR,
            std::string("Exception: ") + e.what());
        return 1;
    }
}

#ifndef VIDEO_FRAME_H
#define VIDEO_FRAME_H

#include <vector>
#include <cstdint>
#include <memory>

struct VideoFrame {
    int width;
    int height;
    uint64_t timestamp; // Microseconds
    std::vector<uint8_t> data; // Raw pixel data (e.g., RGBA or YUV)
    
    enum class Format {
        RGBA,
        I420,
        NV12
    } format;

    VideoFrame(int w = 0, int h = 0, Format fmt = Format::RGBA) 
        : width(w), height(h), timestamp(0), format(fmt) {
        if (w > 0 && h > 0) {
            size_t size = w * h * 4; 
            data.resize(size);
        }
    }
};

#endif // VIDEO_FRAME_H

#pragma once

#include <string>
#include <cstdint>

namespace talos {
namespace encoder {

/**
 * @brief Encoder configuration
 */
struct EncoderConfig {
    // Video parameters
    int width = 1920;
    int height = 1080;
    int framerate = 30;
    int bitrate = 4000000;  // 4 Mbps
    int gopSize = 60;       // Keyframe interval
    
    // Codec settings
    std::string codec = "h264";  // h264, h265, av1
    std::string preset = "fast"; // ultrafast, superfast, veryfast, faster, fast, medium, slow, slower, veryslow
    std::string profile = "main"; // baseline, main, high
    
    // Quality settings
    int crf = 23;  // Constant Rate Factor (0-51, lower is better quality)
    bool useHardwareAccel = true;
    
    // Performance settings
    int threadCount = 0;  // 0 = auto
};

/**
 * @brief Encoder statistics
 */
struct EncoderStats {
    uint64_t framesEncoded = 0;
    uint64_t bytesEncoded = 0;
    uint64_t packetsGenerated = 0;
    float averageFps = 0.0f;
    float currentBitrate = 0.0f;
    uint64_t keyFrames = 0;
};

} // namespace encoder
} // namespace talos
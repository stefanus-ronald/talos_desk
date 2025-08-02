#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include "encoder/encoder_types.h"

namespace talos {

// Forward declarations
namespace capture {
    struct Frame;
}

/**
 * @brief Video encoder interface
 */
class VideoEncoder {
public:
    VideoEncoder() = default;
    virtual ~VideoEncoder() = default;
    
    /**
     * @brief Factory method to create encoder instances
     * @param type Encoder type (e.g., "ffmpeg")
     * @return Unique pointer to encoder instance
     */
    static std::unique_ptr<VideoEncoder> create(const std::string& type = "ffmpeg");
    
    /**
     * @brief Initialize the encoder with configuration
     * @param config Encoder configuration
     * @return true if successful, false otherwise
     */
    virtual bool initialize(const encoder::EncoderConfig& config) = 0;
    
    /**
     * @brief Shutdown the encoder
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Encode a captured frame
     * @param frame The frame to encode
     * @return true if successful, false otherwise
     */
    virtual bool encodeFrame(const capture::Frame& frame) = 0;
    
    /**
     * @brief Check if encoder is initialized
     * @return true if initialized, false otherwise
     */
    virtual bool isInitialized() const = 0;
    
    /**
     * @brief Get the next encoded packet
     * @param packet Output buffer for encoded data
     * @return true if packet is available, false otherwise
     */
    virtual bool getEncodedPacket(std::vector<uint8_t>& packet) = 0;
    
    /**
     * @brief Get encoder statistics
     * @return Current encoder statistics
     */
    virtual encoder::EncoderStats getStats() const = 0;
};

} // namespace talos
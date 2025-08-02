#pragma once

#include "encoder/video_encoder.h"
#include <memory>
#include <mutex>
#include <atomic>

// Forward declare FFmpeg types to avoid including headers
extern "C" {
    struct AVCodecContext;
    struct AVFrame;
    struct AVPacket;
    struct AVCodec;
    struct SwsContext;
}

namespace talos {
namespace encoder {

/**
 * @brief FFmpeg-based video encoder implementation
 */
class FFmpegEncoder : public VideoEncoder {
public:
    FFmpegEncoder();
    ~FFmpegEncoder() override;
    
    // VideoEncoder interface
    bool initialize(const EncoderConfig& config) override;
    void shutdown() override;
    bool encodeFrame(const capture::Frame& frame) override;
    bool isInitialized() const override { return m_initialized; }
    
    /**
     * @brief Get the next encoded packet
     * @param packet Output buffer for encoded data
     * @return true if packet is available, false otherwise
     */
    bool getEncodedPacket(std::vector<uint8_t>& packet) override;
    
    /**
     * @brief Get encoder statistics
     * @return Current encoder statistics
     */
    EncoderStats getStats() const override;
    
private:
    // Helper methods
    bool initializeCodec(const EncoderConfig& config);
    bool initializeScaler(int srcWidth, int srcHeight, int srcFormat);
    void cleanupFFmpeg();
    bool convertFrame(const capture::Frame& frame, AVFrame* avFrame);
    bool encodeAVFrame(AVFrame* frame);
    
    // FFmpeg contexts
    AVCodecContext* m_codecContext;
    const AVCodec* m_codec;
    SwsContext* m_swsContext;
    AVFrame* m_frame;
    AVPacket* m_packet;
    
    // Configuration
    EncoderConfig m_config;
    
    // State
    std::atomic<bool> m_initialized;
    mutable std::mutex m_statsMutex;
    EncoderStats m_stats;
    
    // Frame management
    int64_t m_frameNumber;
    int64_t m_pts;
};

} // namespace encoder
} // namespace talos
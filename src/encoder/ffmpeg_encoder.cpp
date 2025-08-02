#include "encoder/ffmpeg_encoder.h"
#include "capture/capture_engine.h"
#include "core/logger.h"

#ifndef NO_FFMPEG

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>
}

#include <chrono>
#include <sstream>

namespace talos {
namespace encoder {

FFmpegEncoder::FFmpegEncoder()
    : m_codecContext(nullptr)
    , m_codec(nullptr)
    , m_swsContext(nullptr)
    , m_frame(nullptr)
    , m_packet(nullptr)
    , m_initialized(false)
    , m_frameNumber(0)
    , m_pts(0) {
}

FFmpegEncoder::~FFmpegEncoder() {
    shutdown();
}

bool FFmpegEncoder::initialize(const EncoderConfig& config) {
    if (m_initialized) {
        Logger::getInstance().log(LogLevel::Warning, "FFmpeg encoder already initialized");
        return true;
    }
    
    m_config = config;
    
    // Initialize FFmpeg
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100)
    avcodec_register_all();
#endif
    
    // Initialize codec
    if (!initializeCodec(config)) {
        Logger::getInstance().log(LogLevel::Error, "Failed to initialize codec");
        cleanupFFmpeg();
        return false;
    }
    
    // Allocate frame and packet
    m_frame = av_frame_alloc();
    if (!m_frame) {
        Logger::getInstance().log(LogLevel::Error, "Failed to allocate frame");
        cleanupFFmpeg();
        return false;
    }
    
    m_frame->format = m_codecContext->pix_fmt;
    m_frame->width = m_codecContext->width;
    m_frame->height = m_codecContext->height;
    
    int ret = av_frame_get_buffer(m_frame, 0);
    if (ret < 0) {
        Logger::getInstance().log(LogLevel::Error, "Failed to allocate frame buffer");
        cleanupFFmpeg();
        return false;
    }
    
    m_packet = av_packet_alloc();
    if (!m_packet) {
        Logger::getInstance().log(LogLevel::Error, "Failed to allocate packet");
        cleanupFFmpeg();
        return false;
    }
    
    // Initialize scaler for BGRA to YUV420P conversion
    if (!initializeScaler(config.width, config.height, AV_PIX_FMT_BGRA)) {
        Logger::getInstance().log(LogLevel::Error, "Failed to initialize scaler");
        cleanupFFmpeg();
        return false;
    }
    
    m_initialized = true;
    Logger::getInstance().log(LogLevel::Info, "FFmpeg encoder initialized successfully");
    
    return true;
}

bool FFmpegEncoder::initializeCodec(const EncoderConfig& config) {
    // Find codec
    AVCodecID codecId = AV_CODEC_ID_H264;
    if (config.codec == "h265") {
        codecId = AV_CODEC_ID_H265;
    } else if (config.codec == "av1") {
        codecId = AV_CODEC_ID_AV1;
    }
    
    // Try hardware accelerated encoder first if requested
    if (config.useHardwareAccel) {
        std::vector<std::string> hwEncoders;
        
        if (codecId == AV_CODEC_ID_H264) {
            #ifdef _WIN32
            hwEncoders = {"h264_nvenc", "h264_qsv", "h264_amf"};
            #elif defined(__APPLE__)
            hwEncoders = {"h264_videotoolbox"};
            #else
            hwEncoders = {"h264_nvenc", "h264_vaapi"};
            #endif
        } else if (codecId == AV_CODEC_ID_H265) {
            #ifdef _WIN32
            hwEncoders = {"hevc_nvenc", "hevc_qsv", "hevc_amf"};
            #elif defined(__APPLE__)
            hwEncoders = {"hevc_videotoolbox"};
            #else
            hwEncoders = {"hevc_nvenc", "hevc_vaapi"};
            #endif
        }
        
        for (const auto& encoderName : hwEncoders) {
            m_codec = avcodec_find_encoder_by_name(encoderName.c_str());
            if (m_codec) {
                Logger::getInstance().log(LogLevel::Info, "Using hardware encoder: " + encoderName);
                break;
            }
        }
    }
    
    // Fall back to software encoder
    if (!m_codec) {
        m_codec = avcodec_find_encoder(codecId);
        if (!m_codec) {
            Logger::getInstance().log(LogLevel::Error, "Failed to find encoder for codec: " + config.codec);
            return false;
        }
        Logger::getInstance().log(LogLevel::Info, "Using software encoder: " + std::string(m_codec->name));
    }
    
    // Allocate codec context
    m_codecContext = avcodec_alloc_context3(m_codec);
    if (!m_codecContext) {
        Logger::getInstance().log(LogLevel::Error, "Failed to allocate codec context");
        return false;
    }
    
    // Set codec parameters
    m_codecContext->width = config.width;
    m_codecContext->height = config.height;
    m_codecContext->time_base = {1, config.framerate};
    m_codecContext->framerate = {config.framerate, 1};
    m_codecContext->gop_size = config.gopSize;
    m_codecContext->max_b_frames = 2;
    m_codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    
    // Set bitrate or CRF
    if (config.crf >= 0) {
        av_opt_set_int(m_codecContext->priv_data, "crf", config.crf, 0);
    } else {
        m_codecContext->bit_rate = config.bitrate;
    }
    
    // Set preset
    av_opt_set(m_codecContext->priv_data, "preset", config.preset.c_str(), 0);
    
    // Set profile
    if (config.profile == "baseline") {
        m_codecContext->profile = FF_PROFILE_H264_BASELINE;
    } else if (config.profile == "main") {
        m_codecContext->profile = FF_PROFILE_H264_MAIN;
    } else if (config.profile == "high") {
        m_codecContext->profile = FF_PROFILE_H264_HIGH;
    }
    
    // Set thread count
    if (config.threadCount > 0) {
        m_codecContext->thread_count = config.threadCount;
    }
    
    // Open codec
    int ret = avcodec_open2(m_codecContext, m_codec, nullptr);
    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errbuf, sizeof(errbuf));
        Logger::getInstance().log(LogLevel::Error, "Failed to open codec: " + std::string(errbuf));
        return false;
    }
    
    return true;
}

bool FFmpegEncoder::initializeScaler(int srcWidth, int srcHeight, int srcFormat) {
    m_swsContext = sws_getContext(
        srcWidth, srcHeight, static_cast<AVPixelFormat>(srcFormat),
        m_codecContext->width, m_codecContext->height, m_codecContext->pix_fmt,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );
    
    if (!m_swsContext) {
        Logger::getInstance().log(LogLevel::Error, "Failed to create scaler context");
        return false;
    }
    
    return true;
}

void FFmpegEncoder::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    // Flush encoder
    if (m_codecContext) {
        encodeAVFrame(nullptr);
    }
    
    cleanupFFmpeg();
    m_initialized = false;
    
    Logger::getInstance().log(LogLevel::Info, "FFmpeg encoder shut down");
}

void FFmpegEncoder::cleanupFFmpeg() {
    if (m_swsContext) {
        sws_freeContext(m_swsContext);
        m_swsContext = nullptr;
    }
    
    if (m_frame) {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }
    
    if (m_packet) {
        av_packet_free(&m_packet);
        m_packet = nullptr;
    }
    
    if (m_codecContext) {
        avcodec_free_context(&m_codecContext);
        m_codecContext = nullptr;
    }
}

bool FFmpegEncoder::encodeFrame(const capture::Frame& frame) {
    if (!m_initialized) {
        Logger::getInstance().log(LogLevel::Error, "Encoder not initialized");
        return false;
    }
    
    // Make frame writable
    int ret = av_frame_make_writable(m_frame);
    if (ret < 0) {
        Logger::getInstance().log(LogLevel::Error, "Failed to make frame writable");
        return false;
    }
    
    // Convert frame
    if (!convertFrame(frame, m_frame)) {
        Logger::getInstance().log(LogLevel::Error, "Failed to convert frame");
        return false;
    }
    
    // Set PTS
    m_frame->pts = m_pts++;
    
    // Encode frame
    if (!encodeAVFrame(m_frame)) {
        Logger::getInstance().log(LogLevel::Error, "Failed to encode frame");
        return false;
    }
    
    // Update stats
    {
        std::lock_guard<std::mutex> lock(m_statsMutex);
        m_stats.framesEncoded++;
    }
    
    return true;
}

bool FFmpegEncoder::convertFrame(const capture::Frame& frame, AVFrame* avFrame) {
    // Setup source data
    const uint8_t* srcData[4] = {frame.data.data(), nullptr, nullptr, nullptr};
    int srcLinesize[4] = {static_cast<int>(frame.width * 4), 0, 0, 0}; // BGRA = 4 bytes per pixel
    
    // Convert
    int ret = sws_scale(
        m_swsContext,
        srcData, srcLinesize, 0, frame.height,
        avFrame->data, avFrame->linesize
    );
    
    if (ret != static_cast<int>(frame.height)) {
        Logger::getInstance().log(LogLevel::Error, "Failed to scale frame");
        return false;
    }
    
    return true;
}

bool FFmpegEncoder::encodeAVFrame(AVFrame* frame) {
    // Send frame to encoder
    int ret = avcodec_send_frame(m_codecContext, frame);
    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errbuf, sizeof(errbuf));
        Logger::getInstance().log(LogLevel::Error, "Failed to send frame to encoder: " + std::string(errbuf));
        return false;
    }
    
    // Receive packets from encoder
    while (ret >= 0) {
        ret = avcodec_receive_packet(m_codecContext, m_packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            char errbuf[AV_ERROR_MAX_STRING_SIZE];
            av_strerror(ret, errbuf, sizeof(errbuf));
            Logger::getInstance().log(LogLevel::Error, "Failed to receive packet from encoder: " + std::string(errbuf));
            return false;
        }
        
        // Update stats
        {
            std::lock_guard<std::mutex> lock(m_statsMutex);
            m_stats.packetsGenerated++;
            m_stats.bytesEncoded += m_packet->size;
            
            if (m_packet->flags & AV_PKT_FLAG_KEY) {
                m_stats.keyFrames++;
            }
        }
        
        // Packet will be retrieved via getEncodedPacket()
        break;
    }
    
    return true;
}

bool FFmpegEncoder::getEncodedPacket(std::vector<uint8_t>& packet) {
    if (!m_initialized || !m_packet->data) {
        return false;
    }
    
    // Copy packet data
    packet.resize(m_packet->size);
    std::memcpy(packet.data(), m_packet->data, m_packet->size);
    
    // Free packet data
    av_packet_unref(m_packet);
    
    return true;
}

EncoderStats FFmpegEncoder::getStats() const {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    
    // Calculate FPS and bitrate
    EncoderStats stats = m_stats;
    
    if (stats.framesEncoded > 0) {
        // This is a simplified calculation - in production you'd use actual time measurements
        stats.averageFps = static_cast<float>(m_config.framerate);
        stats.currentBitrate = static_cast<float>(stats.bytesEncoded * 8) / 
                              (static_cast<float>(stats.framesEncoded) / m_config.framerate);
    }
    
    return stats;
}

} // namespace encoder
} // namespace talos

#else // NO_FFMPEG

namespace talos {
namespace encoder {

FFmpegEncoder::FFmpegEncoder()
    : m_codecContext(nullptr)
    , m_codec(nullptr)
    , m_swsContext(nullptr)
    , m_frame(nullptr)
    , m_packet(nullptr)
    , m_initialized(false)
    , m_frameNumber(0)
    , m_pts(0) {
}

FFmpegEncoder::~FFmpegEncoder() {
}

bool FFmpegEncoder::initialize(const EncoderConfig& config) {
    Logger::getInstance().log(LogLevel::Error, "FFmpeg support not compiled in");
    return false;
}

void FFmpegEncoder::shutdown() {
}

bool FFmpegEncoder::encodeFrame(const capture::Frame& frame) {
    return false;
}

bool FFmpegEncoder::getEncodedPacket(std::vector<uint8_t>& packet) {
    return false;
}

EncoderStats FFmpegEncoder::getStats() const {
    return EncoderStats();
}

void FFmpegEncoder::cleanupFFmpeg() {
}

bool FFmpegEncoder::initializeCodec(const EncoderConfig& config) {
    return false;
}

bool FFmpegEncoder::initializeScaler(int srcWidth, int srcHeight, int srcFormat) {
    return false;
}

bool FFmpegEncoder::convertFrame(const capture::Frame& frame, AVFrame* avFrame) {
    return false;
}

bool FFmpegEncoder::encodeAVFrame(AVFrame* frame) {
    return false;
}

} // namespace encoder
} // namespace talos

#endif // NO_FFMPEG
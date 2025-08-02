#include "encoder/video_encoder.h"
#include "encoder/ffmpeg_encoder.h"
#include "core/logger.h"
#include <memory>

namespace talos {

// Factory method to create encoder instances
std::unique_ptr<VideoEncoder> VideoEncoder::create(const std::string& type) {
#ifndef NO_FFMPEG
    if (type == "ffmpeg" || type.empty()) {
        return std::make_unique<encoder::FFmpegEncoder>();
    }
#endif
    
    Logger::getInstance().log(LogLevel::Error, "No encoder available for type: " + type);
    return nullptr;
}

} // namespace talos
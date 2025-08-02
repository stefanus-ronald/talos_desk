#include "capture/capture_engine.h"
#include "core/logger.h"
#include <memory>

#ifdef PLATFORM_WINDOWS
#include "capture/windows_capture_engine.h"
#elif defined(PLATFORM_MACOS)
#include "capture/macos_capture_engine.h"
#elif defined(PLATFORM_LINUX)
// TODO: Include Linux capture engine
#endif

namespace talos {
namespace capture {

// Factory method to create platform-specific capture engine
std::unique_ptr<ICaptureEngine> createCaptureEngine() {
#ifdef PLATFORM_WINDOWS
    return std::make_unique<WindowsCaptureEngine>();
#elif defined(PLATFORM_MACOS)
    return std::make_unique<MacOSCaptureEngine>();
#elif defined(PLATFORM_LINUX)
    Logger::getInstance().log(LogLevel::Error, "Linux capture engine not yet implemented");
    return nullptr;
#else
    Logger::getInstance().log(LogLevel::Error, "Unknown platform");
    return nullptr;
#endif
}

} // namespace capture
} // namespace talos
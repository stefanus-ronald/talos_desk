#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include <string>

namespace talos {

// Forward declarations
namespace capture {
    struct Frame;
    struct CaptureStats;
    struct MonitorInfo;
}

/**
 * @brief Interface for platform-specific screen capture engines
 */
class ICaptureEngine {
public:
    ICaptureEngine() = default;
    virtual ~ICaptureEngine() = default;
    
    /**
     * @brief Initialize the capture engine
     * @return true if successful, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Shutdown the capture engine
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Start capturing frames
     * @return true if successful, false otherwise
     */
    virtual bool startCapture() = 0;
    
    /**
     * @brief Stop capturing frames
     */
    virtual void stopCapture() = 0;
    
    /**
     * @brief Check if capture is active
     * @return true if capturing, false otherwise
     */
    virtual bool isCapturing() const = 0;
    
    /**
     * @brief Get the next captured frame
     * @param timeoutMs Timeout in milliseconds
     * @return Captured frame or nullptr if no frame available
     */
    virtual std::shared_ptr<capture::Frame> getNextFrame(uint32_t timeoutMs = 100) = 0;
    
    /**
     * @brief Get capture statistics
     * @return Current capture statistics
     */
    virtual capture::CaptureStats getStats() const = 0;
    
    /**
     * @brief Set the monitor to capture
     * @param monitorIndex Index of the monitor
     * @return true if successful
     */
    virtual bool setMonitor(int monitorIndex) = 0;
    
    /**
     * @brief Get available monitors
     * @return List of available monitors
     */
    virtual std::vector<capture::MonitorInfo> getAvailableMonitors() const = 0;
};

namespace capture {

/**
 * @brief Pixel format enumeration
 */
enum class PixelFormat {
    UNKNOWN,
    BGRA8,      // 32-bit BGRA (Windows native)
    RGBA8,      // 32-bit RGBA
    RGB8,       // 24-bit RGB
    YUV420P,    // YUV 4:2:0 planar
    NV12        // YUV 4:2:0 semi-planar
};

/**
 * @brief Captured frame data
 */
struct Frame {
    int width;              // Frame width in pixels
    int height;             // Frame height in pixels
    int stride;             // Bytes per row (may include padding)
    PixelFormat pixelFormat; // Pixel format
    uint64_t timestamp;     // Timestamp in microseconds
    std::vector<uint8_t> data; // Frame data
};

/**
 * @brief Capture statistics
 */
struct CaptureStats {
    uint64_t framesCapture = 0;  // Total frames captured
    uint64_t framesDropped = 0;   // Frames dropped due to queue overflow
    uint64_t bytesCapture = 0;    // Total bytes captured
    float averageFps = 0.0f;      // Average frames per second
    float currentFps = 0.0f;      // Current frames per second
};

/**
 * @brief Monitor information
 */
struct MonitorInfo {
    uint32_t id;           // Monitor ID
    std::string name;      // Monitor name/identifier
    int width;             // Width in pixels
    int height;            // Height in pixels
    int x;                 // X position
    int y;                 // Y position
    float refreshRate;     // Refresh rate in Hz
    bool isPrimary;        // Is primary monitor
};

/**
 * @brief Factory method to create platform-specific capture engine
 * @return Unique pointer to capture engine instance
 */
std::unique_ptr<ICaptureEngine> createCaptureEngine();

} // namespace capture

// Convenience typedef
using CaptureEngine = ICaptureEngine;

} // namespace talos
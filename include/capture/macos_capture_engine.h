#pragma once

#include "capture/capture_engine.h"
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>

#ifdef __OBJC__
@class AVCaptureSession;
@class AVCaptureScreenInput;
@class AVCaptureVideoDataOutput;
@class TalosCaptureDelegate;
#else
typedef struct objc_object AVCaptureSession;
typedef struct objc_object AVCaptureScreenInput;
typedef struct objc_object AVCaptureVideoDataOutput;
typedef struct objc_object TalosCaptureDelegate;
#endif

namespace talos {
namespace capture {

/**
 * @brief macOS-specific capture engine using AVFoundation
 */
class MacOSCaptureEngine : public ICaptureEngine {
public:
    MacOSCaptureEngine();
    ~MacOSCaptureEngine() override;
    
    // ICaptureEngine interface
    bool initialize() override;
    void shutdown() override;
    bool startCapture() override;
    void stopCapture() override;
    bool isCapturing() const override { return m_isCapturing; }
    std::shared_ptr<Frame> getNextFrame(uint32_t timeoutMs = 100) override;
    CaptureStats getStats() const override;
    bool setMonitor(int monitorIndex) override;
    std::vector<MonitorInfo> getAvailableMonitors() const override;
    
    // Platform-specific methods
    bool selectDisplay(uint32_t displayId);
    static std::vector<MonitorInfo> getMonitors();
    
    // Called by delegate when new frame arrives
    void onFrameCaptured(void* sampleBuffer);
    
private:
    // AVFoundation objects
    AVCaptureSession* m_captureSession;
    AVCaptureScreenInput* m_screenInput;
    AVCaptureVideoDataOutput* m_videoOutput;
    TalosCaptureDelegate* m_delegate;
    
    // Frame queue
    mutable std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;
    std::queue<std::shared_ptr<Frame>> m_frameQueue;
    static constexpr size_t MAX_QUEUE_SIZE = 5;
    
    // State
    std::atomic<bool> m_initialized;
    std::atomic<bool> m_isCapturing;
    
    // Statistics
    mutable std::mutex m_statsMutex;
    CaptureStats m_stats;
    std::chrono::steady_clock::time_point m_startTime;
    
    // Display info
    uint32_t m_displayId;
    int m_displayWidth;
    int m_displayHeight;
    
    // Helper methods
    bool setupCaptureSession();
    void cleanupCaptureSession();
    bool configureScreenInput();
    bool configureVideoOutput();
    void updateStats();
};

} // namespace capture
} // namespace talos
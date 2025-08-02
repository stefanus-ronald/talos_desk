#pragma once

#ifdef PLATFORM_WINDOWS

#include "capture/capture_engine.h"
#include "capture/desktop_duplication_api.h"
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace talos {
namespace capture {

/**
 * @brief Windows implementation of the capture engine using Desktop Duplication API
 */
class WindowsCaptureEngine : public ICaptureEngine {
public:
    WindowsCaptureEngine();
    ~WindowsCaptureEngine() override;
    
    // ICaptureEngine interface
    bool initialize() override;
    void shutdown() override;
    bool startCapture() override;
    void stopCapture() override;
    bool isCapturing() const override;
    
    /**
     * @brief Get the next captured frame
     * @param timeoutMs Timeout in milliseconds
     * @return Captured frame or nullptr if no frame available
     */
    std::shared_ptr<Frame> getNextFrame(uint32_t timeoutMs = 100) override;
    
    /**
     * @brief Get capture statistics
     * @return Current capture statistics
     */
    CaptureStats getStats() const override;
    
    /**
     * @brief Set the monitor to capture
     * @param monitorIndex Index of the monitor (0 = primary)
     * @return true if successful
     */
    bool setMonitor(int monitorIndex) override;
    
    /**
     * @brief Get available monitors
     * @return List of available monitors
     */
    std::vector<MonitorInfo> getAvailableMonitors() const override;
    
private:
    // Capture thread function
    void captureThread();
    
    // Frame queue management
    void pushFrame(std::shared_ptr<Frame> frame);
    std::shared_ptr<Frame> popFrame(uint32_t timeoutMs);
    void clearFrameQueue();
    
    // Desktop Duplication API wrapper
    std::unique_ptr<DesktopDuplicationAPI> m_duplicationAPI;
    
    // Thread management
    std::thread m_captureThread;
    std::atomic<bool> m_capturing;
    std::atomic<bool> m_threadRunning;
    
    // Frame queue
    std::queue<std::shared_ptr<Frame>> m_frameQueue;
    mutable std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;
    size_t m_maxQueueSize;
    
    // Configuration
    int m_monitorIndex;
    
    // Statistics
    mutable std::mutex m_statsMutex;
    CaptureStats m_stats;
    std::chrono::steady_clock::time_point m_captureStartTime;
};

} // namespace capture
} // namespace talos

#endif // PLATFORM_WINDOWS
#ifdef PLATFORM_WINDOWS

#include "capture/windows_capture_engine.h"
#include "core/logger.h"
#include <chrono>
#include <algorithm>

namespace talos {
namespace capture {

WindowsCaptureEngine::WindowsCaptureEngine()
    : m_capturing(false)
    , m_threadRunning(false)
    , m_maxQueueSize(3)  // Keep max 3 frames in queue
    , m_monitorIndex(0)  // Primary monitor by default
{
    m_duplicationAPI = std::make_unique<DesktopDuplicationAPI>();
}

WindowsCaptureEngine::~WindowsCaptureEngine() {
    shutdown();
}

bool WindowsCaptureEngine::initialize() {
    Logger::instance().info("Initializing Windows capture engine");
    
    // Initialize Desktop Duplication API
    if (!m_duplicationAPI->initialize(m_monitorIndex)) {
        Logger::instance().error("Failed to initialize Desktop Duplication API: " + 
                                m_duplicationAPI->getLastError());
        return false;
    }
    
    // Get native resolution
    int width, height;
    if (m_duplicationAPI->getNativeResolution(width, height)) {
        Logger::instance().info("Capture resolution: " + std::to_string(width) + "x" + 
                               std::to_string(height));
    }
    
    // Reset statistics
    m_stats = CaptureStats();
    
    Logger::instance().info("Windows capture engine initialized successfully");
    return true;
}

void WindowsCaptureEngine::shutdown() {
    stopCapture();
    
    if (m_duplicationAPI) {
        m_duplicationAPI->shutdown();
    }
    
    Logger::instance().info("Windows capture engine shut down");
}

bool WindowsCaptureEngine::startCapture() {
    if (m_capturing) {
        Logger::instance().warn("Capture already running");
        return true;
    }
    
    if (!m_duplicationAPI->isInitialized()) {
        Logger::instance().error("Cannot start capture - engine not initialized");
        return false;
    }
    
    Logger::instance().info("Starting capture");
    
    // Clear any existing frames
    clearFrameQueue();
    
    // Reset statistics
    m_stats = CaptureStats();
    m_captureStartTime = std::chrono::steady_clock::now();
    
    // Start capture thread
    m_capturing = true;
    m_threadRunning = true;
    m_captureThread = std::thread(&WindowsCaptureEngine::captureThread, this);
    
    Logger::instance().info("Capture started successfully");
    return true;
}

void WindowsCaptureEngine::stopCapture() {
    if (!m_capturing) {
        return;
    }
    
    Logger::instance().info("Stopping capture");
    
    // Signal thread to stop
    m_capturing = false;
    
    // Wake up any waiting threads
    m_queueCondition.notify_all();
    
    // Wait for thread to finish
    if (m_captureThread.joinable()) {
        m_captureThread.join();
    }
    
    // Clear frame queue
    clearFrameQueue();
    
    Logger::instance().info("Capture stopped");
}

bool WindowsCaptureEngine::isCapturing() const {
    return m_capturing;
}

void WindowsCaptureEngine::captureThread() {
    Logger::instance().debug("Capture thread started");
    
    // Set thread priority for better performance
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    
    while (m_capturing) {
        try {
            // Capture frame with 16ms timeout (for ~60 FPS)
            auto frameInfo = m_duplicationAPI->captureFrame(16);
            
            if (frameInfo) {
                // Create Frame object
                auto frame = std::make_shared<Frame>();
                frame->width = frameInfo->width;
                frame->height = frameInfo->height;
                frame->stride = frameInfo->pitch;
                frame->pixelFormat = PixelFormat::BGRA8;
                frame->timestamp = frameInfo->timestamp;
                
                // Calculate data size
                size_t dataSize = frameInfo->height * frameInfo->pitch;
                frame->data.resize(dataSize);
                
                // Copy frame data
                std::memcpy(frame->data.data(), frameInfo->data, dataSize);
                
                // Update statistics
                {
                    std::lock_guard<std::mutex> lock(m_statsMutex);
                    m_stats.framesCapture++;
                    m_stats.bytesCapture += dataSize;
                    
                    // Calculate FPS
                    auto now = std::chrono::steady_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                        now - m_captureStartTime).count();
                    if (elapsed > 0) {
                        m_stats.averageFps = static_cast<float>(m_stats.framesCapture) / elapsed;
                    }
                }
                
                // Add to queue
                pushFrame(frame);
                
                // Release the frame in Desktop Duplication API
                // This happens automatically in destructor of frameInfo
            }
            
        } catch (const std::exception& e) {
            Logger::instance().error("Exception in capture thread: " + std::string(e.what()));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    m_threadRunning = false;
    Logger::instance().debug("Capture thread ended");
}

std::shared_ptr<Frame> WindowsCaptureEngine::getNextFrame(uint32_t timeoutMs) {
    return popFrame(timeoutMs);
}

void WindowsCaptureEngine::pushFrame(std::shared_ptr<Frame> frame) {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    
    // Drop oldest frame if queue is full
    if (m_frameQueue.size() >= m_maxQueueSize) {
        m_frameQueue.pop();
        
        std::lock_guard<std::mutex> statsLock(m_statsMutex);
        m_stats.framesDropped++;
    }
    
    m_frameQueue.push(frame);
    lock.unlock();
    
    // Notify waiting threads
    m_queueCondition.notify_one();
}

std::shared_ptr<Frame> WindowsCaptureEngine::popFrame(uint32_t timeoutMs) {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    
    // Wait for frame or timeout
    auto waitUntil = std::chrono::steady_clock::now() + 
                     std::chrono::milliseconds(timeoutMs);
    
    m_queueCondition.wait_until(lock, waitUntil, [this] {
        return !m_frameQueue.empty() || !m_capturing;
    });
    
    if (m_frameQueue.empty()) {
        return nullptr;
    }
    
    auto frame = m_frameQueue.front();
    m_frameQueue.pop();
    
    return frame;
}

void WindowsCaptureEngine::clearFrameQueue() {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    while (!m_frameQueue.empty()) {
        m_frameQueue.pop();
    }
}

CaptureStats WindowsCaptureEngine::getStats() const {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    return m_stats;
}

bool WindowsCaptureEngine::setMonitor(int monitorIndex) {
    if (m_capturing) {
        Logger::instance().error("Cannot change monitor while capturing");
        return false;
    }
    
    m_monitorIndex = monitorIndex;
    
    // Reinitialize if already initialized
    if (m_duplicationAPI->isInitialized()) {
        m_duplicationAPI->shutdown();
        return m_duplicationAPI->initialize(m_monitorIndex);
    }
    
    return true;
}

std::vector<MonitorInfo> WindowsCaptureEngine::getAvailableMonitors() const {
    return DesktopDuplicationAPI::getMonitors();
}

} // namespace capture
} // namespace talos

#endif // PLATFORM_WINDOWS
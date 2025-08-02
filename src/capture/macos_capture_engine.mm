#include "capture/macos_capture_engine.h"
#include "core/logger.h"

#import <AVFoundation/AVFoundation.h>
#import <AppKit/AppKit.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreVideo/CoreVideo.h>

#include <vector>
#include <chrono>

// Objective-C delegate to handle capture callbacks
@interface TalosCaptureDelegate : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
@property (nonatomic, assign) talos::capture::MacOSCaptureEngine* engine;
@end

@implementation TalosCaptureDelegate

- (void)captureOutput:(AVCaptureOutput *)output 
    didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer 
    fromConnection:(AVCaptureConnection *)connection {
    if (self.engine) {
        self.engine->onFrameCaptured(sampleBuffer);
    }
}

- (void)captureOutput:(AVCaptureOutput *)output 
    didDropSampleBuffer:(CMSampleBufferRef)sampleBuffer 
    fromConnection:(AVCaptureConnection *)connection {
    // Log dropped frames
    talos::Logger::getInstance().log(talos::LogLevel::Warning, "Dropped frame in capture");
}

@end

namespace talos {
namespace capture {

MacOSCaptureEngine::MacOSCaptureEngine()
    : m_captureSession(nil)
    , m_screenInput(nil)
    , m_videoOutput(nil)
    , m_delegate(nil)
    , m_initialized(false)
    , m_isCapturing(false)
    , m_displayId(CGMainDisplayID())
    , m_displayWidth(0)
    , m_displayHeight(0) {
    m_startTime = std::chrono::steady_clock::now();
}

MacOSCaptureEngine::~MacOSCaptureEngine() {
    shutdown();
}

bool MacOSCaptureEngine::initialize() {
    if (m_initialized) {
        return true;
    }
    
    @autoreleasepool {
        // Create capture session
        m_captureSession = [[AVCaptureSession alloc] init];
        if (!m_captureSession) {
            Logger::getInstance().log(LogLevel::Error, "Failed to create AVCaptureSession");
            return false;
        }
        
        // Create delegate
        m_delegate = [[TalosCaptureDelegate alloc] init];
        m_delegate.engine = this;
        
        // Setup capture session
        if (!setupCaptureSession()) {
            cleanupCaptureSession();
            return false;
        }
        
        m_initialized = true;
        Logger::getInstance().log(LogLevel::Info, "macOS capture engine initialized");
        return true;
    }
}

void MacOSCaptureEngine::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    stopCapture();
    
    @autoreleasepool {
        cleanupCaptureSession();
    }
    
    m_initialized = false;
    Logger::getInstance().log(LogLevel::Info, "macOS capture engine shut down");
}

bool MacOSCaptureEngine::startCapture() {
    if (!m_initialized || m_isCapturing) {
        return false;
    }
    
    @autoreleasepool {
        [m_captureSession startRunning];
        
        if (![m_captureSession isRunning]) {
            Logger::getInstance().log(LogLevel::Error, "Failed to start capture session");
            return false;
        }
        
        m_isCapturing = true;
        m_startTime = std::chrono::steady_clock::now();
        
        Logger::getInstance().log(LogLevel::Info, "Started capturing");
        return true;
    }
}

void MacOSCaptureEngine::stopCapture() {
    if (!m_isCapturing) {
        return;
    }
    
    @autoreleasepool {
        [m_captureSession stopRunning];
        m_isCapturing = false;
        
        // Clear frame queue
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            while (!m_frameQueue.empty()) {
                m_frameQueue.pop();
            }
        }
        
        Logger::getInstance().log(LogLevel::Info, "Stopped capturing");
    }
}

std::shared_ptr<Frame> MacOSCaptureEngine::getNextFrame(uint32_t timeoutMs) {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    
    auto timeout = std::chrono::milliseconds(timeoutMs);
    bool hasFrame = m_queueCondition.wait_for(lock, timeout, [this] {
        return !m_frameQueue.empty();
    });
    
    if (!hasFrame || m_frameQueue.empty()) {
        return nullptr;
    }
    
    auto frame = m_frameQueue.front();
    m_frameQueue.pop();
    
    return frame;
}

CaptureStats MacOSCaptureEngine::getStats() const {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    
    // Return copy of stats with updated FPS
    CaptureStats stats = m_stats;
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime).count();
    if (duration > 0) {
        stats.currentFps = static_cast<float>(stats.framesCapture) / duration;
    }
    
    return stats;
}

bool MacOSCaptureEngine::selectDisplay(uint32_t displayId) {
    if (m_isCapturing) {
        Logger::getInstance().log(LogLevel::Error, "Cannot change display while capturing");
        return false;
    }
    
    m_displayId = displayId;
    
    // If already initialized, reconfigure
    if (m_initialized) {
        @autoreleasepool {
            // Remove old input
            if (m_screenInput) {
                [m_captureSession removeInput:m_screenInput];
                m_screenInput = nil;
            }
            
            // Configure new input
            return configureScreenInput();
        }
    }
    
    return true;
}

std::vector<MonitorInfo> MacOSCaptureEngine::getMonitors() {
    std::vector<MonitorInfo> monitors;
    
    @autoreleasepool {
        NSArray *screens = [NSScreen screens];
        
        for (NSUInteger i = 0; i < [screens count]; i++) {
            NSScreen *screen = screens[i];
            NSDictionary *deviceDescription = [screen deviceDescription];
            NSNumber *screenNumber = deviceDescription[@"NSScreenNumber"];
            
            MonitorInfo info;
            info.id = [screenNumber unsignedIntValue];
            info.width = static_cast<int>(screen.frame.size.width * screen.backingScaleFactor);
            info.height = static_cast<int>(screen.frame.size.height * screen.backingScaleFactor);
            info.x = static_cast<int>(screen.frame.origin.x);
            info.y = static_cast<int>(screen.frame.origin.y);
            info.refreshRate = 60.0f; // macOS doesn't easily expose refresh rate
            info.isPrimary = (i == 0);
            info.name = [[screen localizedName] UTF8String];
            
            monitors.push_back(info);
        }
    }
    
    return monitors;
}

bool MacOSCaptureEngine::setMonitor(int monitorIndex) {
    auto monitors = getMonitors();
    if (monitorIndex < 0 || monitorIndex >= static_cast<int>(monitors.size())) {
        Logger::getInstance().log(LogLevel::Error, "Invalid monitor index: " + std::to_string(monitorIndex));
        return false;
    }
    
    return selectDisplay(monitors[monitorIndex].id);
}

std::vector<MonitorInfo> MacOSCaptureEngine::getAvailableMonitors() const {
    return getMonitors();
}

void MacOSCaptureEngine::onFrameCaptured(void* sampleBuffer) {
    @autoreleasepool {
        CMSampleBufferRef sample = (CMSampleBufferRef)sampleBuffer;
        
        // Get image buffer
        CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sample);
        if (!imageBuffer) {
            Logger::getInstance().log(LogLevel::Error, "Failed to get image buffer from sample");
            return;
        }
        
        // Lock pixel buffer
        CVPixelBufferLockBaseAddress(imageBuffer, kCVPixelBufferLock_ReadOnly);
        
        // Get buffer info
        size_t width = CVPixelBufferGetWidth(imageBuffer);
        size_t height = CVPixelBufferGetHeight(imageBuffer);
        size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
        void* baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
        
        // Get pixel format
        OSType pixelFormat = CVPixelBufferGetPixelFormatType(imageBuffer);
        PixelFormat format = PixelFormat::BGRA8;
        
        // Convert pixel format
        switch (pixelFormat) {
            case kCVPixelFormatType_32BGRA:
                format = PixelFormat::BGRA8;
                break;
            case kCVPixelFormatType_32RGBA:
                format = PixelFormat::RGBA8;
                break;
            case kCVPixelFormatType_32ARGB:
                format = PixelFormat::BGRA8;  // Treat ARGB as BGRA for now
                break;
            default:
                Logger::getInstance().log(LogLevel::Warning, 
                    "Unsupported pixel format: " + std::to_string(pixelFormat));
                CVPixelBufferUnlockBaseAddress(imageBuffer, kCVPixelBufferLock_ReadOnly);
                return;
        }
        
        // Create frame
        auto frame = std::make_shared<Frame>();
        frame->width = static_cast<int>(width);
        frame->height = static_cast<int>(height);
        frame->stride = static_cast<int>(bytesPerRow);
        frame->pixelFormat = format;
        // Convert timestamp to microseconds
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
        frame->timestamp = duration.count();
        
        // Copy pixel data
        size_t dataSize = height * bytesPerRow;
        frame->data.resize(dataSize);
        std::memcpy(frame->data.data(), baseAddress, dataSize);
        
        // Unlock pixel buffer
        CVPixelBufferUnlockBaseAddress(imageBuffer, kCVPixelBufferLock_ReadOnly);
        
        // Add to queue
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            
            // Drop oldest frame if queue is full
            if (m_frameQueue.size() >= MAX_QUEUE_SIZE) {
                m_frameQueue.pop();
                m_stats.framesDropped++;
            }
            
            m_frameQueue.push(frame);
        }
        m_queueCondition.notify_one();
        
        // Update stats
        updateStats();
    }
}

bool MacOSCaptureEngine::setupCaptureSession() {
    @autoreleasepool {
        // Configure session preset for native resolution
        [m_captureSession setSessionPreset:AVCaptureSessionPresetHigh];
        
        // Configure screen input
        if (!configureScreenInput()) {
            return false;
        }
        
        // Configure video output
        if (!configureVideoOutput()) {
            return false;
        }
        
        return true;
    }
}

void MacOSCaptureEngine::cleanupCaptureSession() {
    @autoreleasepool {
        if (m_captureSession) {
            if (m_screenInput) {
                [m_captureSession removeInput:m_screenInput];
                m_screenInput = nil;
            }
            
            if (m_videoOutput) {
                [m_captureSession removeOutput:m_videoOutput];
                m_videoOutput = nil;
            }
            
            m_captureSession = nil;
        }
        
        if (m_delegate) {
            m_delegate.engine = nullptr;
            m_delegate = nil;
        }
    }
}

bool MacOSCaptureEngine::configureScreenInput() {
    @autoreleasepool {
        // Create screen input
        m_screenInput = [[AVCaptureScreenInput alloc] initWithDisplayID:m_displayId];
        if (!m_screenInput) {
            Logger::getInstance().log(LogLevel::Error, "Failed to create screen input");
            return false;
        }
        
        // Configure screen input properties
        [m_screenInput setCapturesMouseClicks:NO];
        [m_screenInput setCapturesCursor:YES];
        
        // Set minimum frame duration (max fps)
        CMTime frameDuration = CMTimeMake(1, 30); // 30 fps
        [m_screenInput setMinFrameDuration:frameDuration];
        
        // Add input to session
        if ([m_captureSession canAddInput:m_screenInput]) {
            [m_captureSession addInput:m_screenInput];
        } else {
            Logger::getInstance().log(LogLevel::Error, "Cannot add screen input to session");
            return false;
        }
        
        // Get display dimensions
        CGDirectDisplayID displayID = (CGDirectDisplayID)m_displayId;
        m_displayWidth = static_cast<int>(CGDisplayPixelsWide(displayID));
        m_displayHeight = static_cast<int>(CGDisplayPixelsHigh(displayID));
        
        Logger::getInstance().log(LogLevel::Info, 
            "Configured screen input for display " + std::to_string(m_displayId) +
            " (" + std::to_string(m_displayWidth) + "x" + std::to_string(m_displayHeight) + ")");
        
        return true;
    }
}

bool MacOSCaptureEngine::configureVideoOutput() {
    @autoreleasepool {
        // Create video data output
        m_videoOutput = [[AVCaptureVideoDataOutput alloc] init];
        if (!m_videoOutput) {
            Logger::getInstance().log(LogLevel::Error, "Failed to create video output");
            return false;
        }
        
        // Configure pixel format
        NSDictionary *videoSettings = @{
            (NSString *)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA)
        };
        [m_videoOutput setVideoSettings:videoSettings];
        
        // Configure output queue
        dispatch_queue_t videoQueue = dispatch_queue_create("com.talos.capture", DISPATCH_QUEUE_SERIAL);
        [m_videoOutput setSampleBufferDelegate:m_delegate queue:videoQueue];
        
        // Set to drop frames if processing is too slow
        [m_videoOutput setAlwaysDiscardsLateVideoFrames:YES];
        
        // Add output to session
        if ([m_captureSession canAddOutput:m_videoOutput]) {
            [m_captureSession addOutput:m_videoOutput];
        } else {
            Logger::getInstance().log(LogLevel::Error, "Cannot add video output to session");
            return false;
        }
        
        return true;
    }
}

void MacOSCaptureEngine::updateStats() {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    m_stats.framesCapture++;
}

} // namespace capture
} // namespace talos
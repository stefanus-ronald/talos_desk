#pragma once

#ifdef PLATFORM_WINDOWS

#include <memory>
#include <vector>
#include <string>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>

namespace talos {
namespace capture {

// Forward declarations
struct FrameInfo {
    int width;
    int height;
    int pitch;
    void* data;
    uint64_t timestamp;
};

struct MonitorInfo {
    std::wstring deviceName;
    int width;
    int height;
    int left;
    int top;
    bool isPrimary;
};

/**
 * @brief Windows Desktop Duplication API wrapper
 * 
 * This class provides a high-level interface to the Windows Desktop Duplication API,
 * which allows efficient screen capture on Windows 8 and later.
 */
class DesktopDuplicationAPI {
public:
    DesktopDuplicationAPI();
    ~DesktopDuplicationAPI();
    
    // Prevent copying
    DesktopDuplicationAPI(const DesktopDuplicationAPI&) = delete;
    DesktopDuplicationAPI& operator=(const DesktopDuplicationAPI&) = delete;
    
    /**
     * @brief Initialize the Desktop Duplication API
     * @param outputIndex Index of the display adapter output to capture
     * @return true if successful, false otherwise
     */
    bool initialize(int outputIndex = 0);
    
    /**
     * @brief Shutdown and release all resources
     */
    void shutdown();
    
    /**
     * @brief Check if the API is initialized and ready
     * @return true if ready, false otherwise
     */
    bool isInitialized() const { return m_initialized; }
    
    /**
     * @brief Capture the next frame
     * @param timeoutMs Timeout in milliseconds to wait for a new frame
     * @return FrameInfo structure with captured frame data, or nullptr if no new frame
     */
    std::unique_ptr<FrameInfo> captureFrame(uint32_t timeoutMs = 100);
    
    /**
     * @brief Get list of available monitors
     * @return Vector of MonitorInfo structures
     */
    static std::vector<MonitorInfo> getMonitors();
    
    /**
     * @brief Get the native resolution of the current output
     * @param width Output parameter for width
     * @param height Output parameter for height
     * @return true if successful, false otherwise
     */
    bool getNativeResolution(int& width, int& height) const;
    
    /**
     * @brief Get last error message
     * @return Error message string
     */
    const std::string& getLastError() const { return m_lastError; }
    
private:
    // Helper methods
    bool initializeDirect3D();
    bool initializeDuplication(int outputIndex);
    void releaseFrame();
    void setError(const std::string& error);
    
    // D3D11 resources
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGIOutputDuplication> m_duplication;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_stagingTexture;
    
    // Frame management
    Microsoft::WRL::ComPtr<IDXGIResource> m_desktopResource;
    DXGI_OUTPUT_DESC m_outputDesc;
    
    // State
    bool m_initialized;
    bool m_frameAcquired;
    int m_outputWidth;
    int m_outputHeight;
    std::string m_lastError;
    
    // Performance counters
    uint64_t m_frameCount;
    uint64_t m_duplicateFrameCount;
};

} // namespace capture
} // namespace talos

#endif // PLATFORM_WINDOWS
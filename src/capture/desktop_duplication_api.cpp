#ifdef PLATFORM_WINDOWS

#include "capture/desktop_duplication_api.h"
#include "core/logger.h"
#include <windows.h>
#include <comdef.h>
#include <chrono>

namespace talos {
namespace capture {

DesktopDuplicationAPI::DesktopDuplicationAPI()
    : m_initialized(false)
    , m_frameAcquired(false)
    , m_outputWidth(0)
    , m_outputHeight(0)
    , m_frameCount(0)
    , m_duplicateFrameCount(0) {
}

DesktopDuplicationAPI::~DesktopDuplicationAPI() {
    shutdown();
}

bool DesktopDuplicationAPI::initialize(int outputIndex) {
    if (m_initialized) {
        return true;
    }
    
    Logger::instance().info("Initializing Desktop Duplication API for output " + std::to_string(outputIndex));
    
    // Initialize Direct3D
    if (!initializeDirect3D()) {
        return false;
    }
    
    // Initialize duplication for specified output
    if (!initializeDuplication(outputIndex)) {
        return false;
    }
    
    m_initialized = true;
    Logger::instance().info("Desktop Duplication API initialized successfully");
    Logger::instance().info("Native resolution: " + std::to_string(m_outputWidth) + "x" + std::to_string(m_outputHeight));
    
    return true;
}

void DesktopDuplicationAPI::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    Logger::instance().info("Shutting down Desktop Duplication API");
    
    releaseFrame();
    
    m_stagingTexture.Reset();
    m_duplication.Reset();
    m_context.Reset();
    m_device.Reset();
    
    m_initialized = false;
}

bool DesktopDuplicationAPI::initializeDirect3D() {
    HRESULT hr;
    
    // Driver types supported
    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);
    
    // Feature levels supported
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);
    
    D3D_FEATURE_LEVEL featureLevel;
    
    // Create device
    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex) {
        hr = D3D11CreateDevice(
            nullptr,
            driverTypes[driverTypeIndex],
            nullptr,
            0,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            &m_device,
            &featureLevel,
            &m_context
        );
        
        if (SUCCEEDED(hr)) {
            break;
        }
    }
    
    if (FAILED(hr)) {
        setError("Failed to create D3D11 device");
        return false;
    }
    
    Logger::instance().debug("D3D11 device created successfully");
    return true;
}

bool DesktopDuplicationAPI::initializeDuplication(int outputIndex) {
    HRESULT hr;
    
    // Get DXGI device
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    hr = m_device.As(&dxgiDevice);
    if (FAILED(hr)) {
        setError("Failed to get DXGI device");
        return false;
    }
    
    // Get adapter
    Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
    hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), &dxgiAdapter);
    if (FAILED(hr)) {
        setError("Failed to get DXGI adapter");
        return false;
    }
    
    // Get output
    Microsoft::WRL::ComPtr<IDXGIOutput> dxgiOutput;
    hr = dxgiAdapter->EnumOutputs(outputIndex, &dxgiOutput);
    if (FAILED(hr)) {
        setError("Failed to get DXGI output " + std::to_string(outputIndex));
        return false;
    }
    
    // Get output description
    hr = dxgiOutput->GetDesc(&m_outputDesc);
    if (FAILED(hr)) {
        setError("Failed to get output description");
        return false;
    }
    
    m_outputWidth = m_outputDesc.DesktopCoordinates.right - m_outputDesc.DesktopCoordinates.left;
    m_outputHeight = m_outputDesc.DesktopCoordinates.bottom - m_outputDesc.DesktopCoordinates.top;
    
    // QI for Output1
    Microsoft::WRL::ComPtr<IDXGIOutput1> dxgiOutput1;
    hr = dxgiOutput.As(&dxgiOutput1);
    if (FAILED(hr)) {
        setError("Failed to get IDXGIOutput1 interface");
        return false;
    }
    
    // Create desktop duplication
    hr = dxgiOutput1->DuplicateOutput(m_device.Get(), &m_duplication);
    if (FAILED(hr)) {
        if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE) {
            setError("Desktop Duplication not available (another application may be using it)");
        } else if (hr == E_ACCESSDENIED) {
            setError("Access denied to Desktop Duplication API");
        } else {
            _com_error err(hr);
            setError("Failed to duplicate output: " + std::string(err.ErrorMessage()));
        }
        return false;
    }
    
    // Create staging texture for CPU access
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width = m_outputWidth;
    textureDesc.Height = m_outputHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_STAGING;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    
    hr = m_device->CreateTexture2D(&textureDesc, nullptr, &m_stagingTexture);
    if (FAILED(hr)) {
        setError("Failed to create staging texture");
        return false;
    }
    
    return true;
}

std::unique_ptr<FrameInfo> DesktopDuplicationAPI::captureFrame(uint32_t timeoutMs) {
    if (!m_initialized) {
        setError("Desktop Duplication API not initialized");
        return nullptr;
    }
    
    HRESULT hr;
    
    // Release previous frame if any
    releaseFrame();
    
    // Get new frame
    DXGI_OUTDUPL_FRAME_INFO frameInfo;
    hr = m_duplication->AcquireNextFrame(timeoutMs, &frameInfo, &m_desktopResource);
    
    if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
        // No new frame available
        return nullptr;
    }
    
    if (FAILED(hr)) {
        if (hr == DXGI_ERROR_ACCESS_LOST) {
            setError("Desktop Duplication access lost - reinitialize required");
            m_initialized = false;
        } else {
            _com_error err(hr);
            setError("Failed to acquire frame: " + std::string(err.ErrorMessage()));
        }
        return nullptr;
    }
    
    m_frameAcquired = true;
    m_frameCount++;
    
    // Check if frame has updates
    if (frameInfo.AccumulatedFrames == 0) {
        m_duplicateFrameCount++;
        releaseFrame();
        return nullptr;
    }
    
    // Get texture from resource
    Microsoft::WRL::ComPtr<ID3D11Texture2D> acquiredTexture;
    hr = m_desktopResource.As(&acquiredTexture);
    if (FAILED(hr)) {
        setError("Failed to get texture from desktop resource");
        releaseFrame();
        return nullptr;
    }
    
    // Copy to staging texture
    m_context->CopyResource(m_stagingTexture.Get(), acquiredTexture.Get());
    
    // Map staging texture
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    hr = m_context->Map(m_stagingTexture.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
    if (FAILED(hr)) {
        setError("Failed to map staging texture");
        releaseFrame();
        return nullptr;
    }
    
    // Create frame info
    auto frame = std::make_unique<FrameInfo>();
    frame->width = m_outputWidth;
    frame->height = m_outputHeight;
    frame->pitch = mappedResource.RowPitch;
    frame->data = mappedResource.pData;
    frame->timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
    
    // Note: We don't unmap here - caller must call releaseFrame() when done with data
    
    return frame;
}

void DesktopDuplicationAPI::releaseFrame() {
    if (m_frameAcquired) {
        // Unmap staging texture if mapped
        m_context->Unmap(m_stagingTexture.Get(), 0);
        
        // Release desktop resource
        m_desktopResource.Reset();
        
        // Release frame
        m_duplication->ReleaseFrame();
        
        m_frameAcquired = false;
    }
}

bool DesktopDuplicationAPI::getNativeResolution(int& width, int& height) const {
    if (!m_initialized) {
        return false;
    }
    
    width = m_outputWidth;
    height = m_outputHeight;
    return true;
}

std::vector<MonitorInfo> DesktopDuplicationAPI::getMonitors() {
    std::vector<MonitorInfo> monitors;
    
    // Enum display monitors
    auto enumProc = [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) -> BOOL {
        auto* monitors = reinterpret_cast<std::vector<MonitorInfo>*>(dwData);
        
        MONITORINFOEXW monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFOEXW);
        
        if (GetMonitorInfoW(hMonitor, &monitorInfo)) {
            MonitorInfo info;
            info.deviceName = monitorInfo.szDevice;
            info.width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
            info.height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
            info.left = monitorInfo.rcMonitor.left;
            info.top = monitorInfo.rcMonitor.top;
            info.isPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
            
            monitors->push_back(info);
        }
        
        return TRUE;
    };
    
    EnumDisplayMonitors(nullptr, nullptr, enumProc, reinterpret_cast<LPARAM>(&monitors));
    
    return monitors;
}

void DesktopDuplicationAPI::setError(const std::string& error) {
    m_lastError = error;
    Logger::instance().error("Desktop Duplication API: " + error);
}

} // namespace capture
} // namespace talos

#endif // PLATFORM_WINDOWS
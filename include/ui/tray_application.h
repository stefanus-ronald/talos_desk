#pragma once

namespace talos {

/**
 * @brief System tray application interface
 */
class TrayApplication {
public:
    TrayApplication() = default;
    virtual ~TrayApplication() = default;
    
    /**
     * @brief Initialize the tray application
     * @return true if successful, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Run the tray application
     */
    virtual void run() = 0;
    
    /**
     * @brief Stop the tray application
     */
    virtual void stop() = 0;
};

} // namespace talos
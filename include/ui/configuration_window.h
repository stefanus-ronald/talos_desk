#pragma once

namespace talos {

/**
 * @brief Configuration window interface
 */
class ConfigurationWindow {
public:
    ConfigurationWindow() = default;
    virtual ~ConfigurationWindow() = default;
    
    /**
     * @brief Initialize the configuration window
     * @return true if successful, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Show the configuration window
     */
    virtual void show() = 0;
    
    /**
     * @brief Hide the configuration window
     */
    virtual void hide() = 0;
    
    /**
     * @brief Render the window contents
     */
    virtual void render() = 0;
};

} // namespace talos
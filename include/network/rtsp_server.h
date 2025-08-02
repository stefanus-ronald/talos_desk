#pragma once

#include <string>

namespace talos {

/**
 * @brief RTSP server interface
 */
class RTSPServer {
public:
    RTSPServer() = default;
    virtual ~RTSPServer() = default;
    
    /**
     * @brief Initialize the RTSP server
     * @param port RTSP port to listen on
     * @return true if successful, false otherwise
     */
    virtual bool initialize(int port) = 0;
    
    /**
     * @brief Start the RTSP server
     * @return true if successful, false otherwise
     */
    virtual bool start() = 0;
    
    /**
     * @brief Stop the RTSP server
     */
    virtual void stop() = 0;
    
    /**
     * @brief Get the number of connected clients
     * @return Number of active client connections
     */
    virtual int getClientCount() const = 0;
};

} // namespace talos
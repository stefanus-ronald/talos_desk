// Talos Desk - Screen-to-RTSP ONVIF Bridge
// Main entry point

#include <iostream>
#include <memory>
#include <csignal>
#include <cstdlib>

#include "core/application.h"
#include "core/logger.h"
#include "core/configuration.h"

// Global application instance for signal handling
std::unique_ptr<talos::Application> g_application;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        talos::Logger::instance().info("Received shutdown signal, stopping...");
        if (g_application) {
            g_application->stop();
        }
    }
}

// Print usage information
void printUsage(const char* programName) {
    std::cout << "Talos Desk - Screen-to-RTSP ONVIF Bridge v1.0.0\n";
    std::cout << "\nUsage: " << programName << " [options]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -h, --help          Show this help message\n";
    std::cout << "  -c, --config FILE   Specify configuration file (default: config.json)\n";
    std::cout << "  -d, --daemon        Run in daemon/service mode\n";
    std::cout << "  -v, --verbose       Enable verbose logging\n";
    std::cout << "  --no-gui            Disable GUI (headless mode)\n";
    std::cout << "  --port PORT         RTSP server port (default: 554)\n";
    std::cout << "  --fps FPS           Frame rate (default: 30)\n";
    std::cout << "  --quality QUALITY   Quality preset: high, balanced, low (default: balanced)\n";
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        std::string configFile = "config.json";
        bool daemonMode = false;
        bool verboseLogging = false;
        bool guiEnabled = true;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "-h" || arg == "--help") {
                printUsage(argv[0]);
                return 0;
            }
            else if (arg == "-c" || arg == "--config") {
                if (i + 1 < argc) {
                    configFile = argv[++i];
                } else {
                    std::cerr << "Error: --config requires a filename\n";
                    return 1;
                }
            }
            else if (arg == "-d" || arg == "--daemon") {
                daemonMode = true;
            }
            else if (arg == "-v" || arg == "--verbose") {
                verboseLogging = true;
            }
            else if (arg == "--no-gui") {
                guiEnabled = false;
            }
            else if (arg == "--port") {
                if (i + 1 < argc) {
                    // TODO: Parse and validate port
                    ++i;
                }
            }
            else if (arg == "--fps") {
                if (i + 1 < argc) {
                    // TODO: Parse and validate FPS
                    ++i;
                }
            }
            else if (arg == "--quality") {
                if (i + 1 < argc) {
                    // TODO: Parse and validate quality
                    ++i;
                }
            }
            else {
                std::cerr << "Error: Unknown option: " << arg << "\n";
                printUsage(argv[0]);
                return 1;
            }
        }
        
        // Initialize logger
        talos::Logger::instance().setLogLevel(
            verboseLogging ? talos::LogLevel::Debug : talos::LogLevel::Info
        );
        
        talos::Logger::instance().info("Talos Desk starting...");
        talos::Logger::instance().info("Version: 1.0.0");
        #ifdef PLATFORM_WINDOWS
        talos::Logger::instance().info("Platform: Windows");
        #elif defined(PLATFORM_MACOS)
        talos::Logger::instance().info("Platform: macOS");
        #elif defined(PLATFORM_LINUX)
        talos::Logger::instance().info("Platform: Linux");
        #else
        talos::Logger::instance().info("Platform: Unknown");
        #endif
        
        // Load configuration
        auto config = std::make_shared<talos::Configuration>();
        if (!config->loadFromFile(configFile)) {
            talos::Logger::instance().warn("Failed to load config file, using defaults");
        }
        
        // Set up signal handlers
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
#ifdef PLATFORM_WINDOWS
        // Windows-specific signal handling
        // TODO: Implement Windows console control handler
#endif
        
        // Create and initialize application
        g_application = std::make_unique<talos::Application>(config);
        g_application->setGuiEnabled(guiEnabled);
        g_application->setDaemonMode(daemonMode);
        
        if (!g_application->initialize()) {
            talos::Logger::instance().error("Failed to initialize application");
            return 1;
        }
        
        // Run the application
        talos::Logger::instance().info("Application initialized successfully");
        int result = g_application->run();
        
        // Cleanup
        g_application.reset();
        
        talos::Logger::instance().info("Talos Desk stopped");
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Fatal error: Unknown exception" << std::endl;
        return 1;
    }
}
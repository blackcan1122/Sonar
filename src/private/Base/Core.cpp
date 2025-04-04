#include "Base/Core.h"

void init_logger() {
    namespace fs = std::filesystem;
    const std::string log_path = "logs/Sonar.log";

    // Create logs directory if it doesn't exist
    fs::create_directories("logs");

    // Rotate previous log (if any)
    if (fs::exists(log_path)) {
        const auto now = std::chrono::system_clock::now();
        const std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);

        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &tm);
        const std::string new_name = fmt::format("logs/Sonar_{}.log", buffer);
        fs::rename(log_path, new_name);
    }

    // Initialize the global logger
    g_logger = spdlog::basic_logger_mt("Basic", log_path, true);
    g_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    g_logger->set_level(spdlog::level::info);
}
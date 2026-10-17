#include "Base/Logging.h"
#include "Base/LoggingBackend.hpp"
std::shared_ptr<spdlog::logger> GeneralLogger;
void LogInfo(const char* category, const std::string& message) {
    if (GeneralLogger) GeneralLogger->log(spdlog::level::info, "{} {}", category, message);
}
void LogInfo(const std::string& message) { LogInfo(l_DEFAULT, message); }
void LogWarn(const char* category, const std::string& message) {
    if (GeneralLogger) GeneralLogger->log(spdlog::level::warn, "{} {}", category, message);
}
void LogWarn(const std::string& message) { LogWarn(l_DEFAULT, message); }
void LogError(const char* category, const std::string& message) {
    if (GeneralLogger) GeneralLogger->log(spdlog::level::err, "{} {}", category, message);
}
void LogError(const std::string& message) { LogError(l_DEFAULT, message); }
#include "Base/Core.h"

void InitLogger() 
{
    namespace fs = std::filesystem;
    const std::string Log = "logs/Sonar.log";
    const std::string LogPath = "logs";

    // Create logs directory if it doesn't exist
    fs::create_directories(LogPath);

    if (fs::exists(Log)) 
    {
        const auto Now = std::chrono::system_clock::now();
        const std::time_t Time = std::chrono::system_clock::to_time_t(Now);
        std::tm CurrentTime = *std::localtime(&Time);

        char NameBuffer[64];
        std::strftime(NameBuffer, sizeof(NameBuffer), "%Y%m%d_%H%M%S", &CurrentTime);
        const std::string NewName = fmt::format("logs/Sonar_{}.log", NameBuffer);
        fs::rename(Log, NewName);
    }

    // Initialize the global logger
    g_logger = spdlog::basic_logger_mt("Basic", Log, true);
    g_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    g_logger->set_level(spdlog::level::info);

    std::vector<std::pair<fs::path, fs::file_time_type>> FileEntries;

    // Collect files with timestamps
    for (const auto& entry : fs::directory_iterator(LogPath))
    {
        if (entry.is_regular_file()) 
{
            FileEntries.emplace_back(entry.path(), fs::last_write_time(entry));
        }
    }

    // Sort files from oldest to newest
    std::sort(FileEntries.begin(), FileEntries.end(),
        [](const auto& a, const auto& b) 
        {
            return a.second < b.second; // We compare timeStamp against each other
        });

    // Delete oldest files if we have more than 5 log files
    if (FileEntries.size() > 5) 
    {

        const size_t NumFilesToDelete = FileEntries.size() - 5;

        for (size_t i = 0; i < NumFilesToDelete; ++i) 
        {
            try 
            {
                fs::remove(FileEntries[i].first);  // Delete oldest files first
                std::cout << "Deleted: " << FileEntries[i].first << std::endl;
                LOG_INFO("Deleted: {}", FileEntries[i].first.string());
            }
            catch (const fs::filesystem_error& e) 
            {
                LOG_INFO("Failed to Delete: {}, Error Message: {}", FileEntries[i].first.string(), e.what());
            }
        }
    }


}
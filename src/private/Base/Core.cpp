#include "Base/Core.h"

void InitLogger() 
{
    const std::string Log = "logs/Sonar.log";
    const std::string LogPath = "logs";

    std::filesystem::create_directories(LogPath);

    if (std::filesystem::exists(Log)) 
    {
        const auto Now = std::chrono::system_clock::now();
        const std::time_t Time = std::chrono::system_clock::to_time_t(Now);
        std::tm CurrentTime = *std::localtime(&Time);

        char NameBuffer[64];
        std::strftime(NameBuffer, sizeof(NameBuffer), "%Y%m%d_%H%M%S", &CurrentTime);
        const std::string NewName = fmt::format("logs/Sonar_{}.log", NameBuffer);
        std::filesystem::rename(Log, NewName);
    }

    // Logger Initialization
    GeneralLogger = spdlog::basic_logger_mt("Basic", Log, true);
    GeneralLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    GeneralLogger->set_level(spdlog::level::info);

    std::vector<std::pair<std::filesystem::path, std::filesystem::file_time_type>> FileEntries;

    // We Collect timestamps and files in Directory
    for (const auto& entry : std::filesystem::directory_iterator(LogPath))
    {
        if (entry.is_regular_file()) 
{
            FileEntries.emplace_back(entry.path(), std::filesystem::last_write_time(entry));
        }
    }

    // Then we delete all but the 5 most recent ones
    std::sort(FileEntries.begin(), FileEntries.end(),
        [](const auto& a, const auto& b) 
        {
            return a.second < b.second;
        });


    if (FileEntries.size() > 5) 
    {

        const size_t NumFilesToDelete = FileEntries.size() - 5;

        for (size_t i = 0; i < NumFilesToDelete; ++i) 
        {
            try 
            {
                std::filesystem::remove(FileEntries[i].first);
                LOG_INFO(l_HOUSE_KEEPING, TEXT("Deleted: '{}'", FileEntries[i].first.string()));
            }
            catch (const std::filesystem::filesystem_error& e) 
            {
                LOG_ERROR(l_HOUSE_KEEPING, TEXT("Failed to Delete: '{}', Error Message: '{}'", 
                    FileEntries[i].first.string(), 
                    e.what()));
            }
        }
    }


}
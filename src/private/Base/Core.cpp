#include "Base/Core.h"
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

class UppercaseLevelFormatter : public spdlog::custom_flag_formatter
{
public:
    void format(const spdlog::details::log_msg& msg, const std::tm&, spdlog::memory_buf_t& dest) override
    {
        std::string level = spdlog::level::to_string_view(msg.level).data();
        std::transform(level.begin(), level.end(), level.begin(), ::toupper);
        
        // Pad to 8 characters
        while (level.size() < 8)
        {
            level.push_back(' ');
        }
        
        dest.append(level.data(), level.data() + level.size());
    }

    std::unique_ptr<custom_flag_formatter> clone() const override
    {
        return spdlog::details::make_unique<UppercaseLevelFormatter>();
    }
};

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

    // Create multiple sinks: console (with color) + file (without color)
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(Log, true);

    // Set patterns
    auto formatter_console = std::make_unique<spdlog::pattern_formatter>();
    formatter_console->add_flag<UppercaseLevelFormatter>('*');
    formatter_console->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%*%$] %v");
    
    auto formatter_file = std::make_unique<spdlog::pattern_formatter>();
    formatter_file->add_flag<UppercaseLevelFormatter>('*');
    formatter_file->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%*] %v");  // No color codes for file

    console_sink->set_formatter(std::move(formatter_console));
    file_sink->set_formatter(std::move(formatter_file));

    // Create logger with both sinks
    GeneralLogger = std::make_shared<spdlog::logger>("Basic", spdlog::sinks_init_list{console_sink, file_sink});
    GeneralLogger->set_level(spdlog::level::info);
    
    // Register it as default logger
    spdlog::register_logger(GeneralLogger);

    std::vector<std::pair<std::filesystem::path, std::filesystem::file_time_type>> FileEntries;

    // Collect timestamps and files in Directory
    for (const auto& entry : std::filesystem::directory_iterator(LogPath))
    {
        if (entry.is_regular_file()) 
        {
            FileEntries.emplace_back(entry.path(), std::filesystem::last_write_time(entry));
        }
    }

    // Delete old log files
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
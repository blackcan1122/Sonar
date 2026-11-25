#include "Base/LoggingBackend.hpp"
#include "Base/Core.h"
#include "Base/CrashHandler.hpp"
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#ifdef _WIN32

#include <Windows.h>
#include <DbgHelp.h>

#pragma comment(lib, "Dbghelp.lib")

#include <atomic>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <eh.h>
#include <exception>
#include <mutex>
#include <sstream>

namespace
{
    std::atomic<bool> g_CrashHandlerInstalled = false;
    std::atomic<bool> g_IsHandlingCrash = false;

    void FlushCrashLog()
    {
        if (GeneralLogger)
        {
            GeneralLogger->flush();
        }
        spdlog::shutdown();
    }

    void WriteCrashLine(const std::string& Message)
    {
        if (GeneralLogger)
        {
            SPDLOG_LOGGER_CRITICAL(GeneralLogger, "{} {}", l_DEFAULT, Message);
        }
        else
        {
            std::cerr << Message << std::endl;
        }
    }

    std::string NarrowWideString(const wchar_t* Value)
    {
        if (Value == nullptr)
        {
            return "<null>";
        }

        const int BufferSize = WideCharToMultiByte(CP_UTF8, 0, Value, -1, nullptr, 0, nullptr, nullptr);
        if (BufferSize <= 0)
        {
            return "<conversion failed>";
        }

        std::string Result(static_cast<size_t>(BufferSize - 1), '\0');
        WideCharToMultiByte(CP_UTF8, 0, Value, -1, Result.data(), BufferSize, nullptr, nullptr);
        return Result;
    }

    bool BeginCrashHandling(const char* Reason)
    {
        bool Expected = false;
        if (!g_IsHandlingCrash.compare_exchange_strong(Expected, true))
        {
            return false;
        }

        WriteCrashLine(SONAR_FMT("================ CRASH DETECTED: {} ================", Reason));
        return true;
    }

    std::string DescribeExceptionCode(DWORD ExceptionCode)
    {
        switch (ExceptionCode)
        {
        case EXCEPTION_ACCESS_VIOLATION:
            return "EXCEPTION_ACCESS_VIOLATION";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
        case EXCEPTION_BREAKPOINT:
            return "EXCEPTION_BREAKPOINT";
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            return "EXCEPTION_DATATYPE_MISALIGNMENT";
        case EXCEPTION_FLT_DENORMAL_OPERAND:
            return "EXCEPTION_FLT_DENORMAL_OPERAND";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
        case EXCEPTION_FLT_INVALID_OPERATION:
            return "EXCEPTION_FLT_INVALID_OPERATION";
        case EXCEPTION_FLT_OVERFLOW:
            return "EXCEPTION_FLT_OVERFLOW";
        case EXCEPTION_FLT_STACK_CHECK:
            return "EXCEPTION_FLT_STACK_CHECK";
        case EXCEPTION_FLT_UNDERFLOW:
            return "EXCEPTION_FLT_UNDERFLOW";
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            return "EXCEPTION_ILLEGAL_INSTRUCTION";
        case EXCEPTION_IN_PAGE_ERROR:
            return "EXCEPTION_IN_PAGE_ERROR";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            return "EXCEPTION_INT_DIVIDE_BY_ZERO";
        case EXCEPTION_INT_OVERFLOW:
            return "EXCEPTION_INT_OVERFLOW";
        case EXCEPTION_INVALID_DISPOSITION:
            return "EXCEPTION_INVALID_DISPOSITION";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
            return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
        case EXCEPTION_PRIV_INSTRUCTION:
            return "EXCEPTION_PRIV_INSTRUCTION";
        case EXCEPTION_STACK_OVERFLOW:
            return "EXCEPTION_STACK_OVERFLOW";
        default:
            return SONAR_FMT("UNKNOWN_EXCEPTION_{:#x}", static_cast<unsigned long>(ExceptionCode));
        }
    }

    void EnsureSymbolsInitialized()
    {
        static std::once_flag SymbolsInitFlag;
        std::call_once(SymbolsInitFlag, []()
            {
                SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
                SymInitialize(GetCurrentProcess(), nullptr, TRUE);
            });
    }

    void WriteMiniDump(EXCEPTION_POINTERS* ExceptionInfo)
    {
        const auto Now = std::chrono::system_clock::now();
        const std::time_t Time = std::chrono::system_clock::to_time_t(Now);
        std::tm CurrentTime = *std::localtime(&Time);

        char NameBuffer[64];
        std::strftime(NameBuffer, sizeof(NameBuffer), "%Y%m%d_%H%M%S", &CurrentTime);
        const std::string DumpPath = fmt::format("logs/Sonar_crash_{}.dmp", NameBuffer);

        HANDLE DumpFile = CreateFileA(DumpPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (DumpFile == INVALID_HANDLE_VALUE)
        {
            WriteCrashLine(SONAR_FMT("Failed to create minidump file '{}'", DumpPath));
            return;
        }

        MINIDUMP_EXCEPTION_INFORMATION DumpExceptionInfo{};
        DumpExceptionInfo.ThreadId = GetCurrentThreadId();
        DumpExceptionInfo.ExceptionPointers = ExceptionInfo;
        DumpExceptionInfo.ClientPointers = FALSE;

        const BOOL DumpWritten = MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            DumpFile,
            MiniDumpNormal,
            ExceptionInfo ? &DumpExceptionInfo : nullptr,
            nullptr,
            nullptr);

        CloseHandle(DumpFile);

        if (DumpWritten)
        {
            WriteCrashLine(SONAR_FMT("Minidump written to '{}'", DumpPath));
        }
        else
        {
            WriteCrashLine(SONAR_FMT("MiniDumpWriteDump failed with error {}", GetLastError()));
        }
    }

    void WriteStackTraceFromContext(CONTEXT* Context)
    {
        if (Context == nullptr)
        {
            WriteCrashLine("No CPU context available for stack trace.");
            return;
        }

        EnsureSymbolsInitialized();

        STACKFRAME64 StackFrame{};
        DWORD MachineType = IMAGE_FILE_MACHINE_AMD64;

        StackFrame.AddrPC.Offset = Context->Rip;
        StackFrame.AddrPC.Mode = AddrModeFlat;
        StackFrame.AddrFrame.Offset = Context->Rbp;
        StackFrame.AddrFrame.Mode = AddrModeFlat;
        StackFrame.AddrStack.Offset = Context->Rsp;
        StackFrame.AddrStack.Mode = AddrModeFlat;

        HANDLE ProcessHandle = GetCurrentProcess();
        HANDLE ThreadHandle = GetCurrentThread();

        WriteCrashLine("Stack trace:");

        for (int FrameIndex = 0; FrameIndex < 64; ++FrameIndex)
        {
            const BOOL Walked = StackWalk64(
                MachineType,
                ProcessHandle,
                ThreadHandle,
                &StackFrame,
                Context,
                nullptr,
                SymFunctionTableAccess64,
                SymGetModuleBase64,
                nullptr);

            if (!Walked || StackFrame.AddrPC.Offset == 0)
            {
                break;
            }

            char SymbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME]{};
            auto* Symbol = reinterpret_cast<SYMBOL_INFO*>(SymbolBuffer);
            Symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            Symbol->MaxNameLen = MAX_SYM_NAME;

            DWORD64 Displacement = 0;
            IMAGEHLP_LINE64 LineInfo{};
            LineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            DWORD LineDisplacement = 0;

            std::ostringstream Builder;
            Builder << "  #" << FrameIndex << " 0x" << std::hex << StackFrame.AddrPC.Offset;

            if (SymFromAddr(ProcessHandle, StackFrame.AddrPC.Offset, &Displacement, Symbol))
            {
                Builder << " " << Symbol->Name;
                if (Displacement != 0)
                {
                    Builder << " + 0x" << std::hex << Displacement;
                }
            }
            else
            {
                Builder << " <unknown symbol>";
            }

            if (SymGetLineFromAddr64(ProcessHandle, StackFrame.AddrPC.Offset, &LineDisplacement, &LineInfo))
            {
                Builder << " (" << LineInfo.FileName << ":" << std::dec << LineInfo.LineNumber << ")";
            }

            WriteCrashLine(Builder.str());
        }
    }

    void WriteCurrentThreadStackTrace()
    {
        CONTEXT Context{};
        RtlCaptureContext(&Context);
        WriteStackTraceFromContext(&Context);
    }

    [[noreturn]] void TerminateAfterCrash()
    {
        FlushCrashLog();
        TerminateProcess(GetCurrentProcess(), 1);
        std::abort();
    }

    LONG WINAPI UnhandledExceptionFilterCallback(EXCEPTION_POINTERS* ExceptionInfo)
    {
        if (!BeginCrashHandling("Unhandled SEH exception"))
        {
            return EXCEPTION_EXECUTE_HANDLER;
        }

        if (ExceptionInfo && ExceptionInfo->ExceptionRecord)
        {
            const DWORD ExceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
            WriteCrashLine(SONAR_FMT("Exception code: {}", DescribeExceptionCode(ExceptionCode)));
            WriteCrashLine(SONAR_FMT("Fault address: {:#x}", reinterpret_cast<uintptr_t>(ExceptionInfo->ExceptionRecord->ExceptionAddress)));
        }

        WriteStackTraceFromContext(ExceptionInfo ? ExceptionInfo->ContextRecord : nullptr);
        WriteMiniDump(ExceptionInfo);
        FlushCrashLog();
        return EXCEPTION_EXECUTE_HANDLER;
    }

    void TerminateHandler()
    {
        if (!BeginCrashHandling("std::terminate"))
        {
            TerminateAfterCrash();
        }

        try
        {
            auto ExceptionPtr = std::current_exception();
            if (ExceptionPtr)
            {
                std::rethrow_exception(ExceptionPtr);
            }
        }
        catch (const std::exception& Exception)
        {
            WriteCrashLine(SONAR_FMT("Unhandled C++ exception: {}", Exception.what()));
        }
        catch (...)
        {
            WriteCrashLine("Unhandled non-standard C++ exception.");
        }

        WriteCurrentThreadStackTrace();
        WriteMiniDump(nullptr);
        TerminateAfterCrash();
    }

    void SignalHandler(int SignalNumber)
    {
        if (!BeginCrashHandling("CRT signal"))
        {
            TerminateAfterCrash();
        }

        WriteCrashLine(SONAR_FMT("Received signal {}", SignalNumber));
        WriteCurrentThreadStackTrace();
        WriteMiniDump(nullptr);
        TerminateAfterCrash();
    }

    void PureCallHandler()
    {
        if (!BeginCrashHandling("Pure virtual function call"))
        {
            TerminateAfterCrash();
        }

        WriteCurrentThreadStackTrace();
        WriteMiniDump(nullptr);
        TerminateAfterCrash();
    }

    void InvalidParameterHandler(const wchar_t* Expression, const wchar_t* Function, const wchar_t* File, unsigned int Line, uintptr_t)
    {
        if (!BeginCrashHandling("Invalid CRT parameter"))
        {
            TerminateAfterCrash();
        }

        WriteCrashLine(SONAR_FMT("Function: {}", NarrowWideString(Function)));
        WriteCrashLine(SONAR_FMT("File: {}", NarrowWideString(File)));
        WriteCrashLine(SONAR_FMT("Expression: {}", NarrowWideString(Expression)));
        WriteCrashLine(SONAR_FMT("Line: {}", Line));
        WriteCurrentThreadStackTrace();
        WriteMiniDump(nullptr);
        TerminateAfterCrash();
    }
}

#endif

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
                LOG_INFO(l_HOUSE_KEEPING, SONAR_FMT("Deleted: '{}'", FileEntries[i].first.string()));
            }
            catch (const std::filesystem::filesystem_error& e) 
            {
                LOG_ERROR(l_HOUSE_KEEPING, SONAR_FMT("Failed to Delete: '{}', Error Message: '{}'", 
                    FileEntries[i].first.string(), 
                    e.what()));
            }
        }
    }
}

void InstallCrashHandler()
{
#ifdef _WIN32
    bool Expected = false;
    if (!g_CrashHandlerInstalled.compare_exchange_strong(Expected, true))
    {
        return;
    }

    SetUnhandledExceptionFilter(&UnhandledExceptionFilterCallback);
    std::set_terminate(&TerminateHandler);
    std::signal(SIGABRT, &SignalHandler);
    std::signal(SIGSEGV, &SignalHandler);
    std::signal(SIGILL, &SignalHandler);
    std::signal(SIGFPE, &SignalHandler);
    _set_purecall_handler(&PureCallHandler);
    _set_invalid_parameter_handler(&InvalidParameterHandler);

    WriteCrashLine("Windows crash handler installed.");
#endif
}
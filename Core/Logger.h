#pragma once

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <format>
#include <string_view>
#include <iostream>

#define AOE_LOG(type, format, ...) aoe::Logger::Log(type, format, ##__VA_ARGS__)
#define AOE_LOG_INFO(format, ...) AOE_LOG(aoe::LogType::kInfo, format, ##__VA_ARGS__)
#define AOE_LOG_WARNING(format, ...) AOE_LOG(aoe::LogType::kWarning, format, ##__VA_ARGS__)
#define AOE_LOG_ERROR(format, ...) AOE_LOG(aoe::LogType::kError, format, ##__VA_ARGS__)
#define AOE_LOG_FATAL(format, ...) AOE_LOG(aoe::LogType::kFatal, format, ##__VA_ARGS__)

namespace aoe {

enum class LogType {
	kInfo,
	kWarning,
	kError,
    kFatal,
};

class Logger {
public:
    Logger() = delete;

    template<typename... TArgs>
    static void Log(LogType type, const char* format, TArgs&&... args) {
        Log(type, std::vformat(format, std::make_format_args(args...)));
    }

    static void Log(LogType type, std::string_view msg) {
        using namespace std::chrono;
        using clock = system_clock;

        const std::string timestamp = GetTimestamp();
        const std::string type_name = ToString(type);
        std::cout << std::format("[{}][{}] {}", timestamp, type_name, msg) << std::endl;
    }

private:
    static std::string GetTimestamp() {
        auto now = std::chrono::system_clock::now();
        return std::format("{0:%T}", now);
    }

    static const char* ToString(LogType type) {
        switch (type)
        {
        case aoe::LogType::kInfo:
            return "Info";
        case aoe::LogType::kWarning:
            return "Warning";
        case aoe::LogType::kError:
            break;
        case aoe::LogType::kFatal:
            return "Fatal";
        }

        return "Unknown";
    }
};

} // namespace aoe
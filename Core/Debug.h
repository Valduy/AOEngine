#pragma once

#include "Logger.h"

#ifndef NDEBUG

    #define AOE_ASSERT(expr) aoe::Debug::Assert(__FILE__, __LINE__, expr, #expr)

    #define AOE_ASSERT_MSG(expr, msg, ...) aoe::Debug::Assert(__FILE__, __LINE__, expr, #expr, msg, __VA_ARGS__)

#else

    #define AOE_ASSERT(expr) ((void)0)

    #define AOE_ASSERT_MSG(expr, msg, ...) ((void)0)

#endif // !NDEBUG

namespace aoe {

class Debug {
public:
    Debug() = delete;

    static void Assert(
        const char* file,
        unsigned int line,
        bool expr,
        const char* expr_str)
    {
        if (expr) {
            return;
        }

        AOE_LOG_FATAL(kAssertFormat, "no message", expr_str, file, line);
        abort();
    }

    static void Assert(
        const char* file,
        unsigned int line,
        bool expr,
        const char* expr_str,
        const char* msg)
    {
        if (expr) {
            return;
        }

        AOE_LOG_FATAL(kAssertFormat, msg, expr_str, file, line);
        abort();
    }

    template<typename... TArgs>
    static void Assert(
        const char* file,
        unsigned int line,
        bool expr,
        const char* expr_str,
        const char* msg,
        TArgs&&... args)
    {
        if (expr) {
            return;
        }

        AOE_LOG_FATAL(kAssertFormat,
            std::vformat(msg, std::make_format_args(args...)),
            expr_str,
            file,
            line);

        abort();
    }

private:
    static constexpr char kAssertFormat[] = 
        "\nAssert failed: {}\nExpected: {}\nSource: {}, line {}";
};

} // namespace aoe
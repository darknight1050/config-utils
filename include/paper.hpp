#pragma once

#include <fmt/core.h>

#include <iostream>

namespace Paper {
    enum class LogLevel : uint8_t { DBG = 3, INF = 4, WRN = 5, ERR = 6, CRIT = 7, OFF = 0 };
    constexpr auto format_as(LogLevel level) {
        switch (level) {
            case LogLevel::DBG:
                return "DEBUG";
            case LogLevel::INF:
                return "INFO";
            case LogLevel::WRN:
                return "WARN";
            case LogLevel::ERR:
                return "ERROR";
            case LogLevel::CRIT:
                return "CRITICAL";
            default:
                return "UNKNOWN";
        }
    }

    namespace Logger {
        inline void vfmtLog(fmt::string_view const str, LogLevel level, std::string_view const tag, fmt::format_args&& args) noexcept {
            std::cout << format_as(level) << " [" << tag << "] " << fmt::vformat(str, args) << "\n";
        }
        template <LogLevel lvl, typename... TArgs>
        constexpr auto fmtLogTag(fmt::format_string<TArgs...> str, std::string_view const tag, TArgs&&... args) {
            return vfmtLog(str, lvl, tag, fmt::make_format_args(args...));
        }
        template <typename... TArgs>
        inline auto error(fmt::format_string<TArgs...> const& s, TArgs&&... args) {
            return fmtLog<LogLevel::ERR, TArgs...>(s, std::forward<TArgs>(args)...);
        }
    }

    template <typename Str>
    struct BaseLoggerContext {
        Str tag;

        constexpr BaseLoggerContext(Str tag) : tag(std::move(tag)) {}
        constexpr BaseLoggerContext() noexcept = default;
        constexpr BaseLoggerContext(BaseLoggerContext&&) noexcept = default;
        constexpr BaseLoggerContext(BaseLoggerContext const&) noexcept = default;
        constexpr ~BaseLoggerContext() = default;

        BaseLoggerContext& operator=(BaseLoggerContext&& o) noexcept = default;
        BaseLoggerContext& operator=(BaseLoggerContext const& o) noexcept = default;

        template <LogLevel lvl, typename... TArgs>
        constexpr auto fmtLog(fmt::format_string<TArgs...> const& str, TArgs&&... args) const {
            return Logger::fmtLogTag<lvl, TArgs...>(str, tag, std::forward<TArgs>(args)...);
        }

        template <typename... TArgs>
        inline auto debug(fmt::format_string<TArgs...> const& s, TArgs&&... args) const {
            return this->fmtLog<LogLevel::DBG, TArgs...>(s, std::forward<TArgs>(args)...);
        }
        template <typename... TArgs>
        inline auto info(fmt::format_string<TArgs...> const& s, TArgs&&... args) const {
            return this->fmtLog<LogLevel::INF>(s, std::forward<TArgs>(args)...);
        }
        template <typename... TArgs>
        inline auto warn(fmt::format_string<TArgs...> const& s, TArgs&&... args) const {
            return this->fmtLog<LogLevel::WRN>(s, std::forward<TArgs>(args)...);
        }
        template <typename... TArgs>
        inline auto error(fmt::format_string<TArgs...> const& s, TArgs&&... args) const {
            return this->fmtLog<LogLevel::ERR>(s, std::forward<TArgs>(args)...);
        }
        template <typename... TArgs>
        inline auto critical(fmt::format_string<TArgs...> const& s, TArgs&&... args) const {
            return this->fmtLog<LogLevel::CRIT>(s, std::forward<TArgs>(args)...);
        }
    };
    template <std::size_t sz>
    struct ConstLoggerContext : public BaseLoggerContext<char[sz]> {
        constexpr ConstLoggerContext(char const (&s)[sz]) : BaseLoggerContext<char[sz]>() { std::copy(s, s + sz, BaseLoggerContext<char[sz]>::tag); }
    };
}

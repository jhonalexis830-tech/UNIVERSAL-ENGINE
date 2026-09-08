#pragma once

#include "ue/core/Types.hpp"

#include <sstream>
#include <string>

namespace ue::core {

/// Nivel de severidad de un mensaje de registro.
enum class LogLevel {
    Trace,
    Info,
    Warning,
    Error
};

/// Registro central de mensajes (Singleton básico).
/// Todo el motor escribe sus mensajes en español a través de este servicio.
class Log {
public:
    static Log& instance();

    /// Habilita color ANSI en la consola de Windows y fija la marca de tiempo.
    static void initialize();

    void log(LogLevel level, const std::string& message);
    void trace(const std::string& message) { log(LogLevel::Trace, message); }
    void info(const std::string& message)  { log(LogLevel::Info,  message); }
    void warn(const std::string& message)  { log(LogLevel::Warning, message); }
    void error(const std::string& message) { log(LogLevel::Error,  message); }

    template <typename... Args>
    void infoFmt(const std::string& format, Args&&... args) {
        // Formato básico tipo printf CSV (suficiente para Fase 1).
        std::ostringstream os;
        formatInto(os, format, std::forward<Args>(args)...);
        info(os.str());
    }

    template <typename... Args>
    void warnFmt(const std::string& format, Args&&... args) {
        std::ostringstream os;
        formatInto(os, format, std::forward<Args>(args)...);
        warn(os.str());
    }

    template <typename... Args>
    void errorFmt(const std::string& format, Args&&... args) {
        std::ostringstream os;
        formatInto(os, format, std::forward<Args>(args)...);
        error(os.str());
    }

private:
    Log() = default;

    template <typename... Args>
    static void formatInto(std::ostringstream& os, const std::string& format, Args&&... args);

    std::string levelPrefix(LogLevel level) const;
};

// ------------------------------------------------------------------
// Sustitución de marcadores "{}" por los argumentos dados.
// ------------------------------------------------------------------
namespace detail {
    template <typename T>
    void sinkInto(std::ostringstream& os, const T& value) {
        os << value;
    }
    inline void sinkInto(std::ostringstream& os, const std::string& value) {
        os << value;
    }
}

template <typename... Args>
void Log::formatInto(std::ostringstream& os, const std::string& format, Args&&... args) {
    std::size_t cursor = 0;
    std::size_t idx = 0;
    const auto next = [&](auto&& value) {
        const auto pos = format.find("{}", cursor);
        os << format.substr(cursor, pos - cursor);
        detail::sinkInto(os, value);
        cursor = (pos == std::string::npos) ? format.size() : pos + 2;
        ++idx;
    };
    (next(std::forward<Args>(args)), ...);
    os << format.substr(cursor);
}

} // namespace ue::core
#include "ue/core/Log.hpp"

#include <iostream>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace ue::core {

namespace {
    const char* ANSI_TRACE = "\033[90m";  // gris
    const char* ANSI_INFO  = "\033[94m";  // azul
    const char* ANSI_WARN  = "\033[93m";  // amarillo
    const char* ANSI_ERROR = "\033[91m";  // rojo
    const char* ANSI_RESET = "\033[0m";
}

Log& Log::instance() {
    static Log singleton;
    return singleton;
}

void Log::initialize() {
#if defined(_WIN32)
    // Permite secuencias ANSI en la terminal de Windows 10+.
    const HANDLE out = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if (out != INVALID_HANDLE_VALUE && out != nullptr) {
        DWORD mode = 0;
        if (::GetConsoleMode(out, &mode)) {
            ::SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }
#endif
}

std::string Log::levelPrefix(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace:   return "[RASTREO] ";
        case LogLevel::Info:    return "[INFO]    ";
        case LogLevel::Warning: return "[AVISO]   ";
        case LogLevel::Error:   return "[ERROR]   ";
    }
    return "[?] ";
}

void Log::log(LogLevel level, const std::string& message) {
    const char* color = ANSI_INFO;
    switch (level) {
        case LogLevel::Trace:   color = ANSI_TRACE; break;
        case LogLevel::Info:    color = ANSI_INFO;  break;
        case LogLevel::Warning: color = ANSI_WARN;  break;
        case LogLevel::Error:   color = ANSI_ERROR; break;
    }
    std::cout << color << levelPrefix(level) << message << ANSI_RESET << "\n";
    std::cout.flush();
}

} // namespace ue::core
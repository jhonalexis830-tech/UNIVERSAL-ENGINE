#include "ue/core/SystemPaths.hpp"
#include "ue/core/Log.hpp"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <sstream>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shlobj.h>
#include <direct.h>
#define UE_GETCWD _getcwd
#define UE_GETPID  _getpid
#else
#include <unistd.h>
#define UE_GETCWD getcwd
#define UE_GETPID  getpid
#endif

namespace ue::core {

namespace fs = std::filesystem;

std::string SystemPaths::homeDirectory() {
#if defined(_WIN32)
    char* profile = nullptr;
    size_t len = 0;
    if (_dupenv_s(&profile, &len, "USERPROFILE") == 0 && profile) {
        std::string result(profile);
        free(profile);
        return result;
    }
    if (profile) free(profile);
#else
    const char* home = std::getenv("HOME");
    if (home) return home;
#endif
    return ".";
}

std::string SystemPaths::userDirectory() {
    return homeDirectory() + "/Documents/UNIVERSAL ENGINE";
}

std::string SystemPaths::recordingsDirectory() {
    return userDirectory() + "/Recordings";
}

std::string SystemPaths::capturesDirectory() {
    return userDirectory() + "/Capturas";
}

std::string SystemPaths::projectsDirectory() {
    return userDirectory() + "/Proyectos";
}

std::string SystemPaths::engineDirectory() {
    return exeDirectory();
}

std::string SystemPaths::templatesDirectory() {
    return exeDirectory() + "/assets/templates";
}

std::string SystemPaths::exeDirectory() {
#if defined(_WIN32)
    HMODULE module = GetModuleHandleW(nullptr);
    if (module) {
        wchar_t wide[4096];
        const DWORD n = GetModuleFileNameW(module, wide, 4096);
        if (n > 0 && n < 4096) {
            return fs::path(wide).parent_path().string();
        }
    }
#endif
    char buffer[4096];
    if (UE_GETCWD(buffer, static_cast<int>(sizeof(buffer))) != nullptr) {
        return std::string(buffer);
    }
    return ".";
}

bool SystemPaths::ensureUserFolders() {
    const std::vector<std::string> folders = {
        userDirectory(),
        recordingsDirectory(),
        capturesDirectory(),
        projectsDirectory(),
    };
    bool ok = true;
    for (const auto& folder : folders) {
        std::error_code ec;
        fs::create_directories(folder, ec);
        if (ec) {
            Log::instance().warnFmt("No se pudo crear la carpeta '{}': {}", folder, ec.message());
            ok = false;
        }
    }
    return ok;
}

std::vector<std::string> SystemPaths::listFiles(const std::string& directory,
                                                const std::string& extension) {
    std::vector<std::string> out;
    std::error_code ec;
    if (!fs::exists(directory, ec)) {
        return out;
    }
    std::string ext = extension;
    if (!ext.empty() && ext.front() != '.') {
        ext = "." + ext;
    }
    for (const auto& entry : fs::directory_iterator(directory, ec)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const std::string path = entry.path().string();
        if (ext.empty() || path.size() >= ext.size() &&
            path.compare(path.size() - ext.size(), ext.size(), ext) == 0) {
            out.push_back(path);
        }
    }
    return out;
}

std::string SystemPaths::timestampName() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%04d%02d%02d_%02d%02d%02d",
                  tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                  tm.tm_hour, tm.tm_min, tm.tm_sec);
    return std::string(buffer);
}

std::string SystemPaths::userName() {
#if defined(_WIN32)
    char* user = nullptr;
    size_t len = 0;
    if (_dupenv_s(&user, &len, "USERNAME") == 0 && user) {
        std::string result(user);
        free(user);
        return result;
    }
    if (user) free(user);
#else
    const char* user = std::getenv("USER");
    if (user) return user;
#endif
    return "usuario";
}

} // namespace ue::core
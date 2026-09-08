cmake_minimum_required(VERSION 3.20)
project(UniversalEngine VERSION 0.1.0 LANGUAGES CXX)

# ============================================================
#  UNIVERSAL ENGINE  —  TEAM S.H.O.T. WARE
#  Motor gráfico 3D multi-herramienta (inspirado en Unity y Blender)
#  Fase 1: núcleo SOLID + editor con viewport 3D funcional
# ============================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Tipo de compilación" FORCE)
endif()

set(UE_BUILD_EDITOR ON CACHE BOOL "Construir la aplicación de edición (EditorApp)")

option(UE_ENABLE_SANITIZERS "Habilitar sanitizadores en Debug (MSVC/Clang)" OFF)

# Ejecutable que funcione sin requerir la instalación del runtime de VS.
# En Windows/MSVC fija /MT (estático); en otros compiladores busca -static.
option(UE_STATIC_RUNTIME "Compilar con el runtime C/C++ enlazado estáticamente" ON)
if(UE_STATIC_RUNTIME)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    elseif(NOT APPLE)
        add_compile_options(-static-libgcc -static-libstdc++)
    endif()
endif()

# ------------------------------------------------------------
# Dependencias externas (se descargan automáticamente)
# ------------------------------------------------------------
include(FetchContent)

# --- GLFW (ventanas / contexto OpenGL / entrada) -------------
FetchContent_Declare(
    glfw
    URL      https://github.com/glfw/glfw/archive/refs/tags/3.3.9.tar.gz
    URL_HASH SHA256=4cf729474f81f37e2e2e6d5beab51c9f8ee9f15c8d49b6224d1fb6e2637dc680
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

# --- Dear ImGui (UI del editor) ------------------------------
FetchContent_Declare(
    imgui
    URL      https://github.com/ocornut/imgui/archive/refs/tags/v1.90.7.tar.gz
    URL_HASH SHA256=d636586e597dd4491a75b254fd3ece6b996132a23d03721d2fee12e0fb2f0d7a
)

# --- nlohmann/json (serialización de escenas) -----------------
FetchContent_Declare(
    nlohmann_json
    URL      https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.tar.gz
    URL_HASH SHA256=0d8ef5af7f9794e3263480193c4915492b6ee6b68887203563c5b709c54b54ea
)

FetchContent_MakeAvailable(glfw imgui nlohmann_json)

# ------------------------------------------------------------
# Motor (biblioteca)
# ------------------------------------------------------------
add_subdirectory(engine)

# ------------------------------------------------------------
# Editor (aplicación)
# ------------------------------------------------------------
if(UE_BUILD_EDITOR)
    add_subdirectory(apps/editor)
endif()
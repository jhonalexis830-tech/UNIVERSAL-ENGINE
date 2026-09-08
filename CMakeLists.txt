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
    URL_HASH SHA256=a7e7faef424fcb5f83d8faecf9d697a338da7f7a906fc1afbc0e1879ef31bd53
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

# --- Dear ImGui (UI del editor) ------------------------------
FetchContent_Declare(
    imgui
    URL      https://github.com/ocornut/imgui/archive/refs/tags/v1.90.7.tar.gz
    URL_HASH SHA256=872574217643d4ad7e9e6df420bb8d9e0d468fb90641c2bf50fd61745e05de99
)

# --- nlohmann/json (serialización de escenas) -----------------
FetchContent_Declare(
    nlohmann_json
URL      https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.tar.gz
    URL_HASH SHA256=0d8ef5af7f9794e3263480193c491549b2ba6cc74bb018906202ada498a79406
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